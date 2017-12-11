// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file jrk.h
///
/// This file provides the C API for libpololu-jrk, a library for configuring
/// and controlling the jrk over USB.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "jrk_protocol.h"

#define JRK_PRODUCT_2017 1

// The maximum firmware major version supported by this library.
#define JRK_FIRMWARE_VERSION_MAJOR_MAX 1

#ifdef _WIN32
#  define JRK_DLL_EXPORT __declspec(dllexport)
#  define JRK_DLL_IMPORT __declspec(dllimport)
#else
#  define JRK_DLL_IMPORT __attribute__((visibility ("default")))
#  define JRK_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#ifdef _MSC_VER
#define JRK_WARN_UNUSED _Check_return_
#else
#define JRK_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#ifdef JRK_STATIC
#  define JRK_API
#else
#  ifdef JRK_EXPORTS
#    define JRK_API JRK_DLL_EXPORT
#  else
#    define JRK_API JRK_DLL_IMPORT
#  endif
#endif

/// Certain functions in the library return a newly-created string and require
/// the caller to call this function to free the string.  Passing a NULL pointer
/// to this function is OK.  Do not free the same non-NULL string twice.
void jrk_string_free(char *);


// Simple name lookup ///////////////////////////////////////////////////////////

/// Looks up a short code string without spaces representing the product.  The
/// product argument should be one of the JRK_PRODUCT_* macros, but if it is
/// not, this function returns an empty string.  The returned string will be
/// valid indefinitely and should not be freed.
JRK_API
const char * jrk_look_up_product_name_short(uint32_t product);

/// Looks up a user-friendly string for the product name.  The product argument
/// should be one of the JRK_PRODUCT_* macros, but if it is not, this function
/// returns "(Unknown)".
JRK_API
const char * jrk_look_up_product_name_ui(uint32_t product);

/// Looks up a user-friendly string corresponding to the specified error bit,
/// e.g. "Awaiting command".  The error argument should be of the form (1 <<
/// x) where x is one of the JRK_ERROR_* macros, but if it is not, this function
/// returns "(Unknown)".  The returned string will be valid indefinitely and
/// should not be freed.
JRK_API
const char * jrk_look_up_error_name_ui(uint32_t error);

/// Looks up a user-friendly string corresponding to the specified device reset,
/// e.g. "Stack underflow".  The device_reset argument should be one of the
/// JRK_RESET_* macros, but if it is not, this function returns "(Unknown)".
/// The returned string will be valid indefinitely and should not be freed.
JRK_API
const char * jrk_look_up_device_reset_name_ui(uint8_t device_reset);


// jrk_error ////////////////////////////////////////////////////////////////////

/// A ::jrk_error object represents an error that occurred in the library.  Many
/// functions return a jrk_error pointer.  The convention is that a NULL pointer
/// indicates success.  If the pointer is not NULL, the caller can use the
/// pointer to get information about the error, and then must free the error at
/// some point by calling jrk_error_free.
typedef struct jrk_error jrk_error;

/// Each ::jrk_error can have 0 or more error codes that give additional
/// information about the error that might help the caller take the right action
/// when the error occurs.  This enum defines which error codes are possible.
enum jrk_error_code
{
  /// There were problems allocating memory.  A memory shortage might be the
  /// root cause of the error, or there might be another error that is masked by
  /// the memory problems.
  JRK_ERROR_MEMORY = 1,

  /// Access was denied.  A common cause of this error on Windows is that
  /// another application has a handle open to the same device.
  JRK_ERROR_ACCESS_DENIED = 2,

  /// The device took too long to respond to a request or transfer data.
  JRK_ERROR_TIMEOUT = 3,

  /// The error might have been caused by the device being disconnected, but it
  /// is possible it was caused by something else.
  JRK_ERROR_DEVICE_DISCONNECTED = 4,
};

/// Attempts to copy an error.  If you copy a NULL ::jrk_error pointer, the
/// result will also be NULL.  If you copy a non-NULL ::jrk_error pointer, the
/// result will be non-NULL, but if there are issues allocating memory, then the
/// copied error might have different properties than the original error, and it
/// will have the ::JRK_ERROR_MEMORY code.
///
/// It is the caller's responsibility to free the copied error.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_error_copy(const jrk_error *);

/// Frees a returned error object.  Passing the NULL pointer to this function is
/// OK.  Do not free the same non-NULL error twice.
JRK_API
void jrk_error_free(jrk_error *);

/// Returns true if the error has specified error code.  The error codes are
/// listed in the ::jrk_error_code enum.
JRK_API JRK_WARN_UNUSED
bool jrk_error_has_code(const jrk_error *, uint32_t code);

/// Returns an English-language ASCII-encoded string describing the error.  The
/// string consists of one or more complete sentences.
///
/// The returned pointer will be valid at least until the error is freed.
JRK_API JRK_WARN_UNUSED
const char * jrk_error_get_message(const jrk_error *);


// jrk_settings ////////////////////////////////////////////////////////////////

/// Represents the settings for a jrk.  This object is just plain old data; it
/// does not have any pointers or handles for other resources.
typedef struct jrk_settings jrk_settings;

/// Creates a new settings object.
///
/// The new settings object will have no product specified and all settings set
/// to zero.  After creating the settings object, you would typically call
/// jrk_settings_set_product() and then jrk_settings_fill_with_defaults().
///
/// Then you would use setter and getter methods to work with the settings.  At
/// some point, you should call jrk_settings_fix() to make sure the settings are
/// valid and provide warnings about what settings were invalid.
///
/// The settings parameter should be a non-null pointer to a jrk_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling jrk_settings_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_create(jrk_settings ** settings);

/// Copies a jrk_settings object.  If this function is successful, the caller must
/// free the settings later by calling jrk_settings_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_copy(
  const jrk_settings * source ,
  jrk_settings ** dest);

