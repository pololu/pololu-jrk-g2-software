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

#define JRK_PRODUCT_UMC04A_30V 1
#define JRK_PRODUCT_UMC04A_40V 2
#define JRK_PRODUCT_UMC05A_30V 3
#define JRK_PRODUCT_UMC05A_40V 4
#define JRK_PRODUCT_UMC06A     5

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
JRK_API
void jrk_string_free(char *);


// Simple name lookup ///////////////////////////////////////////////////////////

/// Looks up a short name string without spaces representing the product.  The
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

/// Looks up a user-friendly string corresponding to the specified force mode
/// from jrk_variables_get_force_mode() (e.g. "Duty cycle"). Returns "(Unknown)"
/// if the argument is not valid.  The returned string will be valid
/// indefinitely and should not be freed.
JRK_API
const char * jrk_look_up_force_mode_name_ui(uint8_t force_mode);

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
/// some point by calling jrk_error_free().
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
  const jrk_settings * source,
  jrk_settings ** dest);

/// Frees a jrk_settings object.  It is OK to pass a NULL pointer to this
/// function.  Do not free the same non-NULL settings object twice.
JRK_API
void jrk_settings_free(jrk_settings *);

/// Fixes the settings to have defaults.  Before calling this, you should
/// specify what product the settings are for by calling
/// jrk_settings_set_product().  If the product is not set to a valid non-zero
/// value, this function will do nothing.
///
/// This function does not change the firmware version field.
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

/// Changes the product and firmware version, adapts any product-specific
/// settings (like encoded hard current limits) as appropriate, and then
/// runs jrk_settings_fix.
///
/// This function is appropriate to use when you want to apply settings to a
/// device but the settings object might have come from a different type of
/// Jrk.
///
/// The product argument should be one of the JRK_PRODUCT_* macros.  You
/// can get the product value of a device using jrk_device_get_product().
///
/// The firmware version should be zero or a firmare version returned by
/// jrk_device_get_firmware_version().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_settings_fix_and_change_product(jrk_settings *,
  uint32_t product, uint16_t firmware_version, char ** warnings);

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

/// Sets the product, which specifies what Jrk product these settings are for.
/// The value should be one of the JRK_PRODUCT_* macros.
///
/// Note that this is just a simple setter function that changes the product
/// ID stored in the settings and nothing else.  If you have a settings object
/// originally made for one product and you want to convert it to be for another
/// product, while adapting/fixing any settings as appropriate, see
/// jrk_settings_fix_and_change_product.
JRK_API
void jrk_settings_set_product(jrk_settings *, uint32_t product);

/// Gets the product, as described in jrk_settings_set_product().
JRK_API
uint32_t jrk_settings_get_product(const jrk_settings *);

/// Sets the firmware version, which specifies what Jrk firmware version these
/// settings are for.
///
/// The firmware version can be 0 to indicate that it is unknown.  Otherwise,
/// it should be a firmware version from jrk_device_get_firmware_version().
JRK_API
void jrk_settings_set_firmware_version(jrk_settings *,
  uint16_t firmware_version);

/// Gets the firmware version, as described in
/// jrk_settings_set_firmware_version().
JRK_API
uint16_t jrk_settings_get_firmware_version(const jrk_settings *);

// Beginning of auto-generated settings accessor prototypes.

// Sets the input_mode setting.
//
// The input mode setting specifies how you want to control the Jrk.  It
// determines the definition of the input and target variables.  The input
// variable is a raw measurement of the Jrk's input.  The target variable is
// the desired state of the system's output, and feeds into the PID feedback
// algorithm.
//
// - If the input mode is "Serial" (JRK_INPUT_MODE_SERIAL), the Jrk gets its
//   input and target settings over its USB, serial, or I2C interfaces.  You
//   can send Set Target commands to the Jrk to set both the input and target
//   variables.
//
// - If the input mode is "Analog voltage" (JRK_INPUT_MODE_ANALOG), the Jrk gets
//   it input variable by reading the voltage on its SDA/AN pin.  A signal level
//   of 0 V corresponds to an input value of 0, and a signal level of 5 V
//   corresponds to an input value of 4092.  The Jrk uses its input scaling
//   feature to set the target variable.
//
// - If the input mode is "RC" (JRK_INPUT_MODE_RC), the Jrk
//   gets it input variable by reading RC pulses on its RC pin.  The input value
//   is the width of the most recent pulse, in units of 2/3 microseconds.  The
//   Jrk uses its input scaling feature to set the target variable.
JRK_API
void jrk_settings_set_input_mode(jrk_settings *,
  uint8_t input_mode);

// Gets the input_mode setting, which is described in
// jrk_settings_set_input_mode.
JRK_API
uint8_t jrk_settings_get_input_mode(const jrk_settings *);

// Sets the input_error_minimum setting.
//
// If the raw input value is below this value, it causes an input disconnect
// error.
JRK_API
void jrk_settings_set_input_error_minimum(jrk_settings *,
  uint16_t input_error_minimum);

// Gets the input_error_minimum setting, which is described in
// jrk_settings_set_input_error_minimum.
JRK_API
uint16_t jrk_settings_get_input_error_minimum(const jrk_settings *);

// Sets the input_error_maximum setting.
//
// If the raw input value is above this value, it causes an "Input disconnect"
// error.
JRK_API
void jrk_settings_set_input_error_maximum(jrk_settings *,
  uint16_t input_error_maximum);

// Gets the input_error_maximum setting, which is described in
// jrk_settings_set_input_error_maximum.
JRK_API
uint16_t jrk_settings_get_input_error_maximum(const jrk_settings *);

// Sets the input_minimum setting.
//
// This is one of the input scaling parameters that determines how the Jrk
// calculates its target value from its raw input.
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

// Sets the input_detect_disconnect setting.
//
// If the input mode is JRK_INPUT_MODE_ANALOG, this setting causes the Jrk to
// drive its input potentiometer power pin (SCL) low once per PID period after
// measuring SDA/AN.  If the voltage on SDA/AN does not drop by at least a
// factor of two while SCL is low, then the Jrk will report an
// "Input disconnect" error (if that error is enabled).
JRK_API
void jrk_settings_set_input_detect_disconnect(jrk_settings *,
  bool input_detect_disconnect);

// Gets the input_detect_disconnect setting, which is described in
// jrk_settings_set_input_detect_disconnect.
JRK_API
bool jrk_settings_get_input_detect_disconnect(const jrk_settings *);

// Sets the input_analog_samples_exponent setting.
//
// This setting specifies how many analog samples to take if the input mode
// is analog.  The number of samples will be 2^x, where x is this setting.
JRK_API
void jrk_settings_set_input_analog_samples_exponent(jrk_settings *,
  uint8_t input_analog_samples_exponent);

// Gets the input_analog_samples_exponent setting, which is described in
// jrk_settings_set_input_analog_samples_exponent.
JRK_API
uint8_t jrk_settings_get_input_analog_samples_exponent(const jrk_settings *);

// Sets the feedback_mode setting.
//
// The feedback mode setting specifies whether the Jrk is using feedback from
// the output of the system, and if so defines what type of feedback to use.
//
// - If the feedback mode is "None" (JRK_FEEDBACK_MODE_NONE), feedback and PID
//   calculations are disabled, and the Jrk will do open-loop control.
//   The duty cycle target variable is always equal to the target variable
//   minus 2048, instead of being the result of a PID calculation.  This means
//   that a target of 2648 corresponds to driving the motor full speed forward,
//   2048 is stopped, and 1448 is full-speed reverse.
//
// - If the feedback mode is "Analog" (JRK_FEEDBACK_MODE_ANALOG), the Jrk gets
//   its feedback by measuring the voltage on the FBA pin.  A level of 0 V
//   corresponds to a feedback value of 0, and a level of 5 V corresponds to a
//   feedback value of 4092.  The feedback scaling algorithm computes the scaled
//   feedback variable, and the PID algorithm uses the scaled feedback and the
//   target to compute the duty cycle target.
//
// - If the feedback mode is "Frequency (speed control)"
//   (JRK_FEEDBACK_MODE_FREQUENCY), the Jrk gets it feedback by measuring the
//   frequency of a digital signal on the FBT pin.  See the fbt_method setting.
JRK_API
void jrk_settings_set_feedback_mode(jrk_settings *,
  uint8_t feedback_mode);

