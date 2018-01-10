#pragma once

#include <jrk.h>
#include <config.h>

#include <libusbp.h>
#include <yaml.h>

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef _MSC_VER
#define JRK_PRINTF(f, a)
#else
#define JRK_PRINTF(f, a) __attribute__((format (printf, f, a)))
#endif

// A setup packet bRequest value from USB 2.0 Table 9-4
#define USB_REQUEST_GET_DESCRIPTOR 6

// A descriptor type from USB 2.0 Table 9-5
#define USB_DESCRIPTOR_TYPE_STRING 3

// TODO: remove this and remove custom_eeprom on the max current settings
#define TMPHAX_CURRENT_UNITS 4

// Internal string manipulation library.

typedef struct jrk_string
{
  char * data;
  size_t capacity;
  size_t length;
} jrk_string;

void jrk_string_setup(jrk_string *);
void jrk_string_setup_dummy(jrk_string *);
JRK_PRINTF(2, 3)
void jrk_sprintf(jrk_string *, const char * format, ...);

#define STRING_TO_INT_ERR_SMALL 1
#define STRING_TO_INT_ERR_LARGE 2
#define STRING_TO_INT_ERR_EMPTY 3
#define STRING_TO_INT_ERR_INVALID 4

uint8_t jrk_string_to_i64(const char *, int64_t *);


// Internal name lookup library.

typedef struct jrk_name
{
  const char * name;
  uint32_t code;
} jrk_name;

bool jrk_name_to_code(const jrk_name * table, const char * name, uint32_t * code);
bool jrk_code_to_name(const jrk_name * table, uint32_t code, const char ** name);

extern const jrk_name jrk_bool_names[];
extern const jrk_name jrk_product_names_short[];
extern const jrk_name jrk_input_mode_names_short[];
extern const jrk_name jrk_input_scaling_degree_names_short[];
extern const jrk_name jrk_feedback_mode_names_short[];
extern const jrk_name jrk_serial_mode_names_short[];
extern const jrk_name jrk_motor_pwm_frequency_names_short[];
extern const jrk_name jrk_pin_func_names[];


// Intenral variables functions.

void jrk_variables_set_from_device(jrk_variables *, const uint8_t * buffer);


// Internal settings conversion functions.

uint32_t jrk_baud_rate_from_brg(uint16_t brg);
uint16_t jrk_baud_rate_to_brg(uint32_t baud_rate);

// Internal jrk_overridable_settings functions.

jrk_error * jrk_overridable_settings_create(jrk_overridable_settings **);

// Internal jrk_device functions.

const libusbp_generic_interface *
jrk_device_get_generic_interface(const jrk_device * device);


// Internal jrk_handle functions.

jrk_error * jrk_set_setting_byte(jrk_handle * handle,
  uint8_t address, uint8_t byte);

jrk_error * jrk_get_setting_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * output);

jrk_error * jrk_set_overridable_setting_segment(jrk_handle * handle,
  size_t index, size_t length, const uint8_t * buf);

jrk_error * jrk_get_overridable_setting_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * output);

jrk_error * jrk_get_variable_segment(jrk_handle * handle,
  size_t index, size_t length, uint8_t * buf, uint16_t flags);


// Error creation functions.

jrk_error * jrk_error_add_code(jrk_error * error, uint32_t code);

JRK_PRINTF(2, 3)
jrk_error * jrk_error_add(jrk_error * error, const char * format, ...);

JRK_PRINTF(1, 2)
jrk_error * jrk_error_create(const char * format, ...);

jrk_error * jrk_usb_error(libusbp_error *);

extern jrk_error jrk_error_no_memory;


// Static helper functions

static inline uint16_t read_uint16_t(const uint8_t * p)
{
  return p[0] + (p[1] << 8);
}

static inline int16_t read_int16_t(const uint8_t * p)
{
  return read_uint16_t(p);
}

static inline uint32_t read_uint32_t(const uint8_t * p)
{
  return p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
}

static inline void write_uint16_t(uint8_t * p, uint16_t value)
{
  p[0] = value & 0xFF;
  p[1] = value >> 8 & 0xFF;
}

static inline void write_int16_t(uint8_t * p, uint16_t value)
{
  write_uint16_t(p, value);
}
