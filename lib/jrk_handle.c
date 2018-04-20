// Functions for communicating with jrks over USB.

#include "jrk_internal.h"

struct jrk_handle
{
  libusbp_generic_handle * usb_handle;
  jrk_device * device;
  char * cached_firmware_version_string;
};

jrk_error * jrk_handle_open(const jrk_device * device, jrk_handle ** handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle output pointer is null.");
  }

  *handle = NULL;

  if (device == NULL)
  {
    return jrk_error_create("Device is null.");
  }

  jrk_error * error = NULL;

  if (error == NULL)
  {
    uint16_t version = jrk_device_get_firmware_version(device);
    uint8_t version_major = version >> 8;
    if (version_major > JRK_FIRMWARE_VERSION_MAJOR_MAX)
    {
      error = jrk_error_create(
        "The device has new firmware that is not supported by this software.  "
        "Try using the latest version of this software from " DOCUMENTATION_URL);
    }
  }

  jrk_handle * new_handle = NULL;
  if (error == NULL)
  {
    new_handle = calloc(1, sizeof(jrk_handle));
    if (new_handle == NULL)
    {
      error = &jrk_error_no_memory;
    }
  }

  if (error == NULL)
  {
    error = jrk_device_copy(device, &new_handle->device);
  }


  if (error == NULL)
  {
    const libusbp_generic_interface * usb_interface =
      jrk_device_get_generic_interface(device);
    error = jrk_usb_error(libusbp_generic_handle_open(
        usb_interface, &new_handle->usb_handle));
  }

  if (error == NULL)
  {
    // Set a timeout for all control transfers to prevent the program from
    // hanging indefinitely.  Want it to be at least 1500 ms because that is how
    // long the jrk might take to respond after restoring its settings to their
    // defaults.
    error = jrk_usb_error(libusbp_generic_handle_set_timeout(
        new_handle->usb_handle, 0, 1600));
  }

  if (error == NULL)
  {
    // Success.  Pass the handle to the caller.
    *handle = new_handle;
    new_handle = NULL;
  }

  jrk_handle_close(new_handle);

  return error;
}

void jrk_handle_close(jrk_handle * handle)
{
  if (handle != NULL)
  {
    libusbp_generic_handle_close(handle->usb_handle);
    jrk_device_free(handle->device);
    free(handle->cached_firmware_version_string);
    free(handle);
  }
}

const jrk_device * jrk_handle_get_device(const jrk_handle * handle)
{
  if (handle == NULL) { return NULL; }
  return handle->device;
}

const char * jrk_get_firmware_version_string(jrk_handle * handle)
{
  if (handle == NULL) { return ""; }

  if (handle->cached_firmware_version_string != NULL)
  {
    return handle->cached_firmware_version_string;
  }

  // Allocate memory for the string.
  // - Initial part, e.g. "99.99": up to 5 bytes
  // - Modification string: up to 127 bytes
  // - Null terminator: 1 byte
  char * new_string = malloc(133);
  if (new_string == NULL)
  {
    return "";
  }

  size_t index = 0;

  // Format the firmware version number and put it in the string.
  uint16_t version_bcd = jrk_device_get_firmware_version(handle->device);

  if (version_bcd & 0xF000)
  {
    new_string[index++] = '0' + (version_bcd >> 12 & 0xF);
  }
  new_string[index++] = '0' + (version_bcd >> 8 & 0xF);
  new_string[index++] = '.';
  new_string[index++] = '0' + (version_bcd >> 4 & 0xF);
  new_string[index++] = '0' + (version_bcd >> 0 & 0xF);

  // Get the firmware modification string from the device.
  size_t transferred = 0;
  uint8_t buffer[256];
  libusbp_error * usb_error = libusbp_control_transfer(handle->usb_handle,
    0x80, USB_REQUEST_GET_DESCRIPTOR,
    (USB_DESCRIPTOR_TYPE_STRING << 8) | JRK_FIRMWARE_MODIFICATION_STRING_INDEX,
    0,
    buffer, sizeof(buffer), &transferred);
  if (usb_error)
  {
    // Let's make this be a non-fatal error because it's not so important.
    // Just add a question mark so we can tell if something is wrong.
    new_string[index++] = '0';
  }

  // Ignore the modification string if it is just a dash.
  if (transferred == 4 && buffer[2] == '-')
  {
    transferred = 0;
  }

  // Add the modification string to the firmware version string.
  for (size_t i = 2; i < transferred; i += 2)
  {
    new_string[index++] = buffer[i];
  }

  new_string[index] = 0;

  handle->cached_firmware_version_string = new_string;

  return new_string;
}

jrk_error * jrk_set_eeprom_setting_byte(jrk_handle * handle,
  uint8_t address, uint8_t byte)
{
  assert(handle != NULL);

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_SET_EEPROM_SETTING, byte, address, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error applying settings.");
  }
  return error;
}