// Gets the feedback_mode setting, which is described in
// jrk_settings_set_feedback_mode.
JRK_API
uint8_t jrk_settings_get_feedback_mode(const jrk_settings *);

// Sets the feedback_error_minimum setting.
//
// If the raw feedback value is below this value, it causes a
// "Feedback disconnect" error.
JRK_API
void jrk_settings_set_feedback_error_minimum(jrk_settings *,
  uint16_t feedback_error_minimum);

// Gets the feedback_error_minimum setting, which is described in
// jrk_settings_set_feedback_error_minimum.
JRK_API
uint16_t jrk_settings_get_feedback_error_minimum(const jrk_settings *);

// Sets the feedback_error_maximum setting.
//
// If the raw feedback value exceeds this value, it causes a
// "Feedback disconnect" error.
JRK_API
void jrk_settings_set_feedback_error_maximum(jrk_settings *,
  uint16_t feedback_error_maximum);

// Gets the feedback_error_maximum setting, which is described in
// jrk_settings_set_feedback_error_maximum.
JRK_API
uint16_t jrk_settings_get_feedback_error_maximum(const jrk_settings *);

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

// Sets the feedback_detect_disconnect setting.
//
// If the feedback mode is JRK_FEEDBACK_MODE_ANALOG, this setting causes the
// Jrk to drive its feedback potentiometer power pin (AUX) low once per PID
// period after measuring FBA.  If the voltage on the FBA pin does not drop
// by at least a factor of two while AUX is low, then the Jrk will report a
// "Feedback disconnect" error (if that error is enabled).
JRK_API
void jrk_settings_set_feedback_detect_disconnect(jrk_settings *,
  bool feedback_detect_disconnect);

// Gets the feedback_detect_disconnect setting, which is described in
// jrk_settings_set_feedback_detect_disconnect.
JRK_API
bool jrk_settings_get_feedback_detect_disconnect(const jrk_settings *);

// Sets the feedback_dead_zone setting.
//
// The Jrk sets the duty cycle target to zero and resets the integral
// whenever the magnitude of the error is smaller than this setting. This is
// useful for preventing the motor from driving when the target is very close to
// scaled feedback.
//
// The Jrk uses hysteresis to keep the system from simply riding the edge of the
// feedback dead zone; once in the dead zone, the duty cycle and integral will
// remain zero until the magnitude of the error exceeds twice the value of the
// dead zone.
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
// is analog.  The number of samples will be 2^x, where x is this setting.
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
// around, so that 0 is next to 4095.  The Jrk will know how to take the
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
//
// The serial mode determines how bytes are transferred between the Jrk's UART
// (TX and RX pins), its two USB virtual serial ports (the command port and the
// TTL Port), and its serial command processor.
//
// - If the serial mode is "USB dual port" (JRK_SERIAL_MODE_USB_DUAL_PORT), the
//   command port can be used to send commands to the Jrk and receive responses
//   from it, while the TTL port can be used to send and receives bytes on the
//   TX and RX lines.  The baud rate set by the USB host on the TTL port
//   determines the baud rate used on the TX and RX lines.
//
// - If the serial mode is "USB chained" (JRK_SERIAL_MODE_USB_CHAINED), the
//   command port can be used to both transmit bytes on the TX line and send
//   commands to the Jrk.  The Jrk's responses to those commands will be sent
//   to the command port but not the TX line.  If the RX line is enabled as a
//   serial line, bytes received on the RX line will be sent to the command
//   port but will not be interpreted as command bytes by the Jrk.  The baud
//   rate set by the USB host on the command port determines the baud rate
//   used on the TX and RX lines.
//
// - If the serial mode is "UART" (JRK_SERIAL_MODE_UART), the TX and RX lines
//   can be used to send commands to the Jrk and receive responses from it.  Any
//   byte received on RX will be sent to the command port, but bytes sent from
//   the command port will be ignored.
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
// This is the time in milliseconds before the device considers it to be an
// error if it has not received certain commands.  A value of 0 disables the
// command timeout feature.
//
// This setting should be a multiple of 10 (JRK_SERIAL_TIMEOUT_UNITS) and be
// between 0 and 655350 (JRK_MAX_ALLOWED_SERIAL_TIMEOUT).
JRK_API
void jrk_settings_set_serial_timeout(jrk_settings *,
  uint32_t serial_timeout);

// Gets the serial_timeout setting, which is described in
// jrk_settings_set_serial_timeout.
JRK_API
uint32_t jrk_settings_get_serial_timeout(const jrk_settings *);

// Sets the serial_device_number setting.
//
// This is the serial device number used in the Pololu protocol on the Jrk's
// serial interfaces, and the I2C device address used on the Jrk's I2C
// interface.
//
// By default, the Jrk only pays attention to the lower 7 bits of this setting,
// but if you enable 14-bit serial device numbers (see
// serial_enable_14bit_device_number) then it will use the lower 14 bits.
//
// To avoid user confusion about the ignored bits, the jrk_settings_fix()
// function clears those bits and warns the user.
JRK_API
void jrk_settings_set_serial_device_number(jrk_settings *,
  uint16_t serial_device_number);

// Gets the serial_device_number setting, which is described in
// jrk_settings_set_serial_device_number.
JRK_API
uint16_t jrk_settings_get_serial_device_number(const jrk_settings *);

// Sets the never_sleep setting.
//
// By default, if the Jrk is powered from a USB bus that is in suspend mode
// (e.g. the computer is sleeping) and VIN power is not present, it will go to
// sleep to reduce its current consumption and comply with the USB
// specification.  If you enable the "Never sleep" option, the Jrk will never go
// to sleep.
JRK_API
void jrk_settings_set_never_sleep(jrk_settings *,
  bool never_sleep);

// Gets the never_sleep setting, which is described in
// jrk_settings_set_never_sleep.
JRK_API
bool jrk_settings_get_never_sleep(const jrk_settings *);

// Sets the serial_enable_crc setting.
//
// If set to true, the Jrk requires a 7-bit CRC byte at the end of each serial
// command, and if the CRC byte is wrong then it ignores the command and sets
// the serial CRC error bit.
JRK_API
void jrk_settings_set_serial_enable_crc(jrk_settings *,
  bool serial_enable_crc);

// Gets the serial_enable_crc setting, which is described in
// jrk_settings_set_serial_enable_crc.
JRK_API
bool jrk_settings_get_serial_enable_crc(const jrk_settings *);

// Sets the serial_enable_14bit_device_number setting.
//
// If enabled, the Jrk's Pololu protocol will require a 14-bit device number to
// be sent with every command.  This option allows you to put more than 128 Jrk
// devices on one serial bus.
JRK_API
void jrk_settings_set_serial_enable_14bit_device_number(jrk_settings *,
  bool serial_enable_14bit_device_number);

// Gets the serial_enable_14bit_device_number setting, which is described in
// jrk_settings_set_serial_enable_14bit_device_number.
JRK_API
bool jrk_settings_get_serial_enable_14bit_device_number(const jrk_settings *);

// Sets the serial_disable_compact_protocol setting.
//
// If enabled, the Jrk will not respond to compact protocol commands.
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
// In the PID algorithm, the integral variable (known as error sum)
// is multiplied by a number called the integral coefficient to
// determine its effect on the motor duty cycle.
//
// The integral coefficient is defined by this mathematical expression:
//   integral_multiplier / 2^(integral_exponent)
//
// Note: On the original Jrks (Jrk 12v12 and Jrk 21v3), the formula was
// different.  Those Jrks added 3 to the integral_exponent before using
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
// The PID period specifies how often the Jrk should calculate its input and
// feedback, run its PID calculation, and update the motor speed, in units of
// milliseconds.  This period is still used even if feedback and PID are
// disabled.
JRK_API
void jrk_settings_set_pid_period(jrk_settings *,
  uint16_t pid_period);