/// Frees a jrk_settings object.  It is OK to pass a NULL pointer to this
/// function.  Do not free the same non-NULL settings object twice.
JRK_API
void jrk_settings_free(jrk_settings *);

/// Fixes the settings to have defaults.  Before calling this, you should
/// specify what product the settings are for by calling
/// jrk_settings_set_product().  If the product is not set to a valid non-zero
/// value, this function will do nothing.
void jrk_settings_fill_with_defaults(jrk_settings * settings);

/// Fixes the settings to be valid and consistent.
///
/// The warnings parameters is an optional pointer to pointer to a string.  If
/// you supply the warnings parameter, and this function is successful, this
/// function will allocate and return a string using the parameter.  The string
/// will describe what was fixed in the settings or be empty if nothing was
/// fixed.  Each distinct warning in the string will be a series of complete
/// English sentences that ends with a newline character.  The string must be
/// freed by the caller using jrk_string_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_fix(jrk_settings *, char ** warnings);

/// Gets the settings as a YAML string, also known as a settings file.  If this
/// function is successful, the string must be freed by the caller using
/// jrk_string_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_to_string(const jrk_settings *, char ** string);

/// Parses an YAML settings string, also known as a settings file, and returns
/// the corresponding settings object.  The settings returned might be invalid,
/// so it is recommend to call jrk_settings_fix() to fix the settings and warn
/// the user.
///
/// The settings parameter should be a non-null pointer to a jrk_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling jrk_settings_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_read_from_string(const char * string,
  jrk_settings ** settings);

/// Sets the product, which specifies what jrk product these settings are for.
/// The value should be one of the JRK_PRODUCT_* macros.
JRK_API
void jrk_settings_set_product(jrk_settings *, uint32_t product);

/// Gets the product, as described in jrk_settings_set_product().
JRK_API
uint32_t jrk_settings_get_product(const jrk_settings *);

// Beginning of auto-generated settings accessor prototypes.

// Sets the input_mode setting.
//
// The input mode setting specifies how you want to control the jrk.  It
// determines the definition of the input and target variables.  The input
// variable is raw measurement of the jrk's input.  The target variable is the
// desired state of the system's output, and feeds into the PID feedback
// algorithm.
//
// - If the input mode is JRK_INPUT_MODE_SERIAL, the jrk gets it input and
//   target settings over its USB, serial, or I2C interfaces.  You would send
//   Set Target commands to the jrk to set both the input and target variables.
//
// - If the input mode is JRK_INPUT_MODE_ANALOG, the jrk gets it input variable
//   by reading the voltage on its SDA/AN pin.  A signal level of 0 V
//   corresponds to an input value of 0, and a signal elvel of 5 V corresponds
//   to an input value of 4092.  The jrk uses its input scaling feature to set
//   the target variable.
//
// - If the input mode is JRK_INPUT_MODE_PULSE_WIDTH, the jrk gets it input
//   variable by reading RC pulses on its RC pin.  The input value is the width
//   of the most recent pulse, in units of 2/3 microseconds.  The jrk uses its
//   input scaling feature to set the target variable.
JRK_API
void jrk_settings_set_input_mode(jrk_settings *,
  uint8_t input_mode);

// Gets the input_mode setting, which is described in
// jrk_settings_set_input_mode.
JRK_API
uint8_t jrk_settings_get_input_mode(const jrk_settings *);

// Sets the input_disconnect_minimum setting.
JRK_API
void jrk_settings_set_input_disconnect_minimum(jrk_settings *,
  uint16_t input_disconnect_minimum);

// Gets the input_disconnect_minimum setting, which is described in
// jrk_settings_set_input_disconnect_minimum.
JRK_API
uint16_t jrk_settings_get_input_disconnect_minimum(const jrk_settings *);

// Sets the input_disconnect_maximum setting.
JRK_API
void jrk_settings_set_input_disconnect_maximum(jrk_settings *,
  uint16_t input_disconnect_maximum);

// Gets the input_disconnect_maximum setting, which is described in
// jrk_settings_set_input_disconnect_maximum.
JRK_API
uint16_t jrk_settings_get_input_disconnect_maximum(const jrk_settings *);

// Sets the input_minimum setting.
//
// This is one of the parameters of the input scaling feature, which is how the
// jrk calculates its target value from its raw input.
//
// By default, the input scaling:
//
// 1. Maps all values less than the input_minimum to the output_minimum.
// 2. Maps all values greater than the input_maximum to the output_maximum.
// 3. Maps all values between the input_neutral_min and input_neutral_max to
//    the output the output_neutral.
// 4. Behaves linearly between those regions.
//
// The input_invert parameter can flip that correspondence, and the
// input_scaling_degree parameter can change item 4 to use higher-order curves
// that give you finer control of the output near the neutral region.
JRK_API
void jrk_settings_set_input_minimum(jrk_settings *,
  uint16_t input_minimum);

// Gets the input_minimum setting, which is described in
// jrk_settings_set_input_minimum.
JRK_API
uint16_t jrk_settings_get_input_minimum(const jrk_settings *);

// Sets the input_maximum setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_input_maximum(jrk_settings *,
  uint16_t input_maximum);

// Gets the input_maximum setting, which is described in
// jrk_settings_set_input_maximum.
JRK_API
uint16_t jrk_settings_get_input_maximum(const jrk_settings *);

// Sets the input_neutral_minimum setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_input_neutral_minimum(jrk_settings *,
  uint16_t input_neutral_minimum);

// Gets the input_neutral_minimum setting, which is described in
// jrk_settings_set_input_neutral_minimum.
JRK_API
uint16_t jrk_settings_get_input_neutral_minimum(const jrk_settings *);

// Sets the input_neutral_maximum setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_input_neutral_maximum(jrk_settings *,
  uint16_t input_neutral_maximum);

