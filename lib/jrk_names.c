// Functions for converting between names and numeric codes.

#include "jrk_internal.h"

const jrk_name jrk_bool_names[] =
{
  { "true", 1 },
  { "false", 0 },
  { NULL, 0 },
};

const jrk_name jrk_product_names_short[] =
{
  { "2017", JRK_PRODUCT_2017 }, // TODO: real name
  { NULL, 0 },
};

const jrk_name jrk_product_names_ui[] =
{
  { "Jrk 2017 USB Motor Controller with Feedback", JRK_PRODUCT_2017 },  // TODO: real name
  { NULL, 0 },
};

const jrk_name jrk_error_names_ui[] =  // TODO
{
  { NULL, 0 },
};

const jrk_name jrk_device_reset_names_ui[] =
{
  { "Power-on reset", JRK_RESET_POWER_UP },
  { "Brown-out reset", JRK_RESET_BROWNOUT },
  { "Reset pin driven low", JRK_RESET_RESET_LINE },
  { "Watchdog reset", JRK_RESET_WATCHDOG },
  { "Software reset (bootloader)", JRK_RESET_SOFTWARE },
  { "Stack overflow", JRK_RESET_STACK_OVERFLOW },
  { "Stack underflow", JRK_RESET_STACK_UNDERFLOW },
  { NULL, 0 },
};

const char * jrk_look_up_product_name_short(uint32_t product)
{
  const char * str = "";
  jrk_code_to_name(jrk_product_names_short, product, &str);
  return str;
}

const char * jrk_look_up_product_name_ui(uint32_t product)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_product_names_ui, product, &str);
  return str;
}

const char * jrk_look_up_error_name_ui(uint32_t error)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_error_names_ui, error, &str);
  return str;
}

const char * jrk_look_up_device_reset_name_ui(uint8_t device_reset)
{
  const char * str = "(Unknown)";
  jrk_code_to_name(jrk_device_reset_names_ui, device_reset, &str);
  return str;
}

bool jrk_name_to_code(const jrk_name * table, const char * name, uint32_t * code)
{
  if (code) { *code = 0; }

  if (!table) { return false; }
  if (!name) { return false; }

  for (const jrk_name * p = table; p->name; p++)
  {
    if (!strcmp(p->name, name))
    {
      if (code) { *code = p->code; }
      return true;
    }
  }

  return false;
}

bool jrk_code_to_name(const jrk_name * table, uint32_t code, const char ** name)
{
  if (!table) { return false; }

  for (const jrk_name * p = table; p->name; p++)
  {
    if (p->code == code)
    {
      if (name) { *name = p->name; }
      return true;
    }
  }

  return false;
}