// Gets the pid_period setting, which is described in
// jrk_settings_set_pid_period.
JRK_API
uint16_t jrk_settings_get_pid_period(const jrk_settings *);

// Sets the integral_divider_exponent setting.
//
// Causes the integral variable to accumulate more slowly.
JRK_API
void jrk_settings_set_integral_divider_exponent(jrk_settings *,
  uint8_t integral_divider_exponent);

// Gets the integral_divider_exponent setting, which is described in
// jrk_settings_set_integral_divider_exponent.
JRK_API
uint8_t jrk_settings_get_integral_divider_exponent(const jrk_settings *);

// Sets the integral_limit setting.
//
// The PID algorithm prevents the absolute value of the integral variable
// (also known as error sum) from exceeding this limit.  This can help limit
// integral wind-up.  The limit can range from 0 to 32767.
//
// Note that the maximum value of the integral term can be computed as the
// integral coefficient times the integral limit: if this is very small
// compared to 600 (maximum duty cycle), the integral term will have at most
// a very small effect on the duty cycle.
JRK_API
void jrk_settings_set_integral_limit(jrk_settings *,
  uint16_t integral_limit);

// Gets the integral_limit setting, which is described in
// jrk_settings_set_integral_limit.
JRK_API
uint16_t jrk_settings_get_integral_limit(const jrk_settings *);

// Sets the reset_integral setting.
//
// If this setting is set to true, the PID algorithm will reset the integral
// variable (also known as error sum) whenever the absolute value of the
// proportional term (see proportional_multiplier) exceeds 600.
JRK_API
void jrk_settings_set_reset_integral(jrk_settings *,
  bool reset_integral);

// Gets the reset_integral setting, which is described in
// jrk_settings_set_reset_integral.
JRK_API
bool jrk_settings_get_reset_integral(const jrk_settings *);

// Sets the pwm_frequency setting.
//
// This setting specifies whether to use 20 kHz (the default) or 5 kHz for the
// motor PWM signal.  This setting should be either
// JRK_PWM_FREQUENCY_20 or JRK_PWM_FREQUENCY_5.
JRK_API
void jrk_settings_set_pwm_frequency(jrk_settings *,
  uint8_t pwm_frequency);

// Gets the pwm_frequency setting, which is described in
// jrk_settings_set_pwm_frequency.
JRK_API
uint8_t jrk_settings_get_pwm_frequency(const jrk_settings *);

// Sets the current_samples_exponent setting.
//
// This setting specifies how many analog samples to take when measuring
// the current.  The number of samples will be 2^x, where x is this setting.
JRK_API
void jrk_settings_set_current_samples_exponent(jrk_settings *,
  uint8_t current_samples_exponent);

// Gets the current_samples_exponent setting, which is described in
// jrk_settings_set_current_samples_exponent.
JRK_API
uint8_t jrk_settings_get_current_samples_exponent(const jrk_settings *);

// Sets the hard_overcurrent_threshold setting.
//
// This is the number of consecutive PID periods where the the hardware current
// chopping must occur before the Jrk triggers a "Hard overcurrent" error.
// The default of 1 means that any current chopping is an error.  You can set it
// to a higher value if you expect some current chopping to happen (e.g. when
// starting up) but you still want to it to be an error when your motor leads
// are shorted out.
//
// This setting is not used on the umc06a since it cannot sense when hardware
// current chopping happens.
JRK_API
void jrk_settings_set_hard_overcurrent_threshold(jrk_settings *,
  uint8_t hard_overcurrent_threshold);

// Gets the hard_overcurrent_threshold setting, which is described in
// jrk_settings_set_hard_overcurrent_threshold.
JRK_API
uint8_t jrk_settings_get_hard_overcurrent_threshold(const jrk_settings *);

// Sets the current_offset_calibration setting.
//
// You can use this current calibration setting to correct current measurements
// and current limit settings.
//
// The current sense circuitry on a umc04a/umc05a Jrks produces a constant
// voltage of about 50 mV (but with large variations from unit to unit) when the
// motor driver is powered, even if there is no current flowing through the
// motor.  This offset must be subtracted from analog voltages representing
// current limits or current measurements as one of the first steps for
// converting those voltages to amps.
//
// For the umc04a/umc05a Jrk models, this setting is defined by the formula:
//
//   current_offset_calibration = (voltage offset in millivolts - 50) * 16
//
// For the umc04a/umc05a Jrk models, this setting should be between -800
// (for an offset of 0 mV) and 800 (for an offset of 100 mV).
//
// For the umc06a, this setting can be any int16_t value and has units of mV/16.
JRK_API
void jrk_settings_set_current_offset_calibration(jrk_settings *,
  int16_t current_offset_calibration);

// Gets the current_offset_calibration setting, which is described in
// jrk_settings_set_current_offset_calibration.
JRK_API
int16_t jrk_settings_get_current_offset_calibration(const jrk_settings *);

// Sets the current_scale_calibration setting.
//
// You can use this current calibration setting to correct current measurements
// and current limit settings that are off by a constant percentage.
//
// For the umc04a/umc05a models, the algorithm for calculating currents in
// milliamps involves multiplying the current by
// (1875 + current_scale_calibration).
// This setting must be between -1875 and 1875.
//
// For the umc06a models, the algorithm for calculating currents in
// milliamps involves multiplying the current by
// (1136 + current_scale_calibration).
// This setting must be between -1136 and 1136.
//
// The default current_scale_calibration value is 0.
JRK_API
void jrk_settings_set_current_scale_calibration(jrk_settings *,
  int16_t current_scale_calibration);

// Gets the current_scale_calibration setting, which is described in
// jrk_settings_set_current_scale_calibration.
JRK_API
int16_t jrk_settings_get_current_scale_calibration(const jrk_settings *);

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

// Sets the max_duty_cycle_while_feedback_out_of_range setting.
//
// If the feedback is beyond the range specified by the feedback error
// minimum and feedback error maximum values, then the duty cycle's magnitude
// cannot exceed this value.
//
// This option helps limit possible damage to systems by reducing the maximum
// duty cycle whenever the feedback is outside the range specified by the
// feedback error minimum and feedback error maximum values.  This can be
// used, for example, to slowly bring a system back into its valid range of
// operation when it is dangerously near a limit.  The Feedback disconnect
// error should be disabled when this option is used.
JRK_API
void jrk_settings_set_max_duty_cycle_while_feedback_out_of_range(jrk_settings *,
  uint16_t max_duty_cycle_while_feedback_out_of_range);

// Gets the max_duty_cycle_while_feedback_out_of_range setting, which is described in
// jrk_settings_set_max_duty_cycle_while_feedback_out_of_range.
JRK_API
uint16_t jrk_settings_get_max_duty_cycle_while_feedback_out_of_range(const jrk_settings *);

// Sets the max_acceleration_forward setting.
//
// This is the maximum allowed acceleration in the forward direction.
//
// This is the maximum amount that the duty cycle can increase during each PID
// period if the duty cycle is positive.
JRK_API
void jrk_settings_set_max_acceleration_forward(jrk_settings *,
  uint16_t max_acceleration_forward);

// Gets the max_acceleration_forward setting, which is described in
// jrk_settings_set_max_acceleration_forward.
JRK_API
uint16_t jrk_settings_get_max_acceleration_forward(const jrk_settings *);

// Sets the max_acceleration_reverse setting.
//
// This is the maximum allowed acceleration in the reverse direction.
//
// This is the maximum amount that the duty cycle can decrease during each PID
// period if the duty cycle is negative.
JRK_API
void jrk_settings_set_max_acceleration_reverse(jrk_settings *,
  uint16_t max_acceleration_reverse);

// Gets the max_acceleration_reverse setting, which is described in
// jrk_settings_set_max_acceleration_reverse.
JRK_API
uint16_t jrk_settings_get_max_acceleration_reverse(const jrk_settings *);