// Gets the input_neutral_maximum setting, which is described in
// jrk_settings_set_input_neutral_maximum.
JRK_API
uint16_t jrk_settings_get_input_neutral_maximum(const jrk_settings *);

// Sets the output_minimum setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_output_minimum(jrk_settings *,
  uint16_t output_minimum);

// Gets the output_minimum setting, which is described in
// jrk_settings_set_output_minimum.
JRK_API
uint16_t jrk_settings_get_output_minimum(const jrk_settings *);

// Sets the output_neutral setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_output_neutral(jrk_settings *,
  uint16_t output_neutral);

// Gets the output_neutral setting, which is described in
// jrk_settings_set_output_neutral.
JRK_API
uint16_t jrk_settings_get_output_neutral(const jrk_settings *);

// Sets the output_maximum setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_output_maximum(jrk_settings *,
  uint16_t output_maximum);

// Gets the output_maximum setting, which is described in
// jrk_settings_set_output_maximum.
JRK_API
uint16_t jrk_settings_get_output_maximum(const jrk_settings *);

// Sets the input_invert setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_input_invert(jrk_settings *,
  bool input_invert);

// Gets the input_invert setting, which is described in
// jrk_settings_set_input_invert.
JRK_API
bool jrk_settings_get_input_invert(const jrk_settings *);

// Sets the input_scaling_degree setting.
//
// This is one of the parameters of the input scaling, as described in the
// input_minimum documentation.
JRK_API
void jrk_settings_set_input_scaling_degree(jrk_settings *,
  uint8_t input_scaling_degree);

// Gets the input_scaling_degree setting, which is described in
// jrk_settings_set_input_scaling_degree.
JRK_API
uint8_t jrk_settings_get_input_scaling_degree(const jrk_settings *);

// Sets the input_power_with_aux setting.
JRK_API
void jrk_settings_set_input_power_with_aux(jrk_settings *,
  bool input_power_with_aux);

// Gets the input_power_with_aux setting, which is described in
// jrk_settings_set_input_power_with_aux.
JRK_API
bool jrk_settings_get_input_power_with_aux(const jrk_settings *);

// Sets the input_analog_samples_exponent setting.
//
// This setting specifies how many analog samples to take if the input mode
// is analog.  The number of samples will be 4*(2^x), where x is this setting.
JRK_API
void jrk_settings_set_input_analog_samples_exponent(jrk_settings *,
  uint8_t input_analog_samples_exponent);

// Gets the input_analog_samples_exponent setting, which is described in
// jrk_settings_set_input_analog_samples_exponent.
JRK_API
uint8_t jrk_settings_get_input_analog_samples_exponent(const jrk_settings *);

// Sets the feedback_mode setting.
//
// The feedback mode setting specifies whether the jrk is using feedback from
// the output of the system, and if so defines what interface is used to
// measure that feedback.
//
// - If the feedback mode is JRK_FEEDBACK_MODE_NONE, feedback and PID
//   calculations are disabled.  The duty cycle target variable is always equal
//   to the target variable minus 2048, instead of being the result of a PID
//   calculation.  This means that a target of 2648 corresponds to driving the
//   motor full speed forward, 2048 is brake, and 1448 is full-speed reverse.
//
// - If the feedback mode is JRK_FEEDBACK_MODE_ANALOG, the jrk gets its
//   feedback by measuring the voltage on the FB_A pin.  A level of 0 V
//   corresponds to a feedback value of 0, and a level of 5 V corresponds to a
//   feedback value of 4092.  The feedback scaling algorithm computes the
//   scaled feedback variable, and the PID algorithm uses the scaled feedback
//   and the target to compute the duty cycle target.
//
// - If the feedback mode is JRK_FEEDBACK_MODE_FREQUENCY, the jrk gets it
//   feedback by counting rising edges on its FB_T pin.  When the target is
//   greater than 2048, the feedback value is 2048 plus the number of rising
//   edges detected during the PID period.  Otherwise, the the feedback is
//   2048 minus the the number of rising edges detected during the PID period.
JRK_API
void jrk_settings_set_feedback_mode(jrk_settings *,
  uint8_t feedback_mode);

// Gets the feedback_mode setting, which is described in
// jrk_settings_set_feedback_mode.
JRK_API
uint8_t jrk_settings_get_feedback_mode(const jrk_settings *);

// Sets the feedback_disconnect_minimum setting.
JRK_API
void jrk_settings_set_feedback_disconnect_minimum(jrk_settings *,
  uint16_t feedback_disconnect_minimum);

// Gets the feedback_disconnect_minimum setting, which is described in
// jrk_settings_set_feedback_disconnect_minimum.
JRK_API
uint16_t jrk_settings_get_feedback_disconnect_minimum(const jrk_settings *);

// Sets the feedback_disconnect_maximum setting.
JRK_API
void jrk_settings_set_feedback_disconnect_maximum(jrk_settings *,
  uint16_t feedback_disconnect_maximum);

// Gets the feedback_disconnect_maximum setting, which is described in
// jrk_settings_set_feedback_disconnect_maximum.
JRK_API
uint16_t jrk_settings_get_feedback_disconnect_maximum(const jrk_settings *);

// Sets the feedback_minimum setting.
//
// This is one of the parameters of the feedback scaling feature.
//
// By default, the feedback scaling:
//
//   1. Maps values less than or equal to feedback_minimum to 0.
//   2. Maps values less than or equal to feedback_maximum to 4095.
//   3. Behaves linearly between those two regions.
//
// The feedback_invert parameter causes the mapping to be flipped.
JRK_API
void jrk_settings_set_feedback_minimum(jrk_settings *,
  uint16_t feedback_minimum);

// Gets the feedback_minimum setting, which is described in
// jrk_settings_set_feedback_minimum.
JRK_API
uint16_t jrk_settings_get_feedback_minimum(const jrk_settings *);

