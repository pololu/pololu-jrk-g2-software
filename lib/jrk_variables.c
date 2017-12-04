#include "jrk_internal.h"

#define PIN_COUNT 5

struct jrk_variables
{
  uint8_t avoid_struct_size_zero;  // TODO: remove, put real variables here
};

jrk_error * jrk_variables_create(jrk_variables ** variables)
{
  if (variables == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  jrk_error * error = NULL;

  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (jrk_variables *)calloc(1, sizeof(jrk_variables));
    if (new_variables == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  return error;
}

jrk_error * jrk_variables_copy(const jrk_variables * source, jrk_variables ** dest)
{
  if (dest == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *dest = NULL;

  if (source == NULL)
  {
    return NULL;
  }

  jrk_error * error = NULL;

  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    new_variables = (jrk_variables *)calloc(1, sizeof(jrk_variables));
    if (new_variables == NULL) { error = &jrk_error_no_memory; }
  }

  if (error == NULL)
  {
    memcpy(new_variables, source, sizeof(jrk_variables));
  }

  if (error == NULL)
  {
    *dest = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  return error;
}

void jrk_variables_free(jrk_variables * variables)
{
  free(variables);
}

static void write_buffer_to_variables(const uint8_t * buf, jrk_variables * vars)
{
  assert(vars != NULL);
  assert(buf != NULL);

  // TODO: interpret the variables buffer
}

jrk_error * jrk_get_variables(jrk_handle * handle, jrk_variables ** variables,
  bool clear_error_flags_halting, bool clear_error_flags_occurred)
{
  if (variables == NULL)
  {
    return jrk_error_create("Variables output pointer is null.");
  }

  *variables = NULL;

  if (handle == NULL)
  {
    return jrk_error_create("Handle is null.");
  }

  jrk_error * error = NULL;

  // Create a variables object.
  jrk_variables * new_variables = NULL;
  if (error == NULL)
  {
    error = jrk_variables_create(&new_variables);
  }

  // Read all the variables from the device.
  uint8_t buf[JRK_VARIABLES_SIZE];
  if (error == NULL)
  {
    size_t index = 0;
    error = jrk_get_variable_segment(handle, index, sizeof(buf), buf,
      clear_error_flags_halting, clear_error_flags_occurred);
  }

  // Store the variables in the new variables object.
  if (error == NULL)
  {
    write_buffer_to_variables(buf, new_variables);
  }

  // Pass the new variables to the caller.
  if (error == NULL)
  {
    *variables = new_variables;
    new_variables = NULL;
  }

  jrk_variables_free(new_variables);

  if (error != NULL)
  {
    error = jrk_error_add(error,
      "There was an error reading variables from the device.");
  }

  return error;
}
