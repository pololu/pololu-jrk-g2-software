#include "jrk_internal.h"

static void jrk_write_settings_to_buffer(const jrk_settings * settings, uint8_t * buf)
{
  assert(settings != NULL);
  assert(buf != NULL);

  // Beginning of auto-generated settings-to-buffer code.
  // End of auto-generated settings-to-buffer code.
}

jrk_error * jrk_set_settings(jrk_handle * handle, const jrk_settings * settings)
{
  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  if (settings == NULL)
  {
    return jrk_error_create("Settings object is null.");
  }

  jrk_error * error = NULL;

  jrk_settings * fixed_settings = NULL;

  // Copy the settings so we can fix them without modifying the input ones,
  // which would be surprising to the caller.
  if (error == NULL)
  {
    error = jrk_settings_copy(settings, &fixed_settings);
  }

  // Set the product code of the settings and fix the settings silently to make
  // sure we don't apply invalid settings to the device.  A good app will set
  // the product and call jrk_settings_fix on its own before calling this
  // function, so there should be nothing to fix here.
  if (error == NULL)
  {
    uint8_t product = jrk_device_get_product(jrk_handle_get_device(handle));
    jrk_settings_set_product(fixed_settings, product);

    error = jrk_settings_fix(fixed_settings, NULL);
  }

  // Construct a buffer holding the bytes we want to write.
  uint8_t buf[JRK_SETTINGS_SIZE];
  memset(buf, 0, sizeof(buf));
  jrk_write_settings_to_buffer(fixed_settings, buf);

  // Write the bytes to the device.
  for (uint8_t i = 1; i < sizeof(buf) && error == NULL; i++)
  {
    error = jrk_set_setting_byte(handle, i, buf[i]);
  }

  jrk_settings_free(fixed_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error applying settings to the device.");
  }

  return error;
}