// Sets the feedback_maximum setting.
//
// This is one of the parameters of the feedback scaling described in
// the feedback_minimum documentation.
JRK_API
void jrk_settings_set_feedback_maximum(jrk_settings *,
  uint16_t feedback_maximum);

// Gets the feedback_maximum setting, which is described in
// jrk_settings_set_feedback_maximum.
JRK_API
uint16_t jrk_settings_get_feedback_maximum(const jrk_settings *);

// Sets the feedback_invert setting.
//
// This is one of the parameters of the feedback scaling described in
// the feedback_minimum documentation.
JRK_API
void jrk_settings_set_feedback_invert(jrk_settings *,
  bool feedback_invert);

// Gets the feedback_invert setting, which is described in
// jrk_settings_set_feedback_invert.
JRK_API
bool jrk_settings_get_feedback_invert(const jrk_settings *);

// Sets the feedback_power_with_aux setting.
JRK_API
void jrk_settings_set_feedback_power_with_aux(jrk_settings *,
  bool feedback_power_with_aux);

// Gets the feedback_power_with_aux setting, which is described in
// jrk_settings_set_feedback_power_with_aux.
JRK_API
bool jrk_settings_get_feedback_power_with_aux(const jrk_settings *);

// Sets the feedback_dead_zone setting.
JRK_API
void jrk_settings_set_feedback_dead_zone(jrk_settings *,
  uint8_t feedback_dead_zone);

// Gets the feedback_dead_zone setting, which is described in
// jrk_settings_set_feedback_dead_zone.
JRK_API
uint8_t jrk_settings_get_feedback_dead_zone(const jrk_settings *);

// Sets the feedback_analog_samples_exponent setting.
//
// This setting specifies how many analog samples to take if the feedback mode
// is analog.  The number of samples will be 4*(2^x), where x is this setting.
JRK_API
void jrk_settings_set_feedback_analog_samples_exponent(jrk_settings *,
  uint8_t feedback_analog_samples_exponent);

// Gets the feedback_analog_samples_exponent setting, which is described in
// jrk_settings_set_feedback_analog_samples_exponent.
JRK_API
uint8_t jrk_settings_get_feedback_analog_samples_exponent(const jrk_settings *);

// Sets the feedback_wraparound setting.
//
// Normally, the error variable used by the PID algorithm is simply the scaled
// feedback minus the target.  With this setting enabled, the PID algorithm
// will add or subtract 4096 from that error value to get it into the -2048 to
// 2048 range.  This is useful for systems where the output of the system wraps
// around, so that 0 is next to 4095.  The jrk will know how to take the
// shortest path from one point to another even if it involves wrapping around
// from 0 to 4095 or vice versa.
JRK_API
void jrk_settings_set_feedback_wraparound(jrk_settings *,
  bool feedback_wraparound);

// Gets the feedback_wraparound setting, which is described in
// jrk_settings_set_feedback_wraparound.
JRK_API
bool jrk_settings_get_feedback_wraparound(const jrk_settings *);

// Sets the serial_mode setting.
JRK_API
void jrk_settings_set_serial_mode(jrk_settings *,
  uint8_t serial_mode);

// Gets the serial_mode setting, which is described in
// jrk_settings_set_serial_mode.
JRK_API
uint8_t jrk_settings_get_serial_mode(const jrk_settings *);

// Sets the serial_baud_rate setting.
//
// This setting specifies the baud rate to use on the RX and TX pins
// when the serial mode is UART.  It should be between
// JRK_MIN_ALLOWED_BAUD_RATE and JRK_MAX_ALLOWED_BAUD_RATE.
JRK_API
void jrk_settings_set_serial_baud_rate(jrk_settings *,
  uint32_t serial_baud_rate);

// Gets the serial_baud_rate setting, which is described in
// jrk_settings_set_serial_baud_rate.
JRK_API
uint32_t jrk_settings_get_serial_baud_rate(const jrk_settings *);

// Sets the serial_timeout setting.
//
// This is the time in units of 10 milliseconds before the device considers
// it to be an error if it has not received certain commands.  A value of 0
// disables the command timeout feature.
JRK_API
void jrk_settings_set_serial_timeout(jrk_settings *,
  uint16_t serial_timeout);

// Gets the serial_timeout setting, which is described in
// jrk_settings_set_serial_timeout.
JRK_API
uint16_t jrk_settings_get_serial_timeout(const jrk_settings *);

// Sets the serial_device_number setting.
JRK_API
void jrk_settings_set_serial_device_number(jrk_settings *,
  uint16_t serial_device_number);

// Gets the serial_device_number setting, which is described in
// jrk_settings_set_serial_device_number.
JRK_API
uint16_t jrk_settings_get_serial_device_number(const jrk_settings *);

// Sets the never_sleep setting.
JRK_API
void jrk_settings_set_never_sleep(jrk_settings *,
  bool never_sleep);

// Gets the never_sleep setting, which is described in
// jrk_settings_set_never_sleep.
JRK_API
bool jrk_settings_get_never_sleep(const jrk_settings *);

// Sets the serial_enable_crc setting.
JRK_API
void jrk_settings_set_serial_enable_crc(jrk_settings *,
  bool serial_enable_crc);

// Gets the serial_enable_crc setting, which is described in
// jrk_settings_set_serial_enable_crc.
JRK_API
bool jrk_settings_get_serial_enable_crc(const jrk_settings *);

// Sets the serial_enable_14bit_device_number setting.
JRK_API
void jrk_settings_set_serial_enable_14bit_device_number(jrk_settings *,
  bool serial_enable_14bit_device_number);

// Gets the serial_enable_14bit_device_number setting, which is described in
// jrk_settings_set_serial_enable_14bit_device_number.
JRK_API
bool jrk_settings_get_serial_enable_14bit_device_number(const jrk_settings *);

