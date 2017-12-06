// Functions for listing jrks connected over USB and getting their properties.

#include "jrk_internal.h"

struct jrk_device
{
  libusbp_generic_interface * usb_interface;
  char * serial_number;
  char * os_id;
  uint16_t firmware_version;
  uint8_t product;
};

jrk_error * jrk_list_connected_devices(
  jrk_device *** device_list,
  size_t * device_count)
{
  if (device_count) { *device_count = 0; }

  if (device_list == NULL)
  {
    return jrk_error_create("Device list output pointer is null.");
  }

  *device_list = NULL;

  jrk_error * error = NULL;

  libusbp_device ** usb_device_list = NULL;
  size_t usb_device_count = 0;
  if (error == NULL)
  {
    error = jrk_usb_error(libusbp_list_connected_devices(
        &usb_device_list, &usb_device_count));
  }

  jrk_device ** jrk_device_list = NULL;
  size_t jrk_device_count = 0;
  if (error == NULL)
  {
    // Allocate enough memory for the case where every USB device is
    // relevant, without forgetting the NULL terminator.
    jrk_device_list = calloc(usb_device_count + 1, sizeof(jrk_device *));
    if (jrk_device_list == NULL)
    {
      error = &jrk_error_no_memory;
    }
  }

  for (size_t i = 0; error == NULL && i < usb_device_count; i++)
  {
    libusbp_device * usb_device = usb_device_list[i];

    // Check the USB vendor ID.
    uint16_t vendor_id;
    error = jrk_usb_error(libusbp_device_get_vendor_id(usb_device, &vendor_id));
    if (error) { break; }
    if (vendor_id != JRK_VENDOR_ID) { continue; }

    // Check the USB product ID.
    uint16_t product_id;
    error = jrk_usb_error(libusbp_device_get_product_id(usb_device, &product_id));
    if (error) { break; }
    if (product_id != JRK_PRODUCT_ID_2017)
    {
      continue;
    }

    // Get the USB interface.
    libusbp_generic_interface * usb_interface = NULL;
    {
      uint8_t interface_number = 0;
      bool composite = true;
      libusbp_error * usb_error = libusbp_generic_interface_create(
        usb_device, interface_number, composite, &usb_interface);
      if (usb_error)
      {
        if (libusbp_error_has_code(usb_error, LIBUSBP_ERROR_NOT_READY))
        {
          // An error occurred that is normal if the interface is simply
          // not ready to use yet.  Silently ignore this device.
          continue;
        }
        error = jrk_usb_error(usb_error);
        break;
      }
    }

    // Allocate the new device.
    jrk_device * new_device = calloc(1, sizeof(jrk_device));
    if (new_device == NULL)
    {
      error = &jrk_error_no_memory;
      break;
    }
    jrk_device_list[jrk_device_count++] = new_device;

    // Store the USB interface.  Must do this here so that it will get freed
    // if any of the calls below fail.
    new_device->usb_interface = usb_interface;

    // Get the serial number.
    error = jrk_usb_error(libusbp_device_get_serial_number(
        usb_device, &new_device->serial_number));
    if (error) { break; }

    // Get the OS ID.
    error = jrk_usb_error(libusbp_device_get_os_id(
        usb_device, &new_device->os_id));
    if (error) { break; }

    // Get the firmware version.
    error = jrk_usb_error(libusbp_device_get_revision(
        usb_device, &new_device->firmware_version));
    if (error) { break; }

    // Get the product code.
    switch (product_id)
    {
    case JRK_PRODUCT_ID_2017:
      new_device->product = JRK_PRODUCT_2017;
      break;
    }
    if (new_device->product == 0)
    {
      // Should not ever happen.
      error = jrk_error_create("Unknown product.");
      break;
    }
  }

  if (error == NULL)
  {
    // Success.  Give the list to the caller.
    *device_list = jrk_device_list;
    if (device_count) { *device_count = jrk_device_count; }
    jrk_device_list = NULL;
    jrk_device_count = 0;
  }

  for (size_t i = 0; i < jrk_device_count; i++)
  {
    jrk_device_free(jrk_device_list[i]);
  }

  jrk_list_free(jrk_device_list);

  for (size_t i = 0; i < usb_device_count; i++)
  {
    libusbp_device_free(usb_device_list[i]);
  }

  libusbp_list_free(usb_device_list);

  return error;
}

void jrk_list_free(jrk_device ** list)
{
  free(list);
}

jrk_error * jrk_device_copy(const jrk_device * source, jrk_device ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Device output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_device * new_device = calloc(1, sizeof(jrk_device));
  if (new_device == NULL)
  {
    error = &jrk_error_no_memory;
  }

  if (error == NULL)
  {
    error = jrk_usb_error(libusbp_generic_interface_copy(
        source->usb_interface, &new_device->usb_interface));
  }

  if (error == NULL)
  {
    new_device->product = source->product;
  }

  if (error == NULL)
  {
    new_device->serial_number = strdup(source->serial_number);
    if (new_device->serial_number == NULL)
    {
      error = &jrk_error_no_memory;
    }
  }

  if (error == NULL)
  {
    new_device->os_id = strdup(source->os_id);
    if (new_device->os_id == NULL)
    {
      error = &jrk_error_no_memory;
    }
  }

  if (error == NULL)
  {
    new_device->firmware_version = source->firmware_version;
    new_device->product = source->product;
  }

  if (error == NULL)
  {
    // Success.  Give the copy to the caller.
    *dest = new_device;
    new_device = NULL;
  }

  jrk_device_free(new_device);

  return error;
}

void jrk_device_free(jrk_device * device)
{
  if (device != NULL)
  {
    libusbp_generic_interface_free(device->usb_interface);
    libusbp_string_free(device->os_id);
    libusbp_string_free(device->serial_number);
    free(device);
  }
}

uint8_t jrk_device_get_product(const jrk_device * device)
{
  if (device == NULL) { return 0; }
  return device->product;
}

const char * jrk_device_get_name(const jrk_device * device)
{
  if (device == NULL) { return ""; }
  return jrk_look_up_product_name_ui(device->product);
}

const char * jrk_device_get_short_name(const jrk_device * device)
{
  if (device == NULL) { return ""; }
  return jrk_look_up_product_name_short(device->product);
}

const char * jrk_device_get_serial_number(const jrk_device * device)
{
  if (device == NULL) { return ""; }
  return device->serial_number;
}

const char * jrk_device_get_os_id(const jrk_device * device)
{
  if (device == NULL) { return ""; }
  return device->os_id;
}

uint16_t jrk_device_get_firmware_version(const jrk_device * device)
{
  if (device == NULL) { return 0xFFFF; }
  return device->firmware_version;
}

const libusbp_generic_interface *
jrk_device_get_generic_interface(const jrk_device * device)
{
  if (device == NULL) { return NULL; }
  return device->usb_interface;
}
