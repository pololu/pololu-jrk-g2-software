#include "jrk_internal.h"

// We apply the product name from the settings file first, and use it to set the
// defaults.
static jrk_error * apply_product_name(jrk_settings * settings, const char * product_name)
{
  uint32_t product;
  if (!jrk_name_to_code(jrk_product_names_short, product_name, &product))
  {
    return jrk_error_create("Unrecognized product name.");
  }
  jrk_settings_set_product(settings, product);
  jrk_settings_fill_with_defaults(settings);
  return NULL;
}

// Note: The range checking we do in this function is solely to make sure the
// value will fit in the argument to the setter function we call.  If the value
// is otherwise outside the allowed range, that will be checked in
// jrk_settings_fix.
static jrk_error * apply_string_pair(jrk_settings * settings,
  const char * key, const char * value, uint32_t line)
{
  if (!strcmp(key, "product"))
  {
    // We already processed the product field separately.
  }

  // Beginning of auto-generated settings file parsing code.

  else if (!strcmp(key, "input_mode"))
  {
    uint32_t input_mode;
    if (!jrk_name_to_code(jrk_input_mode_names_short, value, &input_mode))
    {
      return jrk_error_create("Unrecognized input_mode value.");
    }
    jrk_settings_set_input_mode(settings, input_mode);
  }
  else if (!strcmp(key, "input_minimum"))
  {
    int64_t input_minimum;
    if (jrk_string_to_i64(value, &input_minimum))
    {
      return jrk_error_create("Invalid input_minimum value.");
    }
    if (input_minimum < 0 || input_minimum > UINT16_MAX)
    {
      return jrk_error_create(
        "The input_minimum value is out of range.");
    }
    jrk_settings_set_input_minimum(settings, input_minimum);
  }

  // End of auto-generated settings file parsing code.

  else
  {
    return jrk_error_create("Unrecognized key on line %d: \"%s\".", line, key);
  }

  return NULL;
}

// Returns true if the node is a scalar and its value equals the given
// null-terminated string.
static bool scalar_eq(const yaml_node_t * node, const char * v)
{
  if (node == NULL) { return NULL; }
  if (node->type != YAML_SCALAR_NODE) { return false; }
  if (node->data.scalar.length != strlen(v)) { return false; }
  return !memcmp(v, node->data.scalar.value, node->data.scalar.length);
}

// Given a mapping and a key name (as a null-terminated string), gets the node
// corresponding to the value, or NULL if it could not be found.
static yaml_node_t * map_lookup(yaml_document_t * doc,
  yaml_node_t * map, const char * key_name)
{
  if (doc == NULL) { assert(0); return NULL; }
  if (map == NULL) { assert(0); return NULL; }
  if (map->type != YAML_MAPPING_NODE) { return NULL; }

  for (yaml_node_pair_t * pair = map->data.mapping.pairs.start;
       pair < map->data.mapping.pairs.top; pair++)
  {
    yaml_node_t * key = yaml_document_get_node(doc, pair->key);
    if (scalar_eq(key, key_name))
    {
      return yaml_document_get_node(doc, pair->value);
    }
  }
  return NULL;
}

#define MAX_SCALAR_LENGTH 255

// Takes a key-value pair from the YAML file, does some basic checks, creates
// proper null-terminated C strings, and then calls apply_string_pair to do the
// actual logic of parsing strings and applying the settings.
static jrk_error * apply_yaml_pair(jrk_settings * settings,
  const yaml_node_t * key, const yaml_node_t * value)
{
  if (key == NULL)
  {
    return jrk_error_create("Internal YAML processing error: Invalid key index.");
  }
  if (value == NULL)
  {
    return jrk_error_create("Internal YAML processing error: Invalid value index.");
  }

  uint32_t line = key->start_mark.line + 1;

  // Make sure the key is valid and convert it to a C string (we aren't sure
  // that libyaml always provides a null termination byte because scalars can
  // have have null bytes in them).
  if (key->type != YAML_SCALAR_NODE)
  {
    return jrk_error_create(
      "YAML key is not a scalar on line %d.", line);
  }
  if (key->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return jrk_error_create(
      "YAML key is too long on line %d.", line);
  }
  char key_str[MAX_SCALAR_LENGTH + 1];
  memcpy(key_str, key->data.scalar.value, key->data.scalar.length);
  key_str[key->data.scalar.length] = 0;

  // Make sure the value is valid and convert it to a C string.
  if (value->type != YAML_SCALAR_NODE)
  {
    return jrk_error_create(
      "YAML value is not a scalar on line %d.", line);
  }
  if (value->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return jrk_error_create(
      "YAML value is too long on line %d.", line);
  }
  char value_str[MAX_SCALAR_LENGTH + 1];
  memcpy(value_str, value->data.scalar.value, value->data.scalar.length);
  value_str[value->data.scalar.length] = 0;

  return apply_string_pair(settings, key_str, value_str, line);
}

