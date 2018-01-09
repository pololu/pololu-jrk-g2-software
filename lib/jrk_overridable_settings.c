#include "jrk_internal.h"

struct jrk_overridable_settings
{
  // Beginning of auto-generated overridable settings struct members.
  // End of auto-generated settings struct members.
};

jrk_error * jrk_overridable_settings_create(jrk_overridable_settings ** settings)
{
  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  *settings = NULL;

  jrk_error * error = NULL;

  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_overridable_settings *)
      calloc(1, sizeof(jrk_overridable_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  return error;
}

void jrk_overridable_settings_free(jrk_overridable_settings * s)
{
  free(s);
}

jrk_error * jrk_overridable_settings_copy(
  const jrk_overridable_settings * source,
  jrk_overridable_settings ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Overridable settings output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_overridable_settings * new_settings = NULL;
  if (error == NULL)
  {
    new_settings = (jrk_overridable_settings *)
      calloc(1, sizeof(jrk_overridable_settings));
    if (new_settings == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_settings, source, sizeof(jrk_overridable_settings));
  }

  if (error == NULL)
  {
    *dest = new_settings;
    new_settings = NULL;
  }

  jrk_overridable_settings_free(new_settings);

  return error;
}

// Beginning of auto-generated overridable settings accessors.
// End of auto-generated overridable settings accessors.