// Sets the serial_disable_compact_protocol setting.
JRK_API
void jrk_settings_set_serial_disable_compact_protocol(jrk_settings *,
  bool serial_disable_compact_protocol);

// Gets the serial_disable_compact_protocol setting, which is described in
// jrk_settings_set_serial_disable_compact_protocol.
JRK_API
bool jrk_settings_get_serial_disable_compact_protocol(const jrk_settings *);

// Sets the proportional_multiplier setting.
//
// The allowed range of this setting is 0 to 1023.
//
// In the PID algorithm, the error (the difference between scaled feedback
// and target) is multiplied by a number called the proportional coefficient to
// determine its effect on the motor duty cycle.
//
// The proportional coefficient is defined by this mathematical expression:
//   proportional_multiplier / 2^(proportional_exponent)
JRK_API
void jrk_settings_set_proportional_multiplier(jrk_settings *,
  uint16_t proportional_multiplier);

// Gets the proportional_multiplier setting, which is described in
// jrk_settings_set_proportional_multiplier.
JRK_API
uint16_t jrk_settings_get_proportional_multiplier(const jrk_settings *);

// Sets the proportional_exponent setting.
//
// The allowed range of this setting is 0 to 18.
// For more information, see the proportional_multiplier documentation.
JRK_API
void jrk_settings_set_proportional_exponent(jrk_settings *,
  uint8_t proportional_exponent);

// Gets the proportional_exponent setting, which is described in
// jrk_settings_set_proportional_exponent.
JRK_API
uint8_t jrk_settings_get_proportional_exponent(const jrk_settings *);

// Sets the integral_multiplier setting.
//
// The allowed range of this setting is 0 to 1023.
//
// In the PID algorithm, the accumulated error (known as error sum)
// is multiplied by a number called the integral coefficient to
// determine its effect on the motor duty cycle.
//
// The integral coefficient is defined by this mathematical expression:
//   integral_multiplier / 2^(integral_exponent)
//
// Note: On the original jrks (jrk 12v12 and jrk 21v3), the formula was
// different.  Those jrks added 3 to the integral_exponent before using
// it as a power of 2.
JRK_API
void jrk_settings_set_integral_multiplier(jrk_settings *,
  uint16_t integral_multiplier);

// Gets the integral_multiplier setting, which is described in
// jrk_settings_set_integral_multiplier.
JRK_API
uint16_t jrk_settings_get_integral_multiplier(const jrk_settings *);

// Sets the integral_exponent setting.
//
// The allowed range of this setting is 0 to 18.
// For more information, see the integral_multiplier documentation.
JRK_API
void jrk_settings_set_integral_exponent(jrk_settings *,
  uint8_t integral_exponent);

// Gets the integral_exponent setting, which is described in
// jrk_settings_set_integral_exponent.
JRK_API
uint8_t jrk_settings_get_integral_exponent(const jrk_settings *);

// Sets the derivative_multiplier setting.
//
// The allowed range of this setting is 0 to 1023.
//
// In the PID algorithm, the change in the error since the last PID period
// is multiplied by a number called the derivative coefficient to
// determine its effect on the motor duty cycle.
//
// The derivative coefficient is defined by this mathematical expression:
//   derivative_multiplier / 2^(derivative_exponent)
JRK_API
void jrk_settings_set_derivative_multiplier(jrk_settings *,
  uint16_t derivative_multiplier);

// Gets the derivative_multiplier setting, which is described in
// jrk_settings_set_derivative_multiplier.
JRK_API
uint16_t jrk_settings_get_derivative_multiplier(const jrk_settings *);

// Sets the derivative_exponent setting.
//
// The allowed range of this setting is 0 to 18.
// For more information, see the derivative_multiplier documentation.
JRK_API
void jrk_settings_set_derivative_exponent(jrk_settings *,
  uint8_t derivative_exponent);

// Gets the derivative_exponent setting, which is described in
// jrk_settings_set_derivative_exponent.
JRK_API
uint8_t jrk_settings_get_derivative_exponent(const jrk_settings *);

// Sets the pid_period setting.
//
// The PID period specifies how often the jrk should run its PID calculation
// and update the motor speed, in units of milliseconds.
JRK_API
void jrk_settings_set_pid_period(jrk_settings *,
  uint16_t pid_period);

// Gets the pid_period setting, which is described in
// jrk_settings_set_pid_period.
JRK_API
uint16_t jrk_settings_get_pid_period(const jrk_settings *);

// Sets the pid_integral_limit setting.
//
// The PID algorithm prevents the absolute value of the accumulated error
// (known as error sum) from exceeding pid_integral_limit.
JRK_API
void jrk_settings_set_pid_integral_limit(jrk_settings *,
  uint16_t pid_integral_limit);

// Gets the pid_integral_limit setting, which is described in
// jrk_settings_set_pid_integral_limit.
JRK_API
uint16_t jrk_settings_get_pid_integral_limit(const jrk_settings *);

// Sets the pid_reset_integral setting.
//
// If this setting is set to true, the PID algorithm will reset the accumulated
// error (also known as error sum) whenever the absolute value of the
// proportional term (see proportional_multiplier) exceeds 600.
JRK_API
void jrk_settings_set_pid_reset_integral(jrk_settings *,
  bool pid_reset_integral);

// Gets the pid_reset_integral setting, which is described in
// jrk_settings_set_pid_reset_integral.
JRK_API
bool jrk_settings_get_pid_reset_integral(const jrk_settings *);

// Sets the motor_pwm_frequency setting.
//
// This setting specifies whether to use 20 kHz (the default) or 5 kHz for the
// motor PWM signal.  This setting should be either
// JRK_MOTOR_PWM_FREQUENCY_20 or JRK_MOTOR_PWM_FREQUENCY_5.
JRK_API
void jrk_settings_set_motor_pwm_frequency(jrk_settings *,
  uint8_t motor_pwm_frequency);

