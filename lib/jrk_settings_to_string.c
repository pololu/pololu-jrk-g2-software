#include "jrk_internal.h"

jrk_error * jrk_settings_to_string(const jrk_settings * settings, char ** string)
{
  if (string == NULL)
  {
    return jrk_error_create("String output pointer is null.");
  }

  *string = NULL;

  if (settings == NULL)
  {
    return jrk_error_create("Settings pointer is null.");
  }

  jrk_error * error = NULL;

  jrk_string str;
  jrk_string_setup(&str);

  jrk_sprintf(&str, "# Pololu jrk settings file.\n");
  jrk_sprintf(&str, "# " DOCUMENTATION_URL "\n");

  uint8_t product = jrk_settings_get_product(settings);

  {
    const char * product_str = jrk_look_up_product_name_short(product);
    jrk_sprintf(&str, "product: %s\n", product_str);
  }

  // Beginning of auto-generated settings file printing code.

  {
    uint8_t input_mode = jrk_settings_get_input_mode(settings);
    const char * value_str = "";
    jrk_code_to_name(jrk_input_mode_names_short, input_mode, &value_str);
    jrk_sprintf(&str, "input_mode: %s", value_str);
  }

  {
    uint16_t input_minimum = jrk_settings_get_input_minimum(settings);
    jrk_sprintf(&str, "input_minimum: %u", input_minimum);
  }

  // End of auto-generated settings file printing code.

  if (error == NULL && str.data == NULL)
  {
    error = &jrk_error_no_memory;
  }

  if (error == NULL)
  {
    *string = str.data;
    str.data = NULL;
  }

  jrk_string_free(str.data);

  return error;
}