// Sets the max_deceleration_forward setting.
//
// This is the maximum allowed deceleration in the forward direction.
//
// This is the maximum amount that the duty cycle can decrease during each PID
// period if the duty cycle is positive.
JRK_API
void jrk_settings_set_max_deceleration_forward(jrk_settings *,
  uint16_t max_deceleration_forward);

// Gets the max_deceleration_forward setting, which is described in
// jrk_settings_set_max_deceleration_forward.
JRK_API
uint16_t jrk_settings_get_max_deceleration_forward(const jrk_settings *);

// Sets the max_deceleration_reverse setting.
//
// This is the maximum allowed deceleration in the reverse direction.
//
// This is the maximum amount that the duty cycle can increase during each PID
// period if the duty cycle is negative.
JRK_API
void jrk_settings_set_max_deceleration_reverse(jrk_settings *,
  uint16_t max_deceleration_reverse);

// Gets the max_deceleration_reverse setting, which is described in
// jrk_settings_set_max_deceleration_reverse.
JRK_API
uint16_t jrk_settings_get_max_deceleration_reverse(const jrk_settings *);

// Sets the max_duty_cycle_forward setting.
//
// This is the maximum allowed duty cycle in the forward direction.
//
// Positive duty cycles cannot exceed this number.
//
// A value of 600 means 100%.
JRK_API
void jrk_settings_set_max_duty_cycle_forward(jrk_settings *,
  uint16_t max_duty_cycle_forward);

// Gets the max_duty_cycle_forward setting, which is described in
// jrk_settings_set_max_duty_cycle_forward.
JRK_API
uint16_t jrk_settings_get_max_duty_cycle_forward(const jrk_settings *);

// Sets the max_duty_cycle_reverse setting.
//
// This is the maximum allowed duty cycle in the reverse direction.
//
// Negative duty cycles cannot go below this number negated.
//
// A value of 600 means 100%.
JRK_API
void jrk_settings_set_max_duty_cycle_reverse(jrk_settings *,
  uint16_t max_duty_cycle_reverse);

// Gets the max_duty_cycle_reverse setting, which is described in
// jrk_settings_set_max_duty_cycle_reverse.
JRK_API
uint16_t jrk_settings_get_max_duty_cycle_reverse(const jrk_settings *);

// Sets the encoded_hard_current_limit_forward setting.
//
// Sets the current limit to be used when driving forward.
//
// This setting is not actually a current, it is an encoded value telling
// the Jrk how to set up its current limiting hardware.
//
// This setting is not used for the umc06a, since it does not have a
// configurable hardware current limiting.
//
// The correspondence between this setting and the actual current limit
// in milliamps depends on what product you are using.  See also:
//
// - jrk_current_limit_decode()
// - jrk_current_limit_encode()
// - jrk_get_recommended_encoded_hard_current_limits()
JRK_API
void jrk_settings_set_encoded_hard_current_limit_forward(jrk_settings *,
  uint16_t encoded_hard_current_limit_forward);

// Gets the encoded_hard_current_limit_forward setting, which is described in
// jrk_settings_set_encoded_hard_current_limit_forward.
JRK_API
uint16_t jrk_settings_get_encoded_hard_current_limit_forward(const jrk_settings *);

// Sets the encoded_hard_current_limit_reverse setting.
//
// Sets the current limit to be used when driving in reverse.
// See the documentation of encoded_hard_current_limit_forward.
JRK_API
void jrk_settings_set_encoded_hard_current_limit_reverse(jrk_settings *,
  uint16_t encoded_hard_current_limit_reverse);

// Gets the encoded_hard_current_limit_reverse setting, which is described in
// jrk_settings_set_encoded_hard_current_limit_reverse.
JRK_API
uint16_t jrk_settings_get_encoded_hard_current_limit_reverse(const jrk_settings *);

// Sets the brake_duration_forward setting.
//
// The number of milliseconds to spend braking before starting to drive forward.
//
// This setting should be a multiple of 5 (JRK_BRAKE_DURATION_UNITS) and be
// between 0 and 5 * 255 (JRK_MAX_ALLOWED_BRAKE_DURATION).
JRK_API
void jrk_settings_set_brake_duration_forward(jrk_settings *,
  uint32_t brake_duration_forward);

// Gets the brake_duration_forward setting, which is described in
// jrk_settings_set_brake_duration_forward.
JRK_API
uint32_t jrk_settings_get_brake_duration_forward(const jrk_settings *);

// Sets the brake_duration_reverse setting.
//
// The number of milliseconds to spend braking before starting to drive in
// reverse.
//
// This setting should be a multiple of 5 (JRK_BRAKE_DURATION_UNITS) and be
// between 0 and 5 * 255 (JRK_MAX_ALLOWED_BRAKE_DURATION).
JRK_API
void jrk_settings_set_brake_duration_reverse(jrk_settings *,
  uint32_t brake_duration_reverse);

// Gets the brake_duration_reverse setting, which is described in
// jrk_settings_set_brake_duration_reverse.
JRK_API
uint32_t jrk_settings_get_brake_duration_reverse(const jrk_settings *);

// Sets the soft_current_limit_forward setting.
//
// This is the maximum current while driving forward.  If the current exceeds
// this value, the Jrk will trigger a "Max. current exceeded" error.
//
// A value of 0 means no limit.
//
// The units for this setting are milliamps.
JRK_API
void jrk_settings_set_soft_current_limit_forward(jrk_settings *,
  uint16_t soft_current_limit_forward);

// Gets the soft_current_limit_forward setting, which is described in
// jrk_settings_set_soft_current_limit_forward.
JRK_API
uint16_t jrk_settings_get_soft_current_limit_forward(const jrk_settings *);

// Sets the soft_current_limit_reverse setting.
//
// This is the maximum current while driving in reverse.  If the current exceeds
// this value, the Jrk will trigger a "Max. current exceeded" error.
//
// A value of 0 means no limit.
//
// The units for this setting are milliamps.
JRK_API
void jrk_settings_set_soft_current_limit_reverse(jrk_settings *,
  uint16_t soft_current_limit_reverse);

// Gets the soft_current_limit_reverse setting, which is described in
// jrk_settings_set_soft_current_limit_reverse.
JRK_API
uint16_t jrk_settings_get_soft_current_limit_reverse(const jrk_settings *);

// Sets the soft_current_regulation_level_forward setting.
//
// If this setting is non-zero and the Jrk is driving the motor forward, the Jrk
// will attempt to prevent the motor current from exceeding this value by
// limiting the duty cycle using a simple linear formula.
//
// A value of 0 disables software current regulation.
//
// This feature is not supported on the umc04a/umc05a Jrks since they have
// hardware current regulation.
//
// The units for this setting are milliamps.
JRK_API
void jrk_settings_set_soft_current_regulation_level_forward(jrk_settings *,
  uint16_t soft_current_regulation_level_forward);

// Gets the soft_current_regulation_level_forward setting, which is described in
// jrk_settings_set_soft_current_regulation_level_forward.
JRK_API
uint16_t jrk_settings_get_soft_current_regulation_level_forward(const jrk_settings *);

// Sets the soft_current_regulation_level_reverse setting.
//
// If this setting is non-zero and the Jrk is driving the motor in reverse, the
// Jrk will attempt to prevent the motor current from exceeding this value by
// limiting the duty cycle using a simple linear formula.
//
// A value of 0 disables software current regulation.
//
// This feature is not supported on the umc04a/umc05a Jrks since they have
// hardware current regulation.
//
// The units for this setting are milliamps.
JRK_API
void jrk_settings_set_soft_current_regulation_level_reverse(jrk_settings *,
  uint16_t soft_current_regulation_level_reverse);

// Gets the soft_current_regulation_level_reverse setting, which is described in
// jrk_settings_set_soft_current_regulation_level_reverse.
JRK_API
uint16_t jrk_settings_get_soft_current_regulation_level_reverse(const jrk_settings *);

// Sets the coast_when_off setting.
//
// By default, the Jrk drives both motor outputs low when the motor is
// stopped (duty cycle is zero or there is an error), causing it to brake.
// If enabled, this setting causes it to instead tri-state both outputs,
// making the motor coast.
JRK_API
void jrk_settings_set_coast_when_off(jrk_settings *,
  bool coast_when_off);