// Gets the motor_pwm_frequency setting, which is described in
// jrk_settings_set_motor_pwm_frequency.
JRK_API
uint8_t jrk_settings_get_motor_pwm_frequency(const jrk_settings *);

// Sets the motor_invert setting.
//
// By default, a positive duty cycle (which we call "forward") corresponds
// to current flowing from output A to output B.  If enabled, this setting flips
// the correspondence, so a positive duty cycle corresponds to current flowing
// from B to A.
JRK_API
void jrk_settings_set_motor_invert(jrk_settings *,
  bool motor_invert);

// Gets the motor_invert setting, which is described in
// jrk_settings_set_motor_invert.
JRK_API
bool jrk_settings_get_motor_invert(const jrk_settings *);

// Sets the motor_max_duty_cycle_while_feedback_out_of_range setting.
JRK_API
void jrk_settings_set_motor_max_duty_cycle_while_feedback_out_of_range(jrk_settings *,
  uint16_t motor_max_duty_cycle_while_feedback_out_of_range);

// Gets the motor_max_duty_cycle_while_feedback_out_of_range setting, which is described in
// jrk_settings_set_motor_max_duty_cycle_while_feedback_out_of_range.
JRK_API
uint16_t jrk_settings_get_motor_max_duty_cycle_while_feedback_out_of_range(const jrk_settings *);

// Sets the motor_max_acceleration_forward setting.
JRK_API
void jrk_settings_set_motor_max_acceleration_forward(jrk_settings *,
  uint16_t motor_max_acceleration_forward);

// Gets the motor_max_acceleration_forward setting, which is described in
// jrk_settings_set_motor_max_acceleration_forward.
JRK_API
uint16_t jrk_settings_get_motor_max_acceleration_forward(const jrk_settings *);

// Sets the motor_max_acceleration_reverse setting.
JRK_API
void jrk_settings_set_motor_max_acceleration_reverse(jrk_settings *,
  uint16_t motor_max_acceleration_reverse);

// Gets the motor_max_acceleration_reverse setting, which is described in
// jrk_settings_set_motor_max_acceleration_reverse.
JRK_API
uint16_t jrk_settings_get_motor_max_acceleration_reverse(const jrk_settings *);

// Sets the motor_max_deceleration_forward setting.
JRK_API
void jrk_settings_set_motor_max_deceleration_forward(jrk_settings *,
  uint16_t motor_max_deceleration_forward);

// Gets the motor_max_deceleration_forward setting, which is described in
// jrk_settings_set_motor_max_deceleration_forward.
JRK_API
uint16_t jrk_settings_get_motor_max_deceleration_forward(const jrk_settings *);

// Sets the motor_max_deceleration_reverse setting.
JRK_API
void jrk_settings_set_motor_max_deceleration_reverse(jrk_settings *,
  uint16_t motor_max_deceleration_reverse);

// Gets the motor_max_deceleration_reverse setting, which is described in
// jrk_settings_set_motor_max_deceleration_reverse.
JRK_API
uint16_t jrk_settings_get_motor_max_deceleration_reverse(const jrk_settings *);

// Sets the motor_max_duty_cycle_forward setting.
JRK_API
void jrk_settings_set_motor_max_duty_cycle_forward(jrk_settings *,
  uint16_t motor_max_duty_cycle_forward);

// Gets the motor_max_duty_cycle_forward setting, which is described in
// jrk_settings_set_motor_max_duty_cycle_forward.
JRK_API
uint16_t jrk_settings_get_motor_max_duty_cycle_forward(const jrk_settings *);

// Sets the motor_max_duty_cycle_reverse setting.
JRK_API
void jrk_settings_set_motor_max_duty_cycle_reverse(jrk_settings *,
  uint16_t motor_max_duty_cycle_reverse);

// Gets the motor_max_duty_cycle_reverse setting, which is described in
// jrk_settings_set_motor_max_duty_cycle_reverse.
JRK_API
uint16_t jrk_settings_get_motor_max_duty_cycle_reverse(const jrk_settings *);

// Sets the motor_max_current_forward setting.
JRK_API
void jrk_settings_set_motor_max_current_forward(jrk_settings *,
  uint8_t motor_max_current_forward);

// Gets the motor_max_current_forward setting, which is described in
// jrk_settings_set_motor_max_current_forward.
JRK_API
uint8_t jrk_settings_get_motor_max_current_forward(const jrk_settings *);

// Sets the motor_max_current_reverse setting.
JRK_API
void jrk_settings_set_motor_max_current_reverse(jrk_settings *,
  uint8_t motor_max_current_reverse);

// Gets the motor_max_current_reverse setting, which is described in
// jrk_settings_set_motor_max_current_reverse.
JRK_API
uint8_t jrk_settings_get_motor_max_current_reverse(const jrk_settings *);

// Sets the motor_current_calibration_forward setting.
JRK_API
void jrk_settings_set_motor_current_calibration_forward(jrk_settings *,
  int8_t motor_current_calibration_forward);

// Gets the motor_current_calibration_forward setting, which is described in
// jrk_settings_set_motor_current_calibration_forward.
JRK_API
int8_t jrk_settings_get_motor_current_calibration_forward(const jrk_settings *);

// Sets the motor_current_calibration_reverse setting.
JRK_API
void jrk_settings_set_motor_current_calibration_reverse(jrk_settings *,
  int8_t motor_current_calibration_reverse);

// Gets the motor_current_calibration_reverse setting, which is described in
// jrk_settings_set_motor_current_calibration_reverse.
JRK_API
int8_t jrk_settings_get_motor_current_calibration_reverse(const jrk_settings *);

// Sets the motor_brake_duration_forward setting.
//
// Units of 5 ms.
JRK_API
void jrk_settings_set_motor_brake_duration_forward(jrk_settings *,
  uint8_t motor_brake_duration_forward);

