#include "jrk_internal.h"

static bool enum_is_valid(uint8_t code, uint8_t * valid_codes, uint8_t code_count)
{
  for (uint32_t i = 0; i < code_count; i++)
  {
    if (code == valid_codes[i]) { return true; }
  }
  return false;
}

static void jrk_settings_fix_core(jrk_settings * settings, jrk_string * warnings)
{
  // Beginning of auto-generated settings fixing code.
  // End of auto-generated settings fixing code.
}

jrk_error * jrk_settings_fix(jrk_settings * settings, char ** warnings)
{
  if (warnings) { *warnings = NULL; }

  if (settings == NULL)
  {
    return jrk_error_create("Jrk settings pointer is null.");
  }

  // Make a string to store the warnings we accumulate in this function.
  jrk_string str;
  if (warnings)
  {
    jrk_string_setup(&str);
  }
  else
  {
    jrk_string_setup_dummy(&str);
  }

  jrk_settings_fix_core(settings, &str);

  #ifndef NDEBUG
  {
    // In debug builds, assert that this function is idempotent.
    jrk_string str2;
    jrk_string_setup(&str2);
    jrk_settings_fix_core(settings, &str2);
    assert(str2.data != NULL && str2.data[0] == 0);
    jrk_string_free(str2.data);
  }
  #endif

  if (warnings && str.data == NULL)
  {
    // Memory allocation for the warning string failed at some point.
    return &jrk_error_no_memory;
  }

  if (warnings)
  {
    *warnings = str.data;
    str.data = NULL;
  }

  return NULL;
}