// Gets the coast_when_off setting, which is described in
// jrk_settings_set_coast_when_off.
JRK_API
bool jrk_settings_get_coast_when_off(const jrk_settings *);

// Sets the error_enable setting.
//
// This setting is a bitmap specifying which errors are enabled.
//
// This includes errors that are enabled and latched.
//
// The JRK_ERROR_* macros specify the bits in the bitmap.  Certain errors are
// always enabled, so the Jrk ignores the bits for those errors.
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
//
// When a latched error occurs, the Jrk will not clear the corresponding error
// bit (and thus not restart the motor) until the Jrk receives a command to
// clear the error bits.
//
// The JRK_ERROR_* macros specify the bits in the bitmap.  Certain errors are
// always latched if they are enabled, so the Jrk ignores the bits for those
// errors.
JRK_API
void jrk_settings_set_error_latch(jrk_settings *,
  uint16_t error_latch);

// Gets the error_latch setting, which is described in
// jrk_settings_set_error_latch.
JRK_API
uint16_t jrk_settings_get_error_latch(const jrk_settings *);

// Sets the error_hard setting.
//
// This setting is a bitmap specifying which errors are hard errors.
//
// If a hard error is enabled and it happens, the Jrk will set the motor's duty
// cycle to 0 immediately without respecting deceleration limits.
//
// The JRK_ERROR_* macros specify the bits in the bitmap.  Certain errors are
// always hard errors, so the Jrk ignores the bits for those errors.
JRK_API
void jrk_settings_set_error_hard(jrk_settings *,
  uint16_t error_hard);

// Gets the error_hard setting, which is described in
// jrk_settings_set_error_hard.
JRK_API
uint16_t jrk_settings_get_error_hard(const jrk_settings *);

// Sets the vin_calibration setting.
//
// The firmware uses this calibration factor when calculating the VIN voltage.
// One of the steps in the process is to multiply the VIN voltage reading by
// (825 + vin_calibration).
//
// So for every 8 counts that you add or subtract from the vin_calibration
// setting, you increase or decrease the VIN voltage reading by about 1%.
JRK_API
void jrk_settings_set_vin_calibration(jrk_settings *,
  int16_t vin_calibration);

// Gets the vin_calibration setting, which is described in
// jrk_settings_set_vin_calibration.
JRK_API
int16_t jrk_settings_get_vin_calibration(const jrk_settings *);

// Sets the disable_i2c_pullups setting.
//
// This option disables the internal pull-up resistors on the SDA/AN and SCL
// pins if those pins are being used for I2C communication.
JRK_API
void jrk_settings_set_disable_i2c_pullups(jrk_settings *,
  bool disable_i2c_pullups);

// Gets the disable_i2c_pullups setting, which is described in
// jrk_settings_set_disable_i2c_pullups.
JRK_API
bool jrk_settings_get_disable_i2c_pullups(const jrk_settings *);

// Sets the analog_sda_pullup setting.
//
// This option enables the internal pull-up resistor on the SDA/AN pin if it is
// being used as an analog input.
JRK_API
void jrk_settings_set_analog_sda_pullup(jrk_settings *,
  bool analog_sda_pullup);

// Gets the analog_sda_pullup setting, which is described in
// jrk_settings_set_analog_sda_pullup.
JRK_API
bool jrk_settings_get_analog_sda_pullup(const jrk_settings *);

// Sets the always_analog_sda setting.
//
// This option causes the Jrk to perform analog measurements on the SDA/AN pin
// and configure SCL as a potentiometer power pin even if the "Input mode"
// setting is not "Analog".
JRK_API
void jrk_settings_set_always_analog_sda(jrk_settings *,
  bool always_analog_sda);

// Gets the always_analog_sda setting, which is described in
// jrk_settings_set_always_analog_sda.
JRK_API
bool jrk_settings_get_always_analog_sda(const jrk_settings *);

// Sets the always_analog_fba setting.
//
// This option causes the Jrk to perform analog measurements on the FBA pin
// even if the "Feedback mode" setting is not "Analog".
JRK_API
void jrk_settings_set_always_analog_fba(jrk_settings *,
  bool always_analog_fba);

// Gets the always_analog_fba setting, which is described in
// jrk_settings_set_always_analog_fba.
JRK_API
bool jrk_settings_get_always_analog_fba(const jrk_settings *);

// Sets the fbt_method setting.
//
// This settings specifies what kind of pulse measurement to perform
// on the FBT pin.
//
// JRK_FBT_METHOD_PULSE_COUNTING means the Jrk will count the number of
// rising edges on the pin, and is more suitable for fast tachometers.
//
// JRK_FBT_METHOD_PULSE_TIMING means the Jrk will measure the pulse width
// (duration) of pulses on the pin, and is more suitable for slow tachometers.
JRK_API
void jrk_settings_set_fbt_method(jrk_settings *,
  uint8_t fbt_method);

// Gets the fbt_method setting, which is described in
// jrk_settings_set_fbt_method.
JRK_API
uint8_t jrk_settings_get_fbt_method(const jrk_settings *);

// Sets the fbt_timing_clock setting.
//
// This specifies the speed of the clock (in MHz) to use for pulse timing on the
// FBT pin.  The options are:
//
// - JRK_FBT_TIMING_CLOCK_1_5: 1.5 MHz
// - JRK_FBT_TIMING_CLOCK_3: 3 MHz
// - JRK_FBT_TIMING_CLOCK_6: 6 MHz
// - JRK_FBT_TIMING_CLOCK_12: 12 MHz
// - JRK_FBT_TIMING_CLOCK_24: 24 MHz
// - JRK_FBT_TIMING_CLOCK_48: 48 MHz
JRK_API
void jrk_settings_set_fbt_timing_clock(jrk_settings *,
  uint8_t fbt_timing_clock);

// Gets the fbt_timing_clock setting, which is described in
// jrk_settings_set_fbt_timing_clock.
JRK_API
uint8_t jrk_settings_get_fbt_timing_clock(const jrk_settings *);

// Sets the fbt_timing_polarity setting.
//
// By default, the pulse timing mode on the FBT pin measures the time of
// high pulses.  When true, this option causes it to measure low pulses.
JRK_API
void jrk_settings_set_fbt_timing_polarity(jrk_settings *,
  bool fbt_timing_polarity);

// Gets the fbt_timing_polarity setting, which is described in
// jrk_settings_set_fbt_timing_polarity.
JRK_API
bool jrk_settings_get_fbt_timing_polarity(const jrk_settings *);

// Sets the fbt_timing_timeout setting.
//
// The pulse timing mode for the FBT pin will assume the motor has stopped, and
// start recording maximum-width pulses if it has not seen any pulses in this
// amount of time.
JRK_API
void jrk_settings_set_fbt_timing_timeout(jrk_settings *,
  uint16_t fbt_timing_timeout);

// Gets the fbt_timing_timeout setting, which is described in
// jrk_settings_set_fbt_timing_timeout.
JRK_API
uint16_t jrk_settings_get_fbt_timing_timeout(const jrk_settings *);

// Sets the fbt_samples setting.
//
// The number of consecutive FBT measurements to average together in pulse
// timing mode or to add together in pulse counting mode.
JRK_API
void jrk_settings_set_fbt_samples(jrk_settings *,
  uint8_t fbt_samples);

// Gets the fbt_samples setting, which is described in
// jrk_settings_set_fbt_samples.
JRK_API
uint8_t jrk_settings_get_fbt_samples(const jrk_settings *);

// Sets the fbt_divider_exponent setting.
//
// This setting specifies how many bits to shift the raw tachomter reading to
// the right before using it to calculate the "feedback" variable.  The
// default value is 0.
JRK_API
void jrk_settings_set_fbt_divider_exponent(jrk_settings *,
  uint8_t fbt_divider_exponent);