// Gets the motor_brake_duration_forward setting, which is described in
// jrk_settings_set_motor_brake_duration_forward.
JRK_API
uint8_t jrk_settings_get_motor_brake_duration_forward(const jrk_settings *);

// Sets the motor_brake_duration_reverse setting.
//
// Units of 5 ms.
JRK_API
void jrk_settings_set_motor_brake_duration_reverse(jrk_settings *,
  uint8_t motor_brake_duration_reverse);

// Gets the motor_brake_duration_reverse setting, which is described in
// jrk_settings_set_motor_brake_duration_reverse.
JRK_API
uint8_t jrk_settings_get_motor_brake_duration_reverse(const jrk_settings *);

// Sets the motor_coast_when_off setting.
//
// By default, the jrk drives both motor outputs low when the motor is
// stopped (duty cycle is zero or there is an error), causing it to brake.
// If enabled, this setting causes it to instead tri-state both inputs, making
// the motor coast.
JRK_API
void jrk_settings_set_motor_coast_when_off(jrk_settings *,
  bool motor_coast_when_off);

// Gets the motor_coast_when_off setting, which is described in
// jrk_settings_set_motor_coast_when_off.
JRK_API
bool jrk_settings_get_motor_coast_when_off(const jrk_settings *);

// Sets the error_enable setting.
//
// This setting is a bitmap specifying which errors are enabled.
// The JRK_ERROR_* specifies the bits in the bitmap.  Certain errors are
// always enabled, so the jrk ignores the bits for those errors.
JRK_API
void jrk_settings_set_error_enable(jrk_settings *,
  uint16_t error_enable);

// Gets the error_enable setting, which is described in
// jrk_settings_set_error_enable.
JRK_API
uint16_t jrk_settings_get_error_enable(const jrk_settings *);

// Sets the error_latch setting.
//
// This setting is a bitmap specifying which errors are enabled and latched.
// The JRK_ERROR_* specifies the bits in the bitmap.  Certain errors are
// always latched if they are enabled, so the jrk ignores the bits for those
// errors.
JRK_API
void jrk_settings_set_error_latch(jrk_settings *,
  uint16_t error_latch);

// Gets the error_latch setting, which is described in
// jrk_settings_set_error_latch.
JRK_API
uint16_t jrk_settings_get_error_latch(const jrk_settings *);

// Sets the vin_calibration setting.
//
// A higher number gives you higher VIN readings, while a lower number gives
// you lower VIN readings.
JRK_API
void jrk_settings_set_vin_calibration(jrk_settings *,
  int16_t vin_calibration);

// Gets the vin_calibration setting, which is described in
// jrk_settings_set_vin_calibration.
JRK_API
int16_t jrk_settings_get_vin_calibration(const jrk_settings *);

// End of auto-generated settings accessor prototypes.

/// Returns an acheivable baud rate corresponding to the specified baud rate.
/// Does not modify the settings object.
JRK_API
uint32_t jrk_settings_achievable_serial_baud_rate(const jrk_settings *, uint32_t);


// jrk_variables ////////////////////////////////////////////////////////////////

/// Represents run-time variables that have been read from the jrk.
typedef struct jrk_variables jrk_variables;

/// Copies a jrk_variables object.  If this function is successful, the caller
/// must free the settings later by calling jrk_settings_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_variables_copy(
  const jrk_variables * source,
  jrk_variables ** dest);

/// Frees a jrk_variables object.  It is OK to pass a NULL pointer to this
/// function.  Do not free the same non-NULL settings object twice.
JRK_API
void jrk_variables_free(jrk_variables *);

// Beginning of auto-generated variables getter prototypes.

// Gets the input variable.
JRK_API
uint16_t jrk_variables_get_input(const jrk_variables *);

// Gets the target variable.
JRK_API
uint16_t jrk_variables_get_target(const jrk_variables *);

// Gets the feedback variable.
JRK_API
uint16_t jrk_variables_get_feedback(const jrk_variables *);

// Gets the scaled_feedback variable.
JRK_API
uint16_t jrk_variables_get_scaled_feedback(const jrk_variables *);

// Gets the error_sum variable.
JRK_API
uint16_t jrk_variables_get_error_sum(const jrk_variables *);

// Gets the duty_cycle_target variable.
JRK_API
int16_t jrk_variables_get_duty_cycle_target(const jrk_variables *);

// Gets the duty_cycle variable.
JRK_API
int16_t jrk_variables_get_duty_cycle(const jrk_variables *);

// Gets the current variable.
JRK_API
uint8_t jrk_variables_get_current(const jrk_variables *);

// Gets the pid_period_exceeded variable.
JRK_API
bool jrk_variables_get_pid_period_exceeded(const jrk_variables *);

// Gets the pid_period_count variable.
JRK_API
uint16_t jrk_variables_get_pid_period_count(const jrk_variables *);

// Gets the error_flags_halting variable.
JRK_API
uint16_t jrk_variables_get_error_flags_halting(const jrk_variables *);

// Gets the error_flags_occurred variable.
JRK_API
uint16_t jrk_variables_get_error_flags_occurred(const jrk_variables *);

// Gets the vin_voltage variable.
JRK_API
uint16_t jrk_variables_get_vin_voltage(const jrk_variables *);

// Gets the device_reset variable.
JRK_API
uint8_t jrk_variables_get_device_reset(const jrk_variables *);

// Gets the up_time variable.
JRK_API
uint32_t jrk_variables_get_up_time(const jrk_variables *);

// Gets the rc_pulse_width variable.
JRK_API
uint16_t jrk_variables_get_rc_pulse_width(const jrk_variables *);

// Gets the tachometer_reading variable.
JRK_API
uint16_t jrk_variables_get_tachometer_reading(const jrk_variables *);

