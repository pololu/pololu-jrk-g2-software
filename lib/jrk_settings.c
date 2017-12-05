#include "jrk_internal.h"

struct jrk_settings
{
  uint32_t product;

  // Beginning of auto-generated settings struct members.

  uint8_t input_mode;
  uint16_t input_minimum;

  // End of auto-generated settings struct members.
};

void jrk_settings_fill_with_defaults(jrk_settings * settings)
{
  assert(settings != NULL);

  uint32_t product = settings->product;

  // The product should be set beforehand, and if it is not then we should do
  // nothing.
  if (product != JRK_PRODUCT_2017)
  {
    return;
  }

  // Reset all fields to zero.
  memset(settings, 0, sizeof(jrk_settings));

  // TODO: finish filling in the default settings
}

jrk_error * jrk_settings_create(jrk_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  jrk_error * error = NULL;

  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_settings *)calloc(1, sizeof(jrk_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}

void jrk_settings_free(jrk_settings * settings)
{
  free(settings);
}

jrk_error * jrk_settings_copy(const jrk_settings * source, jrk_settings ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_settings *)calloc(1, sizeof(jrk_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_settings, source, sizeof(jrk_settings));
  }

  if (error == NULL)
  {
    *dest = new_settings;
    new_settings = NULL;
  }

  jrk_settings_free(new_settings);

  return error;
}

void jrk_settings_set_product(jrk_settings * settings, uint32_t product)
{
  if (!settings) { return; }
  settings->product = product;
}

uint32_t jrk_settings_get_product(const jrk_settings * settings)
{
  if (!settings) { return 0; }
  return settings->product;
}

// Beginning of auto-generated settings accessors.

void jrk_settings_set_input_mode(jrk_settings * settings, uint8_t input_mode)
{
  if (settings == NULL) { return; }
  settings->input_mode = input_mode;
}

uint8_t jrk_settings_get_input_mode(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_mode;
}

void jrk_settings_set_input_minimum(jrk_settings * settings, uint16_t input_minimum)
{
  if (settings == NULL) { return; }
  settings->input_minimum = input_minimum;
}

uint16_t jrk_settings_get_input_minimum(const jrk_settings * settings)
{
  if (settings == NULL) { return 0; }
  return settings->input_minimum;
}

// End of auto-generated settings accessors.