// Gets the fbt_divider_exponent setting, which is described in
// jrk_settings_set_fbt_divider_exponent.
JRK_API
uint8_t jrk_settings_get_fbt_divider_exponent(const jrk_settings *);

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
//
// The input variable is a raw, unscaled value representing a measurement taken
// by the Jrk of the input to the system.  In serial input mode, the input is
// equal to the target, which can be set to any value from 0 to 4095 using
// serial commands.  In analog input mode, the input is a measurement the
// voltage on the SDA pin, where 0 is 0 V and 4092 is a voltage equal the Jrk's
// 5V pin (approximately 4.8 V).  In RC input mode, the input is the
// duration of the last pulse measured, in units of 2/3 us.
JRK_API
uint16_t jrk_variables_get_input(const jrk_variables *);

// Gets the target variable.
//
// In serial input mode, the target is set directly with serial commands.  In
// the other input modes, the target is computed by scaling the input, using the
// configurable input scaling settings.
JRK_API
uint16_t jrk_variables_get_target(const jrk_variables *);

// Gets the feedback variable.
//
// The feedback variable is a raw, unscaled feedback value representing a
// measurement taken by the Jrk of the output of the system.  In analog mode,
// the feedback is a measurement of the voltage on the FBA pin, where 0 is 0 V
// and 4092 is a voltage equal to the Jrk's 5V pin (approximately 4.8 V).  In
// frequency feedback mode, the feedback is 2048 plus or minus a measurement of
// the frequency of pulses on the FBT pin.  In feedback mode none (open-loop
// speed control mode), the feedback is always zero.
JRK_API
uint16_t jrk_variables_get_feedback(const jrk_variables *);

// Gets the scaled_feedback variable.
//
// The scaled feedback is calculated from the feedback using the Jrk's
// configurable feedback scaling settings.
JRK_API
uint16_t jrk_variables_get_scaled_feedback(const jrk_variables *);

// Gets the integral variable.
//
// In general, every PID period, the error (scaled feedback minus target) is
// added to the integral (also known as error sum).  There are several settings
// to configure the behavior of this variable, and it is used in the PID
// calculation.
JRK_API
int16_t jrk_variables_get_integral(const jrk_variables *);

// Gets the duty_cycle_target variable.
//
// In general, this is the duty cycle that the jrk is trying to achieve.  A
// value of -600 or less means full speed reverse, while a value of 600 or more
// means full speed forward.  A value of 0 means stopped (braking or coasting).
// In no feedback mode (open-loop speed control mode), the duty cycle target is
// normally the target minus 2048. In other feedback modes, the duty cycle
// target is normally the sum of the proportional, integral, and derivative
// terms of the PID algorithm.  In any mode, the duty cycle target can be
// overridden with a jrk_force_duty_cycle_target().
//
// If an error is stopping the motor, the duty cycle target variable will not be
// directly affected, but the duty cycle variable will change/decelerate to
// zero.
JRK_API
int16_t jrk_variables_get_duty_cycle_target(const jrk_variables *);

// Gets the duty_cycle variable.
//
// The duty cycle variable is the duty cycle at which the jrk is currently
// driving the motor.  A value of -600 means full speed reverse, while a value
// of 600 means full speed forward.  A value of 0 means stopped (braking or
// coasting).  The duty cycle could be different from the duty cycle target
// because it normally takes into account the Jrk's configurable motor limits
// and errors.  The duty cycle can be overridden with jrk_force_duty_cycle().
JRK_API
int16_t jrk_variables_get_duty_cycle(const jrk_variables *);

// Gets the current_low_res variable.
//
// This is the most-significant 8 bits of the 'current' variable returned by
// jrk_variables_get_current().
//
// The Jrk G2 has this variable mainly to be compatible with older Jrk models.
// In new applications, we recommend using jrk_variables_get_current(), which
// provides a higher-resolution measurement.
JRK_API
uint8_t jrk_variables_get_current_low_res(const jrk_variables *);

// Gets the pid_period_exceeded variable.
//
// This variable is true if the Jrk's most recent PID cycle took more time than
// the configured PID period.  This indicates that the Jrk does not have time to
// perform all of its tasks at the desired rate.  Most often, this is caused by
// the configured number of analog samples for input, feedback, or current
// sensing being too high for the configured PID period.
JRK_API
bool jrk_variables_get_pid_period_exceeded(const jrk_variables *);

// Gets the pid_period_count variable.
//
// This is the number of PID periods that have elapsed.  It resets to 0 after
// reaching 65535.
JRK_API
uint16_t jrk_variables_get_pid_period_count(const jrk_variables *);

// Gets the error_flags_halting variable.
//
// This variable indicates which errors are currently stopping the motor.
//
// Each bit in the variable represents a different error.  The bits are defined
// by the JRK_ERROR_* macros.
JRK_API
uint16_t jrk_variables_get_error_flags_halting(const jrk_variables *);

// Gets the error_flags_occurred variable.
//
// This variable indicates which errors have occurred since the last time the
// variable was cleared.
//
// Like error flags halting, each bit in the variable represents a different
// error.  The bits are defined by the JRK_ERROR_* macros.
JRK_API
uint16_t jrk_variables_get_error_flags_occurred(const jrk_variables *);

// Gets the vin_voltage variable.
//
// This is the measurement of the voltage supplied to the Jrk's VIN pin,
// in millivolts.
JRK_API
uint16_t jrk_variables_get_vin_voltage(const jrk_variables *);

// Gets the current variable.
//
// This is the Jrk's measurement of the current running through the motor, in
// milliamps.
JRK_API
uint16_t jrk_variables_get_current(const jrk_variables *);

// Gets the device_reset variable.
//
// This is the cause of the Jrk's last full microcontroller reset.
//
// It should be equal to one of the JRK_RESET_* macros.
JRK_API
uint8_t jrk_variables_get_device_reset(const jrk_variables *);

// Gets the up_time variable.
//
// This is the time since the last full reset of the Jrk's microcontroller, in
// milliseconds.
JRK_API
uint32_t jrk_variables_get_up_time(const jrk_variables *);

// Gets the rc_pulse_width variable.
//
// This is the raw RC pulse width measured on the Jrk's RC input, in units of
// twelfths of a microsecond.
//
// Returns 0 if the RC input is missing or invalid.
JRK_API
uint16_t jrk_variables_get_rc_pulse_width(const jrk_variables *);

// Gets the fbt_reading variable.
//
// This is the raw pulse rate or pulse width measured on the Jrk's FBT
// (tachometer feedback) pin.
//
// In pulse counting mode, this will be the number of pulses on the FBT pin seen
// in the last N PID periods, where N is the "Pulse samples" setting.
//
// In pulse timing mode, this will be a measurement of the width of pulses on
// the FBT pin.  This measurement is affected by several configurable settings.
JRK_API
uint16_t jrk_variables_get_fbt_reading(const jrk_variables *);

// Gets the raw_current variable.
//
// This is an analog voltage reading from the Jrk's current sense pin.
//
// For the umc04a/umc05a models, the units of the reading depend on what hard
// current limit is being used (jrk_variable_get_encoded_hard_current_limit()).
//
// For the umc06a, the units are always mV / 16.
//
// Either way, use jrk_calculate_raw_current_mv64() to get a raw current
// reading with reliable units.
JRK_API
uint16_t jrk_variables_get_raw_current(const jrk_variables *);

// Gets the encoded_hard_current_limit variable.
//
// This is the encoded value representing the hardware current limit the jrk is
// currently using.
JRK_API
uint16_t jrk_variables_get_encoded_hard_current_limit(const jrk_variables *);

// Gets the last_duty_cycle variable.
//
// This is the duty cycle the Jrk drove the motor with during the last PID
// period.
//
// This can be useful for converting the raw current reading into milliamps.
JRK_API
int16_t jrk_variables_get_last_duty_cycle(const jrk_variables *);

// Gets the current_chopping_consecutive_count variable.
//
// This is the number of consecutive PID periods during which current chopping
// due to the hard current limit has been active.
JRK_API
uint8_t jrk_variables_get_current_chopping_consecutive_count(const jrk_variables *);