// End of auto-generated variables getter prototypes.

// Gets the error, which is the scaled feedback minus the target.
JRK_API
int16_t jrk_variables_get_error(const jrk_variables *);


// jrk_device ///////////////////////////////////////////////////////////////////

/// Represents a jrk that is or was connected to the computer.
typedef struct jrk_device jrk_device;

/// Finds all the supported jrk devices connected to the computer via USB and
/// returns a list of them.
///
/// The list is terminated by a NULL pointer.  The optional @a device_count
/// parameter is used to return the number of devices in the list.
///
/// If this function is successful (the returned error pointer is NULL), then
/// you must later free each device by calling jrk_device_free() and free the
/// list by calling jrk_list_free().  The order in which the list and the
/// retrieved objects are freed does not matter.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_list_connected_devices(
  jrk_device *** device_list,
  size_t * device_count);

/// Frees a device list returned by ::jrk_list_connected_devices.  It is OK to
/// pass NULL to this function.
JRK_API
void jrk_list_free(jrk_device ** list);

/// Makes a copy of a device object.  If this function is successful, you will
/// need to free the copy by calling jrk_device_free() at some point.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_device_copy(
  const jrk_device * source,
  jrk_device ** dest);

/// Frees a device object.  Passing a NULL pointer to this function is OK.  Do
/// not free the same non-NULL pointer twice.
JRK_API
void jrk_device_free(jrk_device *);

/// Gets the product code of the device, which will be one of the JRK_PRODUCT_*
/// macros.  If the device is NULL, returns 0 instead.
JRK_API JRK_WARN_UNUSED
uint8_t jrk_device_get_product(const jrk_device *);

/// Gets the serial number of the device as an ASCII-encoded string.
/// The string will be valid at least as long as the device object.
/// If the device is NULL, returns an empty string.
JRK_API JRK_WARN_UNUSED
const char * jrk_device_get_serial_number(const jrk_device *);

/// Get an operating system-specific identifier for this device as an
/// ASCII-encoded string.  The string will be valid at least as long as the
/// device object.
/// If the device is NULL, returns an empty string.
JRK_API JRK_WARN_UNUSED
const char * jrk_device_get_os_id(const jrk_device *);

/// Gets the firmware version as a binary-coded decimal number (e.g. 0x0100
/// means "1.00").  We recommend using jrk_get_firmware_version_string() instead
/// of this function if possible.
JRK_API JRK_WARN_UNUSED
uint16_t jrk_device_get_firmware_version(const jrk_device *);


// jrk_handle ///////////////////////////////////////////////////////////////////

/// Represents an open handle that can be used to read and write data from a
/// device.
typedef struct jrk_handle jrk_handle;

/// Opens a handle to the specified device.  The handle must later be closed
/// with jrk_handle_close().
///
/// On Windows, if another applications has a handle open already, then this
/// function will fail and the returned error will have code
/// ::JRK_ERROR_ACCESS_DENIED.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_handle_open(const jrk_device *, jrk_handle **);

/// Closes and frees the specified handle.  It is OK to pass NULL to this
/// function.  Do not close the same non-NULL handle twice.
JRK_API
void jrk_handle_close(jrk_handle *);

/// Gets the device object that corresponds to this handle.
/// The device object will be valid for at least as long as the handle.
JRK_API JRK_WARN_UNUSED
const jrk_device * jrk_handle_get_device(const jrk_handle *);

/// Gets the firmware version string, including any special modification codes
/// (e.g. "1.07nc").  The string will be valid for at least as long as the device.
///
/// This function might need to do I/O, but it will cache the result so it is OK
/// to call it frequently.
JRK_API JRK_WARN_UNUSED
const char * jrk_get_firmware_version_string(jrk_handle *);

/// Reads the jrk's status variables and returns them as an object.
///
/// This function sends a "Get variables" command.
///
/// The variables parameter should be a non-null pointer to a jrk_variables
/// pointer, which will receive a pointer to a new variables object if and only
/// if this function is successful.  The caller must free the variables later by
/// calling jrk_variables_free().
///
/// To read information from the variables object, see the jrk_variables_get_*
/// functions.
///
/// The clear_errors_halting option should be true if you want to clear the
/// error bits in the device's "Error flags halting" variable (except for the
/// Awaiting Command error bit).
///
/// The clear_errors_occurred option should be true if you want to clear the
/// bits in the device's "Error flags occurred" variable as a side effect.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_variables(jrk_handle *, jrk_variables ** variables,
  bool clear_error_flags_halting, bool clear_error_flags_occurred);

/// Reads all of the jrk's non-volatile settings and returns them as an object.
///
/// The settings parameter should be a non-null pointer to a jrk_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling jrk_settings_free().
///
/// To access fields in the variables, see the jrk_settings_* functions.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_settings(jrk_handle *, jrk_settings ** settings);

/// Writes all of the jrk's non-volatile settings.
///
/// Internally, this function copies the settings and calls jrk_settings_fix()
/// to make sure that the settings will be valid when they are written to the
/// device.  If you want to get warnings about what was changed, you should call
/// jrk_settings_fix() yourself beforehand.
///
/// After calling this function, to make the settings actually take effect, you
/// should call jrk_reinitialize().
///
/// Be careful not to call this command in a fast loop to avoid wearing out the
/// jrk's EEPROM, which is rated for only 100,000 write cycles.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_set_settings(jrk_handle *, const jrk_settings *);

/// Resets the jrk's settings to their factory default values.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_restore_defaults(jrk_handle * handle);

/// Causes the jrk to reload all of its settings from EEPROM and make them take
/// effect.  See also jrk_reset().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_reinitialize(jrk_handle * handle);

/// \cond
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_debug_data(jrk_handle *, uint8_t * data, size_t * size);
/// \endcond


#ifdef __cplusplus
}
#endif