jrk_error * jrk_set_target(jrk_handle * handle, uint16_t target)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  // Cap the value since the firmware doesn't cap it, it could concievably
  // cause some minor problems if people set targets outside of the allowed
  // range, and the Arduino library does it.
  if (target > 4095)
  {
    target = 4095;
  }

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_SET_TARGET_USB, target, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error setting the target.");
  }

  return error;
}

jrk_error * jrk_stop_motor(jrk_handle * handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_MOTOR_OFF_USB, 0, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error stopping the motor.");
  }

  return error;
}

jrk_error * jrk_clear_errors(jrk_handle * handle, uint16_t * error_flags)
{
  if (error_flags != NULL)
  {
    *error_flags = 0;
  }

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  // Get the halting error flags and clear them at the same time.
  uint8_t buffer[2];
  uint16_t flags = 1 << JRK_GET_VARIABLES_FLAG_CLEAR_ERROR_FLAGS_HALTING;
  jrk_error * error = jrk_get_variable_segment(handle,
    JRK_VAR_ERROR_FLAGS_HALTING, 2, buffer, flags);

  if (error == NULL && error_flags != NULL)
  {
    *error_flags = buffer[0] | (buffer[1] << 8);
  }

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error while clearing errors.");
  }

  return error;
}

jrk_error * jrk_run_motor(jrk_handle * handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  // Get the target value and clear halting errors at the same time.
  uint8_t buffer[2];
  if (error == NULL)
  {
    uint16_t flags = 1 << JRK_GET_VARIABLES_FLAG_CLEAR_ERROR_FLAGS_HALTING;
    error = jrk_get_variable_segment(handle, JRK_VAR_TARGET, 2, buffer, flags);
  }
  uint16_t target = buffer[0] | (buffer[1] << 8);

  // Send a "Set target" command with the same target value to clear the
  // "Awaiting command" error bit.
  if (error == NULL)
  {
    error = jrk_set_target(handle, target);
  }

  return error;
}

jrk_error * jrk_force_duty_cycle_target(jrk_handle * handle, int16_t duty_cycle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (duty_cycle > 600) { duty_cycle = 600; }
  if (duty_cycle < -600) { duty_cycle = -600; }

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_FORCE_DUTY_CYCLE_TARGET, duty_cycle, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error forcing the duty cycle target.");
  }

  return error;
}

jrk_error * jrk_force_duty_cycle(jrk_handle * handle, int16_t duty_cycle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (duty_cycle > 600) { duty_cycle = 600; }
  if (duty_cycle < -600) { duty_cycle = -600; }

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_FORCE_DUTY_CYCLE, duty_cycle, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error forcing the duty cycle.");
  }

  return error;
}

jrk_error * jrk_get_eeprom_setting_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * output)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (output == NULL)
  {
    return jrk_error_create("Setting output pointer is null.");
  }

  if (index > 0xFF)
  {
    // The firmware would ignore the high bits if we tried to send this.
    return jrk_error_create("Setting segment index is too large.");
  }

  if (length == 0)
  {
    return jrk_error_create("Setting segment length is zero.");
  }

  if (length > JRK_MAX_USB_RESPONSE_SIZE)
  {
    return jrk_error_create(
      "Setting segment length is too large.");
  }

  size_t transferred;
  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0xC0, JRK_CMD_GET_EEPROM_SETTINGS, 0, index, output, length, &transferred));
  if (error != NULL)
  {
    error = jrk_error_add(error, "There was an error reading settings.");
    return error;
  }

  if (transferred != length)
  {
    return jrk_error_create(
      "Failed to read settings.  Expected %u bytes, got %u.\n",
      (unsigned int)length, (unsigned int)transferred);
  }

  return NULL;
}

jrk_error * jrk_get_ram_setting_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * output)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (output == NULL)
  {
    return jrk_error_create("RAM setting output pointer is null.");
  }

  if (index > 0xFF)
  {
    // The firmware would ignore the high bits if we tried to send this.
    return jrk_error_create(
      "RAM setting segment index is too large.");
  }

  if (length == 0)
  {
    return jrk_error_create(
      "RAM setting segment length is zero.");
  }

  if (length > JRK_MAX_USB_RESPONSE_SIZE)
  {
    return jrk_error_create(
      "RAM setting segment length is too large.");
  }

  size_t transferred;
  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0xC0, JRK_CMD_GET_RAM_SETTINGS, 0, index,
    output, length, &transferred));
  if (error != NULL)
  {
    error = jrk_error_add(error, "There was an error reading RAM settings.");
    return error;
  }

  if (transferred != length)
  {
    return jrk_error_create(
      "Failed to read RAM settings.  Expected %u bytes, got %u.\n",
      (unsigned int)length, (unsigned int)transferred);
  }

  return NULL;
}