// Gets the current_chopping_occurrence_count variable.
//
// This is the number of PID periods during which current chopping due to the
// hard current limit has been active, since the last time the variable was
// cleared.
JRK_API
uint8_t jrk_variables_get_current_chopping_occurrence_count(const jrk_variables *);

// End of auto-generated variables getter prototypes.

// Gets the error, which is the scaled feedback minus the target.
JRK_API
int16_t jrk_variables_get_error(const jrk_variables *);

// Gets the force_mode variable.
//
// This will be one of the following values:
//
// - JRK_FORCE_MODE_NONE: The jrk's movement is not being forced; it will
//   operate normally if there are no errors.
// - JRK_FORCE_MODE_DUTY_CYCLE_TARGET: The duty cycle target is being forced
//   to a particular value due to a previous jrk_force_duty_cycle_target()
//   command that was issued.  The duty cycle target gets ignored if
//   there are any errors.  You can get the forced value from
//   jrk_variables_get_duty_cycle_target().
//   Use jrk_force_duty_cycle_target() to get into this mode.
// - JRK_FORCE_MODE_DUTY_CYCLE:  The duty cycle is being forced to a
//   particular value unless there are errors happening (excluding
//   "Input invalid", "Input disconnect", "Feedback disconnect" errors).
//   You can get the forced value by looking at jrk_variables_get_duty_cycle(),
//   unless important errors are happening as defined above, in which case
//   the duty cycle is controlled normally and will decelerate to 0.
//   Use jrk_force_duty_cycle() to get into this mode.
//
// When the 'Awaiting command' error flag is set, this variable should always be
// JRK_FORCE_MODE_NONE.
JRK_API
uint8_t jrk_variables_get_force_mode(const jrk_variables *);

// Gets the analog reading from the specified pin.
//
// The pin_number argument should be one of the JRK_PIN_NUM_* macros.
//
// The return value will be a left-justified analog reading; a value of 0
// represents 0 V and a value near 0xFFFE represents the voltage on the
// controller's 5V pin.  A value of 0xFFFF means the readings is not available.
//
// See also jrk_variables_get_digital_reading().
JRK_API
uint16_t jrk_variables_get_analog_reading(const jrk_variables *, uint8_t pin);

// Gets the digital reading for the specified pin.
//
// The pin argument should be one of the JRK_PIN_NUM_* macros.
//
// A return value of 0 means low while 1 means high.  In most cases, pins
// configured as analog inputs cannot be read as digital inputs, so their values
// will be 0.  See jrk_variables_get_analog_reading() for those pins.
JRK_API
bool jrk_variables_get_digital_reading(const jrk_variables *, uint8_t pin);


// jrk_device ///////////////////////////////////////////////////////////////////

/// Represents a Jrk that is or was connected to the computer.
typedef struct jrk_device jrk_device;

/// Finds all the supported Jrk devices connected to the computer via USB and
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

/// Gets the command port name (e.g. "COM4").
/// Does not do any caching.
/// The retrieved string must be freed with jrk_string_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_device_get_cmd_port_name(const jrk_device *, char ** name);

/// Gets the TTL port name (e.g. "COM5").
/// Does not do any caching.
/// The retrieved string must be freed with jrk_string_free().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_device_get_ttl_port_name(const jrk_device *, char ** name);


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

/// Sets the target of the Jrk to a value in the range 0 to 4095.
///
/// The target can represent a target duty cycle, speed, or position depending
/// on the feedback mode.
///
/// This functions sends a "Set target" command to the jrk, which clears the
/// "Awaiting command" error bit and (if the input mode is serial) will set
/// the jrk's input and target variables.
///
/// This is the main command you would use to control the Jrk over USB.
///
/// See also jrk_force_duty_cycle_target(), jrk_force_duty_cycle(),
/// jrk_variables_get_target().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_set_target(jrk_handle *, uint16_t target);

/// Turns the motor off.
///
/// This function sends a "Motor off" command to the Jrk, which sets the
/// "Awaiting command" error bit.  The Jrk will respect the configured
/// deceleration limit while decelerating to a duty cycle of 0, unless the
/// "Awaiting command" error has been configured as a hard error.  Once the duty
/// cycle reaches zero, the Jrk will either brake or coast.
///
/// The Jrk will not restart the motor until it receives a
/// jrk_set_target(), jrk_force_duty_cycle_target(), or jrk_force_duty_cycle()
/// command.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_stop_motor(jrk_handle *);

/// Sends a command to the Jrk that causes it to clear latched errors in the
/// "Error flags halting" varible except for "Awaiting command".
///
/// If the optional error_flags pointer is supplied, this function uses it to
/// return the value of the "Error flags halting" variable before it was
/// cleared.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_clear_errors(jrk_handle *, uint16_t * error_flags);

/// This function clears all of the latched errors including the "Awaiting
/// command" error bit.
///
/// It uses two commands to do this:
///
/// 1. A "Get variable" command to fetch the current Target value from the jrk
///    and clear all latched errors except "Awaiting command" as a side effect.
/// 2. A "Set target" command to send the previously-fetched target to the jrk.
///
/// If you want to have control over what target gets set, you should use
/// jrk_clear_errors() and jrk_set_target() separately instead of this function.
///
/// This is the command to use if you want the Jrk to clear all of its errors
/// and run the motor if possible.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_run_motor(jrk_handle *);

/// Forces the duty cycle target of the Jrk to a value in the range
/// -600 to +600.
///
/// The Jrk will ignore the results of the usual algorithm for choosing the duty
/// cycle target, and instead set it to be equal to the target specified by this
/// command.  The Jrk will set its 'Integral' variable to 0 while in this mode.
///
/// This is useful if the Jrk is configured to use feedback but you want to take
/// control of the motor for some time, while still respecting errors and motor
/// limits as usual.
///
/// This function sends a "Force duty cycle target" command to the Jrk G2, which
/// clears the "Awaiting command" error bit.
///
/// To get out of this mode, use jrk_set_target(), jrk_force_duty_cycle(),
/// or jrk_stop_motor().
///
/// See jrk_variables_get_force_mode() and jrk_variables_get_duty_cycle_target().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_force_duty_cycle_target(jrk_handle *, int16_t duty_cycle);

/// Forces the duty cycle of the Jrk to a value in the range -600 to +600.
///
/// The Jrk will ignore the results of the usual algorithm for choosing the duty
/// cycle, and instead set it to be equal to the value specified by this
/// command, ignoring all motor limits except the maximum duty cycle parameters,
/// and ignoring the 'Input invalid', 'Input disconnect', and 'Feedback
/// disconnect' errors.  This command will have an immediate effect, regardless
/// of the PID period.  The Jrk will set its 'Integral' variable to 0 while in
/// this mode.
///
/// This is useful if the Jrk is configured to use feedback but you want to take
/// control of the motor for some time, without respecting most motor limits.
///
/// This function sends a "Force duty cycle" command to the Jrk G2, which
/// clears the "Awaiting command" error bit.
///
/// To get out of this mode, use jrk_set_target(), jrk_force_duty_cycle(),
/// or jrk_stop_motor().
///
/// See jrk_variables_get_force_mode() and jrk_variables_get_duty_cycle_target().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_force_duty_cycle(jrk_handle *, int16_t duty_cycle);

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
/// The flags argument should be zero or a bitwise-or combination of some
/// these flags:
/// - `(1 << JRK_GET_VARIABLES_FLAG_CLEAR_ERROR_FLAGS_HALTING)`:
///   Clears the errors indicated in the "Error flags halting" variable
///   as a side effect (except "Awaiting command").
/// - `(1 << JRK_GET_VARIABLES_FLAG_CLEAR_ERROR_FLAGS_OCCURRED)`:
///   Clears the errors indicated in the "Error flags occurred" variable
///   as a side effect.
/// - `(1 << JRK_GET_VARIABLES_FLAG_CLEAR_CURRENT_CHOPPING_OCCURRENCE_COUNT)`:
///   Clears the "Current chopping occurrence count" variable as a side effect.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_variables(jrk_handle *, jrk_variables ** variables,
  uint16_t flags);