// Validates the YAML doc and populates the settings object with the settings
// from the document.
static jrk_error * read_from_yaml_doc(
  yaml_document_t * doc, jrk_settings * settings)
{
  assert(doc != NULL);
  assert(settings != NULL);

  // Get the root node and make sure it is a mapping.
  yaml_node_t * root = yaml_document_get_root_node(doc);
  if (root->type != YAML_MAPPING_NODE)
  {
    return jrk_error_create("YAML root node is not a mapping.");
  }

  // Process the "product" key/value pair first.
  yaml_node_t * product_value = map_lookup(doc, root, "product");
  if (product_value == NULL)
  {
    return jrk_error_create("No product was specified in the settings file.");
  }
  uint32_t product_line = product_value->start_mark.line + 1;
  if (product_value->type != YAML_SCALAR_NODE)
  {
    return jrk_error_create(
      "YAML product value is not a scalar on line %d.", product_line);
  }
  if (product_value->data.scalar.length > MAX_SCALAR_LENGTH)
  {
    return jrk_error_create(
      "YAML product value is too long on line %d.", product_line);
  }
  char product_str[MAX_SCALAR_LENGTH + 1];
  memcpy(product_str, product_value->data.scalar.value,
    product_value->data.scalar.length);
  product_str[product_value->data.scalar.length] = 0;
  apply_product_name(settings, product_str);

  // Iterate over the pairs in the YAML mapping and process each one.
  for (yaml_node_pair_t * pair = root->data.mapping.pairs.start;
       pair < root->data.mapping.pairs.top; pair++)
  {
    yaml_node_t * key = yaml_document_get_node(doc, pair->key);
    yaml_node_t * value = yaml_document_get_node(doc, pair->value);
    jrk_error * error = apply_yaml_pair(settings, key, value);
    if (error) { return error; }
  }

  return NULL;
}

jrk_error * jrk_settings_read_from_string(const char * string,
  jrk_settings ** settings)
{
  if (string == NULL)
  {
    return jrk_error_create("Settings input string is null.");
  }

  if (settings == NULL)
  {
    return jrk_error_create("Settings output pointer is null.");
  }

  jrk_error * error = NULL;

  // Allocate a new settings object.
  jrk_settings * new_settings = NULL;
  if (error == NULL)
  {
    error = jrk_settings_create(&new_settings);
  }

  // Make a YAML parser.
  bool parser_initialized = false;
  yaml_parser_t parser;
  if (error == NULL)
  {
    int success = yaml_parser_initialize(&parser);
    if (success)
    {
      parser_initialized = true;
    }
    else
    {
      error = jrk_error_create("Failed to initialize YAML parser.");
    }
  }

  // Construct a YAML document using the parser.
  bool document_initialized = false;
  yaml_document_t doc;
  if (error == NULL)
  {
    yaml_parser_set_input_string(&parser, (const uint8_t *)string, strlen(string));
    int success = yaml_parser_load(&parser, &doc);
    if (success)
    {
      document_initialized = true;
    }
    else
    {
      error = jrk_error_create("Failed to load document: %s at line %u.",
        parser.problem, (unsigned int)parser.problem_mark.line + 1);
    }
  }

  // Proces the YAML document.
  if (error == NULL)
  {
    error = read_from_yaml_doc(&doc, new_settings);
  }

  // Success!  Pass the settings to the caller.
  if (error == NULL)
  {
    *settings = new_settings;
    new_settings = NULL;
  }

  if (document_initialized)
  {
    yaml_document_delete(&doc);
  }

  if (parser_initialized)
  {
    yaml_parser_delete(&parser);
  }

  jrk_settings_free(new_settings);

  if (error != NULL)
  {
    error = jrk_error_add(error, "There was an error reading the settings file.");
  }

  return error;
}