jrk_error * jrk_set_ram_setting_segment(jrk_handle * handle,
  size_t index, size_t length, const uint8_t * input)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (input == NULL)
  {
    return jrk_error_create("RAM setting input pointer is null.");
  }

  if (index > 0xFF)
  {
    // The firmware would ignore the high bits if we tried to send this.
    return jrk_error_create(
      "RAM setting index is too large.");
  }

  if (length == 0)
  {
    return jrk_error_create(
      "RAM setting segment length is zero.");
  }

  if (length > JRK_MAX_USB_RESPONSE_SIZE)
  {
    return jrk_error_create(
      "RAM setting segment length is too large.");
  }

  size_t transferred;
  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_SET_RAM_SETTINGS, 0, index,
    (uint8_t *)input, length, &transferred));
  if (error != NULL)
  {
    error = jrk_error_add(error, "There was an error settings RAM settings.");
    return error;
  }

  if (transferred != length)
  {
    return jrk_error_create(
      "Failed to set RAM settings.  Expected %u bytes, got %u.\n",
      (unsigned int)length, (unsigned int)transferred);
  }

  return NULL;
}

jrk_error * jrk_get_variable_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * output, uint16_t flags)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (output == NULL)
  {
    return jrk_error_create("Variable output pointer is null.");
  }

  if (index > 0xFF)
  {
    // The firmware would ignore the high bits if we tried to send this.
    return jrk_error_create(
      "Variable segment index is too large.");
  }

  if (length == 0)
  {
    return jrk_error_create("Variable segment length is zero.");
  }

  if (length > JRK_MAX_USB_RESPONSE_SIZE)
  {
    return jrk_error_create("Variable length is too large.");
  }

  size_t transferred;
  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0xC0, JRK_CMD_GET_VARIABLES, flags, index, output, length, &transferred));
  if (error != NULL)
  {
    error = jrk_error_add(error, "There was an error reading variables.");
    return error;
  }

  if (transferred != length)
  {
    return jrk_error_create(
      "Failed to read variables.  Expected %u bytes, got %u.\n",
      (unsigned int)length, (unsigned int)transferred);
  }

  return NULL;
}

jrk_error * jrk_restore_defaults(jrk_handle * handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  if (error == NULL)
  {
    error = jrk_set_eeprom_setting_byte(handle, JRK_SETTING_NOT_INITIALIZED, 1);
  }

  if (error == NULL)
  {
    error = jrk_reinitialize(handle);
  }

  // The request returns before the settings are actually initialized.
  // Wait until the device succeeds in reinitializing its settings.
  if (error == NULL)
  {
    time_t start = time(0);
    while (true)
    {
      usleep(10000);

      uint8_t not_initialized;
      error = jrk_get_eeprom_setting_segment(handle, JRK_SETTING_NOT_INITIALIZED,
        1, &not_initialized);
      if (error != NULL)
      {
        // Communication error
        break;
      }

      if (!not_initialized)
      {
        // Success
        break;
      }

      if (difftime(time(0), start) >= 3)
      {
        // Time out after 2 to 3 seconds.
        error = jrk_error_create("The device took too long to finish.");
        break;
      }
    }
  }

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error restoring the default settings.");
  }

  return error;
}

jrk_error * jrk_reinitialize(jrk_handle * handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  // In this function, we preserve errors.  If we ever want the behavior of the
  // old jrks for some reason, make a new function called
  // jrk_reinitialize_and_reset_errors.
  uint16_t flags = 1 << JRK_REINITIALIZE_FLAG_PRESERVE_ERRORS;

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_REINITIALIZE, flags, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reinitializing the device.");
  }

  return error;
}

jrk_error * jrk_start_bootloader(jrk_handle * handle)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = jrk_usb_error(libusbp_control_transfer(handle->usb_handle,
    0x40, JRK_CMD_START_BOOTLOADER, 0, 0, NULL, 0, NULL));

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error starting the bootloader.");
  }

  return error;
}

jrk_error * jrk_get_debug_data(jrk_handle * handle, uint8_t * data, size_t * size)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (data == NULL)
  {
    return jrk_error_create("Data output pointer is null.");
  }

  if (size == NULL)
  {
    return jrk_error_create("Size output pointer is null.");
  }

  size_t transferred;
  libusbp_error * usb_error = libusbp_control_transfer(handle->usb_handle,
    0xC0, JRK_CMD_GET_DEBUG_DATA, 0, 0, data, *size, &transferred);
  if (usb_error)
  {
    *size = 0;
    return jrk_usb_error(usb_error);
  }

  *size = transferred;

  return NULL;
}
