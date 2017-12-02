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
const char * jrk_look_up_product_name_short(uint8_t product);

/// Looks up a user-friendly string for the product name.  The product argument
/// should be one of the JRK_PRODUCT_* macros, but if it is not, this function
/// returns "(Unknown)".
JRK_API
const char * jrk_look_up_product_name_ui(uint8_t product);

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
void jrk_settings_set_product(jrk_settings *, uint8_t product);

/// Gets the product described in jrk_settings_set_product().
JRK_API
uint8_t jrk_settings_get_product(const jrk_settings *);



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