/// Reads the specified bytes from the Jrk variables data structure.
///
/// The index parameter specifies the address of the first byte to read, and the
/// length parameter specifies how many bytes to read.  The output parameter
/// must point to a buffer that can hold the specified number of bytes.
///
/// You can use the JRK_VAR_* constants in jrk_protocol.h to determine
/// the index and offset.
///
/// The flags parameter is the same as the flags parameter for
/// jrk_get_variables().
///
/// For a higher-level version of this that just reads all variables,
/// see jrk_get_variables().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_variable_segment(jrk_handle *,
  size_t index, size_t length, uint8_t * output, uint16_t flags);

/// Reads all of the jrk's non-volatile settings from EEPROM and returns them as
/// an object.
///
/// The settings parameter should be a non-null pointer to a jrk_settings
/// pointer, which will receive a pointer to a new settings object if and only
/// if this function is successful.  The caller must free the settings later by
/// calling jrk_settings_free().
///
/// To access fields in the settings, see the jrk_settings_* functions.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_eeprom_settings(jrk_handle *, jrk_settings ** settings);

/// Writes all of the jrk's non-volatile EEPROM settings.
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
jrk_error * jrk_set_eeprom_settings(jrk_handle *, const jrk_settings *);

/// Reads the jrk's RAM settings.
///
/// The RAM settings are a copy of the jrk's settings that is stored
/// in RAM.  These settings are initialized from EEPROM when the Jrk starts up
/// or when it receives a jrk_reinitialize() command.  Most (but not all) of of
/// the jrk's settings can be quickly and temporarily changed by writing to the
/// RAM settings instead of writing to EEPROM.
///
/// The settings parameter should be a non-null pointer to a
/// jrk_settings pointer, which will receive a pointer to a new
/// object if and only if this function is successful.  The caller must free the
/// settings later by calling jrk_settings_free().
///
/// To access fields in the settings, see the jrk_settings_*
/// functions.
///
/// For a lower-level version of this, see
/// jrk_get_ram_settings_segment().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_ram_settings(jrk_handle *, jrk_settings **);

/// Writes to all of the jrk's RAM settings.
///
/// See jrk_get_ram_settings() for information about what the
/// RAM settings are.
///
/// Internally, this function copies the settings and calls jrk_settings_fix()
/// to make sure that the settings will be valid when they are written to the
/// device.  If you want to get warnings about what was changed, you should call
/// jrk_settings_fix() yourself beforehand.
///
/// You can use this command to temporarily change settings such as PID
/// coefficients and motor limits without modifying EEPROM or reinitializing the
/// jrk.  This command takes effect immediately.  Some settings are not
/// RAM in this way, so you will have to use jrk_set_eeprom_settings() and
/// jrk_reinitialize() to change them.
///
/// Note that this command overrides *all* of the jrk's RAM settings, so it
/// needs knowledge of what those settings are.  If you are
/// writing your own USB code, you can do something simpler by just overwriting
/// the settings that you know and care about.  See the
/// function jrk_set_ram_setting_segment() which takes an arbitrary index
/// and length.
///
/// For a lower-level version of this, see
/// jrk_set_ram_settings_segment().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_set_ram_settings(jrk_handle *,
  const jrk_settings *);

/// Reads the specified bytes of the RAM settings.
///
/// See jrk_get_ram_settings() for information about what the
/// RAM settings are.
///
/// The index parameter specifies the address of the first byte to retrieve,
/// and the length parameter specifies how many bytes to retrieve.  The output
/// parameter must point to a buffer large enough for the requested length.
///
/// You can use the JRK_SETTINGS_* constants in jrk_protocol.h to calculate
/// the index and offset.
///
/// For a higher-level version of this that just reads all the RAM
/// settings, see jrk_get_ram_settings().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_ram_setting_segment(jrk_handle *,
  size_t index, size_t length, uint8_t * output);

/// Sets the specified bytes of the RAM settings.
///
/// See jrk_get_ram_settings() for information about what the
/// RAM settings are.
///
/// The index parameter specifies the address of the first byte to set,
/// and the length parameter specifies how many bytes to set.  The input
/// parameter must point to a buffer of the specified length.
///
/// You can use the JRK_SETTINGS_* constants in jrk_protocol.h to calculate
/// the index and offset.
///
/// For a higher-level version of this that just sets all the RAM
/// settings and fixes invalid settings, see jrk_set_ram_settings().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_set_ram_setting_segment(jrk_handle *,
  size_t index, size_t length, const uint8_t * input);

/// Resets the jrk's settings in RAM and EEPROM to their factory default values.
/// Part of this process is calling jrk_reinitialize_and_reset_errors().
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_restore_defaults(jrk_handle * handle);

/// Causes the Jrk to reload all of its settings from EEPROM and make them take
/// effect.  Note that this does not have an immediate effect; the effect
/// happens at the end of the PID period.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_reinitialize(jrk_handle * handle);

/// This is just like jrk_reinitialize() but it does not send the
/// "Preserve errors" flag to the Jrk.  This means that all the latched errors
/// will get cleared, and if the new input mode is serial then the
/// "Awaiting command" error bit will be set.  This is how the Reinitialize
/// command behaved on the original Jrk controllers.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_reinitialize_and_reset_errors(jrk_handle *);

/// Sends a "Start bootloader" command, which tells the Jrk to get into
/// bootloader mode.
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_start_bootloader(jrk_handle * handle);

/// \cond
JRK_API JRK_WARN_UNUSED
jrk_error * jrk_get_debug_data(jrk_handle *, uint8_t * data, size_t * size);
/// \endcond


//// Current limiting and measurment ////////////////////////////////////////////

/// Gets a list of the recommended encoded hard current limits for the specified
/// product.  They will be in ascending order by current limit in milliamps.
JRK_API
const uint16_t * jrk_get_recommended_encoded_hard_current_limits(
  uint32_t product, size_t * count);

/// Converts encoded current limit codes to milliamps for the specified product.  You can
/// use this to interpret the encoded values returned by
/// jrk_settings_get_encoded_hard_current_limit_forward() or
/// jrk_settings_get_encoded_hard_current_limit_reverse().
///
/// See also jrk_current_limit_encode().
JRK_API
uint32_t jrk_current_limit_decode(const jrk_settings *, uint16_t encoded_limit);

// Converts a current limit value in milliamps into a recommended encoded hard
// current limit value.
//
// You can use this to get the encoded values needed by
// jrk_settings_set_encoded_hard_current_limit_forward() or
// jrk_settings_set_encoded_hard_current_limit_reverse().
//
// Note that this function only returns codes that are in the recommended set, a
// subset of the codes supported by the device.
//
// The ma argument should be a current limit in milliamps.
//
// See also jrk_current_limit_decode().
JRK_API
uint16_t jrk_current_limit_encode(const jrk_settings *, uint32_t ma);

// Calculates the voltage on the current sense line in units of mV/64.
//
// To get millivolts, divide the return value by 64.
JRK_API
uint32_t jrk_calculate_raw_current_mv64(
  const jrk_settings * settings, const jrk_variables *);

/// Returns a friendly but short diagnostic sentence about the Jrk to say
/// whether the motor is running and why.
///
/// The settings and vars arguments are required, but osettings can be NULL.
/// (Currently osettings is only used to see if the PID parameters are all zero,
/// but we can fall back to just checking the persistent settings and that will
/// be good enough for almost all users.)
///
/// The flags argument should generally be set to 0.  If you want a message
/// tailored for the GUI's feedback wizard, set it to
/// JRK_DIAGNOSE_FLAG_FEEDBACK_WIZARD.
///
/// If there is an error, this returns a pointer to the error, and writes NULL
/// to the diagnosis pointer.  Otherwise, it returns a NULL error and writes a
/// pointer to a string into the diagnosis pointer.  The string is
/// null-terminated and it must be freed with jrk_string_free().
JRK_API
jrk_error * jrk_diagnose(
  const jrk_settings * settings,
  const jrk_variables * vars,
  uint32_t flags,
  char ** diagnosis);

#define JRK_DIAGNOSE_FLAG_FEEDBACK_WIZARD 1

#ifdef __cplusplus
}
#endif
