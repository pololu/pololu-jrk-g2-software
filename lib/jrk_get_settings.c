// Function for reading settings from the Jrk over USB.

#include "jrk_internal.h"

static void write_buffer_to_settings(const uint8_t * buf, jrk_settings * settings)
{
  // Beginning of auto-generated buffer-to-settings code.
  // End of auto-generated buffer-to-settings code.
}

jrk_error * jrk_get_settings(jrk_handle * handle, jrk_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  uint8_t product = jrk_device_get_product(jrk_handle_get_device(handle));

  // Allocate the new settings object.
  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_settings_create(&new_settings);
  }

  // Specify what product these settings are for.
  if (error == NULL)
  {
    jrk_settings_set_product(new_settings, product);
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[JRK_SETTINGS_SIZE];
  {
    memset(buf, 0, sizeof(buf));
    size_t index = 1;
    while (index < sizeof(buf) && error == NULL)
    {
      size_t length = JRK_MAX_USB_RESPONSE_SIZE;
      if (index + length > sizeof(buf))
      {
        length = sizeof(buf) - index;
      }
      error = jrk_get_setting_segment(handle, index, length, buf + index);
      index += length;
    }
  }

  // Store the settings in the new settings object.
  if (error == NULL)
  {
    write_buffer_to_settings(buf, new_settings);
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading settings from the device.");
  }

  return error;
}
