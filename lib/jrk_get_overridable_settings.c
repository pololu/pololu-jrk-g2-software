// Function for reading settings from the Jrk over USB.

#include "jrk_internal.h"

static void write_buffer_to_overridable_settings(const uint8_t * buf,
  jrk_overridable_settings * settings)
{
  // Beginning of auto-generated buffer-to-settings code.
  // End of auto-generated buffer-to-settings code.
}

jrk_error * jrk_get_overridable_settings(jrk_handle * handle,
  jrk_overridable_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Overridable settings output pointer is null.");
  }

  *settings = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  // Allocate the new settings object.
  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_overridable_settings_create(&new_settings);
  }

  // Read all the settings from the device into a buffer.
  uint8_t buf[JRK_SETTINGS_SIZE - JRK_OVERRIDABLE_SETTINGS_START];
  memset(buf, 0, sizeof(buf));
  if (error == NULL)
  {
    error = jrk_get_overridable_setting_segment(handle, 0, sizeof(buf), buf);
  }

  // Store the settings in the new settings object.
  if (error == NULL)
  {
    write_buffer_to_overridable_settings(buf, new_settings);
  }

  // Pass the new settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading settings from the device.");
  }

  return error;
}
