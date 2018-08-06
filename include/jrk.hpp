// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file jrk.hpp
///
/// This file provides the C++ API for libpololu-jrk, a library that supports
/// communciating with the Pololu Jrk USB Stepper Motor Controller.  The classes
/// and functions here are just thin wrappers around the C API defined in jrk.h.
///
/// Note: There are some C functions in libpololu-jrk that are not wrapped here
/// because they can easily be called from C++.

#pragma once

#include "jrk.h"
#include <cstddef>
#include <utility>
#include <memory>
#include <string>
#include <vector>

// Display a nice error if C++11 is not enabled (e.g. --std=gnu++11).
#if (!defined(__cplusplus) || (__cplusplus < 201103L)) && \
    !defined(__GXX_EXPERIMENTAL_CXX0X__) && !defined(_MSC_VER)
#error This header requires features from C++11.
#endif

namespace jrk
{
  /// \cond
  inline void throw_if_needed(jrk_error * err);
  /// \endcond

  /// Wrapper for jrk_error_free().
  inline void pointer_free(jrk_error * p) noexcept
  {
    jrk_error_free(p);
  }

  /// Wrapper for jrk_error_copy().
  inline jrk_error * pointer_copy(const jrk_error * p) noexcept
  {
    return jrk_error_copy(p);
  }

  /// Wrapper for jrk_variables_free().
  inline void pointer_free(jrk_variables * p) noexcept
  {
    jrk_variables_free(p);
  }

  /// Wrapper for jrk_variables_copy().
  inline jrk_variables * pointer_copy(const jrk_variables * p)
  {
    jrk_variables * copy;
    throw_if_needed(jrk_variables_copy(p, &copy));
    return copy;
  }

  /// Wrapper for jrk_settings_free().
  inline void pointer_free(jrk_settings * p) noexcept
  {
    jrk_settings_free(p);
  }

  /// Wrapper for jrk_settings_copy().
  inline jrk_settings * pointer_copy(const jrk_settings * p)
  {
    jrk_settings * copy;
    throw_if_needed(jrk_settings_copy(p, &copy));
    return copy;
  }

  /// Wrapper for jrk_device_free().
  inline void pointer_free(jrk_device * p) noexcept
  {
    jrk_device_free(p);
  }

  /// Wrapper for jrk_device_copy().
  inline jrk_device * pointer_copy(const jrk_device * p)
  {
    jrk_device * copy;
    throw_if_needed(jrk_device_copy(p, &copy));
    return copy;
  }

  /// Wrapper for jrk_handle_close().
  inline void pointer_free(jrk_handle * p) noexcept
  {
    jrk_handle_close(p);
  }

  /// This class is not part of the public API of the library and you should
  /// not use it directly, but you can use the public methods it provides to
  /// the classes that inherit from it.
  template<class T>
  class unique_pointer_wrapper
  {
  public:
    /// Constructor that takes a pointer.
    explicit unique_pointer_wrapper(T * p = NULL) noexcept : pointer(p)
    {
    }

    /// Move constructor.
    unique_pointer_wrapper(unique_pointer_wrapper && other) noexcept
    {
      pointer = other.pointer_release();
    }

    /// Move assignment operator.
    unique_pointer_wrapper & operator=(unique_pointer_wrapper && other) noexcept
    {
      pointer_reset(other.pointer_release());
      return *this;
    }

    /// Destructor
    ~unique_pointer_wrapper() noexcept
    {
      pointer_reset();
    }

    /// Returns true if this object is wrapping something real (i.e. the
    /// underlying pointer is not NULL).
    ///
    /// We don't want to allow implicit conversion to bool because that weakens
    /// the type-checker a lot, allowing code like 'int x = some_object;' to
    /// compile.
    bool is_present() const noexcept
    {
      return pointer != NULL;
    }

    /// Returns the underlying pointer.
    T * get_pointer() const noexcept
    {
      return pointer;
    }

    /// Sets the underlying pointer to the specified value, freeing the
    /// previous pointer and taking ownership of the specified one.
    void pointer_reset(T * p = NULL) noexcept
    {
      pointer_free(pointer);
      pointer = p;
    }

    /// Releases the pointer, transferring ownership of it to the caller and
    /// resetting the underlying pointer of this object to NULL.  The caller
    /// is responsible for freeing the returned pointer if it is not NULL.
    T * pointer_release() noexcept
    {
      T * p = pointer;
      pointer = NULL;
      return p;
    }

    /// Returns a pointer to the underlying pointer.
    T ** get_pointer_to_pointer() noexcept
    {
      return &pointer;
    }

    /// Copy constructor: forbid.
    unique_pointer_wrapper(const unique_pointer_wrapper & other) = delete;

    /// Copy assignment operator: forbid.
    unique_pointer_wrapper & operator=(const unique_pointer_wrapper & other) = delete;

  protected:
    T * pointer;
  };

  /// This class is not part of the public API of the library and you should not
  /// use it directly, but you can use the public methods it provides to the
  /// classes that inherit from it.
  template <class T>
  class unique_pointer_wrapper_with_copy : public unique_pointer_wrapper<T>
  {
  public:
    /// Constructor that takes a pointer.
    explicit unique_pointer_wrapper_with_copy(T * p = NULL) noexcept
      : unique_pointer_wrapper<T>(p)
    {
    }

    /// Move constructor.
    unique_pointer_wrapper_with_copy(
      unique_pointer_wrapper_with_copy && other) = default;

    /// Copy constructor.
    unique_pointer_wrapper_with_copy(
      const unique_pointer_wrapper_with_copy & other)
      : unique_pointer_wrapper<T>()
    {
      this->pointer = pointer_copy(other.pointer);
    }

    /// Copy assignment operator.
    unique_pointer_wrapper_with_copy & operator=(
      const unique_pointer_wrapper_with_copy & other)
    {
      this->pointer_reset(pointer_copy(other.pointer));
      return *this;
    }

    /// Move assignment operator.
    unique_pointer_wrapper_with_copy & operator=(
      unique_pointer_wrapper_with_copy && other) = default;
  };

  /// Represents an error from a library call.
  class error : public unique_pointer_wrapper_with_copy<jrk_error>, public std::exception
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit error(jrk_error * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Similar to message(), but returns a C string.  The returned string will
    /// be valid at least until this object is destroyed or modified.
    virtual const char * what() const noexcept
    {
      return jrk_error_get_message(pointer);
    }

    /// Returns an English-language ASCII-encoded string describing the error.
    /// The string consists of one or more complete sentences.
    std::string message() const
    {
      return jrk_error_get_message(pointer);
    }

    /// Returns true if the error has specified error code.  The error codes are
    /// listed in the ::jrk_error_code enum.
    bool has_code(uint32_t error_code) const noexcept
    {
      return jrk_error_has_code(pointer, error_code);
    }
  };

  /// \cond
  inline void throw_if_needed(jrk_error * err)
  {
    if (err != NULL) { throw error(err); }
  }
  /// \endcond

  /// Represets the settings for a jrk.  This object just stores plain old data;
  /// it does not have any pointers or handles for other resources.
  class settings : public unique_pointer_wrapper_with_copy<jrk_settings>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit settings(jrk_settings * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    /// Wrapper for jrk_settings_create().
    static settings create()
    {
      jrk_settings * p;
      throw_if_needed(jrk_settings_create(&p));
      return settings(p);
    }

    /// Wrapper for jrk_settings_fill_with_defaults().
    void fill_with_defaults()
    {
      jrk_settings_fill_with_defaults(pointer);
    }

    /// Wrapper for jrk_settings_fix().
    ///
    /// If a non-NULL warnings pointer is provided, and this function does not
    /// throw an exception, the string it points to will be overridden with an
    /// empty string or a string with English warnings.
    void fix(std::string * warnings = NULL)
    {
      char * cstr = NULL;
      char ** cstr_pointer = warnings ? &cstr : NULL;
      throw_if_needed(jrk_settings_fix(pointer, cstr_pointer));
      if (warnings) { *warnings = std::string(cstr); }
    }

    /// Wrapper for jrk_settings_fix_and_change_product().
    void fix_and_change_product(uint32_t product, uint16_t firmware_version,
      std::string * warnings = NULL)
    {
      char * cstr = NULL;
      char ** cstr_pointer = warnings ? &cstr : NULL;
      throw_if_needed(jrk_settings_fix_and_change_product(
        pointer, product, firmware_version, cstr_pointer));
      if (warnings) { *warnings = std::string(cstr); }
    }

    /// Wrapper for jrk_settings_to_string().
    std::string to_string() const
    {
      char * str;
      throw_if_needed(jrk_settings_to_string(pointer, &str));
      std::string result(str);
      jrk_string_free(str);
      return result;
    }

    /// Wrapper for jrk_settings_read_from_string().
    static settings read_from_string(const std::string & settings_string)
    {
      settings r;
      throw_if_needed(jrk_settings_read_from_string(
          settings_string.c_str(), r.get_pointer_to_pointer()));
      return r;
    }

    /// Wrapper for jrk_settings_set_product().
    void set_product(uint32_t product) noexcept
    {
      jrk_settings_set_product(pointer, product);
    }

    /// Wrapper for jrk_settings_get_product().
    uint32_t get_product() const noexcept
    {
      return jrk_settings_get_product(pointer);
    }

    /// Wrapper for jrk_settings_set_firmware_version().
    void set_firmware_version(uint32_t version) noexcept
    {
      jrk_settings_set_firmware_version(pointer, version);
    }

    /// Wrapper for jrk_settings_get_firmware_version().
    uint32_t get_firmware_version() const noexcept
    {
      return jrk_settings_get_firmware_version(pointer);
    }

    // Beginning of auto-generated settings C++ accessors.

    /// Wrapper for jrk_settings_set_input_mode().
    void set_input_mode(uint8_t value) noexcept
    {
      jrk_settings_set_input_mode(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_mode().
    uint8_t get_input_mode() const noexcept
    {
      return jrk_settings_get_input_mode(pointer);
    }

    /// Wrapper for jrk_settings_set_input_error_minimum().
    void set_input_error_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_input_error_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_error_minimum().
    uint16_t get_input_error_minimum() const noexcept
    {
      return jrk_settings_get_input_error_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_error_maximum().
    void set_input_error_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_input_error_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_error_maximum().
    uint16_t get_input_error_maximum() const noexcept
    {
      return jrk_settings_get_input_error_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_minimum().
    void set_input_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_input_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_minimum().
    uint16_t get_input_minimum() const noexcept
    {
      return jrk_settings_get_input_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_maximum().
    void set_input_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_input_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_maximum().
    uint16_t get_input_maximum() const noexcept
    {
      return jrk_settings_get_input_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_neutral_minimum().
    void set_input_neutral_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_input_neutral_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_neutral_minimum().
    uint16_t get_input_neutral_minimum() const noexcept
    {
      return jrk_settings_get_input_neutral_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_neutral_maximum().
    void set_input_neutral_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_input_neutral_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_neutral_maximum().
    uint16_t get_input_neutral_maximum() const noexcept
    {
      return jrk_settings_get_input_neutral_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_output_minimum().
    void set_output_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_output_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_output_minimum().
    uint16_t get_output_minimum() const noexcept
    {
      return jrk_settings_get_output_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_output_neutral().
    void set_output_neutral(uint16_t value) noexcept
    {
      jrk_settings_set_output_neutral(pointer, value);
    }

    /// Wrapper for jrk_settings_get_output_neutral().
    uint16_t get_output_neutral() const noexcept
    {
      return jrk_settings_get_output_neutral(pointer);
    }

    /// Wrapper for jrk_settings_set_output_maximum().
    void set_output_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_output_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_output_maximum().
    uint16_t get_output_maximum() const noexcept
    {
      return jrk_settings_get_output_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_input_invert().
    void set_input_invert(bool value) noexcept
    {
      jrk_settings_set_input_invert(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_invert().
    bool get_input_invert() const noexcept
    {
      return jrk_settings_get_input_invert(pointer);
    }

    /// Wrapper for jrk_settings_set_input_scaling_degree().
    void set_input_scaling_degree(uint8_t value) noexcept
    {
      jrk_settings_set_input_scaling_degree(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_scaling_degree().
    uint8_t get_input_scaling_degree() const noexcept
    {
      return jrk_settings_get_input_scaling_degree(pointer);
    }

    /// Wrapper for jrk_settings_set_input_detect_disconnect().
    void set_input_detect_disconnect(bool value) noexcept
    {
      jrk_settings_set_input_detect_disconnect(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_detect_disconnect().
    bool get_input_detect_disconnect() const noexcept
    {
      return jrk_settings_get_input_detect_disconnect(pointer);
    }

    /// Wrapper for jrk_settings_set_input_analog_samples_exponent().
    void set_input_analog_samples_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_input_analog_samples_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_input_analog_samples_exponent().
    uint8_t get_input_analog_samples_exponent() const noexcept
    {
      return jrk_settings_get_input_analog_samples_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_mode().
    void set_feedback_mode(uint8_t value) noexcept
    {
      jrk_settings_set_feedback_mode(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_mode().
    uint8_t get_feedback_mode() const noexcept
    {
      return jrk_settings_get_feedback_mode(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_error_minimum().
    void set_feedback_error_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_feedback_error_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_error_minimum().
    uint16_t get_feedback_error_minimum() const noexcept
    {
      return jrk_settings_get_feedback_error_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_error_maximum().
    void set_feedback_error_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_feedback_error_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_error_maximum().
    uint16_t get_feedback_error_maximum() const noexcept
    {
      return jrk_settings_get_feedback_error_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_minimum().
    void set_feedback_minimum(uint16_t value) noexcept
    {
      jrk_settings_set_feedback_minimum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_minimum().
    uint16_t get_feedback_minimum() const noexcept
    {
      return jrk_settings_get_feedback_minimum(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_maximum().
    void set_feedback_maximum(uint16_t value) noexcept
    {
      jrk_settings_set_feedback_maximum(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_maximum().
    uint16_t get_feedback_maximum() const noexcept
    {
      return jrk_settings_get_feedback_maximum(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_invert().
    void set_feedback_invert(bool value) noexcept
    {
      jrk_settings_set_feedback_invert(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_invert().
    bool get_feedback_invert() const noexcept
    {
      return jrk_settings_get_feedback_invert(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_detect_disconnect().
    void set_feedback_detect_disconnect(bool value) noexcept
    {
      jrk_settings_set_feedback_detect_disconnect(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_detect_disconnect().
    bool get_feedback_detect_disconnect() const noexcept
    {
      return jrk_settings_get_feedback_detect_disconnect(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_dead_zone().
    void set_feedback_dead_zone(uint8_t value) noexcept
    {
      jrk_settings_set_feedback_dead_zone(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_dead_zone().
    uint8_t get_feedback_dead_zone() const noexcept
    {
      return jrk_settings_get_feedback_dead_zone(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_analog_samples_exponent().
    void set_feedback_analog_samples_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_feedback_analog_samples_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_analog_samples_exponent().
    uint8_t get_feedback_analog_samples_exponent() const noexcept
    {
      return jrk_settings_get_feedback_analog_samples_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_feedback_wraparound().
    void set_feedback_wraparound(bool value) noexcept
    {
      jrk_settings_set_feedback_wraparound(pointer, value);
    }

    /// Wrapper for jrk_settings_get_feedback_wraparound().
    bool get_feedback_wraparound() const noexcept
    {
      return jrk_settings_get_feedback_wraparound(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_mode().
    void set_serial_mode(uint8_t value) noexcept
    {
      jrk_settings_set_serial_mode(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_mode().
    uint8_t get_serial_mode() const noexcept
    {
      return jrk_settings_get_serial_mode(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_baud_rate().
    void set_serial_baud_rate(uint32_t value) noexcept
    {
      jrk_settings_set_serial_baud_rate(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_baud_rate().
    uint32_t get_serial_baud_rate() const noexcept
    {
      return jrk_settings_get_serial_baud_rate(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_timeout().
    void set_serial_timeout(uint32_t value) noexcept
    {
      jrk_settings_set_serial_timeout(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_timeout().
    uint32_t get_serial_timeout() const noexcept
    {
      return jrk_settings_get_serial_timeout(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_device_number().
    void set_serial_device_number(uint16_t value) noexcept
    {
      jrk_settings_set_serial_device_number(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_device_number().
    uint16_t get_serial_device_number() const noexcept
    {
      return jrk_settings_get_serial_device_number(pointer);
    }

    /// Wrapper for jrk_settings_set_never_sleep().
    void set_never_sleep(bool value) noexcept
    {
      jrk_settings_set_never_sleep(pointer, value);
    }

    /// Wrapper for jrk_settings_get_never_sleep().
    bool get_never_sleep() const noexcept
    {
      return jrk_settings_get_never_sleep(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_enable_crc().
    void set_serial_enable_crc(bool value) noexcept
    {
      jrk_settings_set_serial_enable_crc(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_enable_crc().
    bool get_serial_enable_crc() const noexcept
    {
      return jrk_settings_get_serial_enable_crc(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_enable_14bit_device_number().
    void set_serial_enable_14bit_device_number(bool value) noexcept
    {
      jrk_settings_set_serial_enable_14bit_device_number(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_enable_14bit_device_number().
    bool get_serial_enable_14bit_device_number() const noexcept
    {
      return jrk_settings_get_serial_enable_14bit_device_number(pointer);
    }

    /// Wrapper for jrk_settings_set_serial_disable_compact_protocol().
    void set_serial_disable_compact_protocol(bool value) noexcept
    {
      jrk_settings_set_serial_disable_compact_protocol(pointer, value);
    }

    /// Wrapper for jrk_settings_get_serial_disable_compact_protocol().
    bool get_serial_disable_compact_protocol() const noexcept
    {
      return jrk_settings_get_serial_disable_compact_protocol(pointer);
    }

    /// Wrapper for jrk_settings_set_proportional_multiplier().
    void set_proportional_multiplier(uint16_t value) noexcept
    {
      jrk_settings_set_proportional_multiplier(pointer, value);
    }

    /// Wrapper for jrk_settings_get_proportional_multiplier().
    uint16_t get_proportional_multiplier() const noexcept
    {
      return jrk_settings_get_proportional_multiplier(pointer);
    }

    /// Wrapper for jrk_settings_set_proportional_exponent().
    void set_proportional_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_proportional_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_proportional_exponent().
    uint8_t get_proportional_exponent() const noexcept
    {
      return jrk_settings_get_proportional_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_integral_multiplier().
    void set_integral_multiplier(uint16_t value) noexcept
    {
      jrk_settings_set_integral_multiplier(pointer, value);
    }

    /// Wrapper for jrk_settings_get_integral_multiplier().
    uint16_t get_integral_multiplier() const noexcept
    {
      return jrk_settings_get_integral_multiplier(pointer);
    }

    /// Wrapper for jrk_settings_set_integral_exponent().
    void set_integral_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_integral_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_integral_exponent().
    uint8_t get_integral_exponent() const noexcept
    {
      return jrk_settings_get_integral_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_derivative_multiplier().
    void set_derivative_multiplier(uint16_t value) noexcept
    {
      jrk_settings_set_derivative_multiplier(pointer, value);
    }

    /// Wrapper for jrk_settings_get_derivative_multiplier().
    uint16_t get_derivative_multiplier() const noexcept
    {
      return jrk_settings_get_derivative_multiplier(pointer);
    }

    /// Wrapper for jrk_settings_set_derivative_exponent().
    void set_derivative_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_derivative_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_derivative_exponent().
    uint8_t get_derivative_exponent() const noexcept
    {
      return jrk_settings_get_derivative_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_pid_period().
    void set_pid_period(uint16_t value) noexcept
    {
      jrk_settings_set_pid_period(pointer, value);
    }

    /// Wrapper for jrk_settings_get_pid_period().
    uint16_t get_pid_period() const noexcept
    {
      return jrk_settings_get_pid_period(pointer);
    }

    /// Wrapper for jrk_settings_set_integral_divider_exponent().
    void set_integral_divider_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_integral_divider_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_integral_divider_exponent().
    uint8_t get_integral_divider_exponent() const noexcept
    {
      return jrk_settings_get_integral_divider_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_integral_limit().
    void set_integral_limit(uint16_t value) noexcept
    {
      jrk_settings_set_integral_limit(pointer, value);
    }

    /// Wrapper for jrk_settings_get_integral_limit().
    uint16_t get_integral_limit() const noexcept
    {
      return jrk_settings_get_integral_limit(pointer);
    }

    /// Wrapper for jrk_settings_set_reset_integral().
    void set_reset_integral(bool value) noexcept
    {
      jrk_settings_set_reset_integral(pointer, value);
    }

    /// Wrapper for jrk_settings_get_reset_integral().
    bool get_reset_integral() const noexcept
    {
      return jrk_settings_get_reset_integral(pointer);
    }

    /// Wrapper for jrk_settings_set_pwm_frequency().
    void set_pwm_frequency(uint8_t value) noexcept
    {
      jrk_settings_set_pwm_frequency(pointer, value);
    }

    /// Wrapper for jrk_settings_get_pwm_frequency().
    uint8_t get_pwm_frequency() const noexcept
    {
      return jrk_settings_get_pwm_frequency(pointer);
    }

    /// Wrapper for jrk_settings_set_current_samples_exponent().
    void set_current_samples_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_current_samples_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_current_samples_exponent().
    uint8_t get_current_samples_exponent() const noexcept
    {
      return jrk_settings_get_current_samples_exponent(pointer);
    }

    /// Wrapper for jrk_settings_set_hard_overcurrent_threshold().
    void set_hard_overcurrent_threshold(uint8_t value) noexcept
    {
      jrk_settings_set_hard_overcurrent_threshold(pointer, value);
    }

    /// Wrapper for jrk_settings_get_hard_overcurrent_threshold().
    uint8_t get_hard_overcurrent_threshold() const noexcept
    {
      return jrk_settings_get_hard_overcurrent_threshold(pointer);
    }

    /// Wrapper for jrk_settings_set_current_offset_calibration().
    void set_current_offset_calibration(int16_t value) noexcept
    {
      jrk_settings_set_current_offset_calibration(pointer, value);
    }

    /// Wrapper for jrk_settings_get_current_offset_calibration().
    int16_t get_current_offset_calibration() const noexcept
    {
      return jrk_settings_get_current_offset_calibration(pointer);
    }

    /// Wrapper for jrk_settings_set_current_scale_calibration().
    void set_current_scale_calibration(int16_t value) noexcept
    {
      jrk_settings_set_current_scale_calibration(pointer, value);
    }

    /// Wrapper for jrk_settings_get_current_scale_calibration().
    int16_t get_current_scale_calibration() const noexcept
    {
      return jrk_settings_get_current_scale_calibration(pointer);
    }

    /// Wrapper for jrk_settings_set_motor_invert().
    void set_motor_invert(bool value) noexcept
    {
      jrk_settings_set_motor_invert(pointer, value);
    }

    /// Wrapper for jrk_settings_get_motor_invert().
    bool get_motor_invert() const noexcept
    {
      return jrk_settings_get_motor_invert(pointer);
    }

    /// Wrapper for jrk_settings_set_max_duty_cycle_while_feedback_out_of_range().
    void set_max_duty_cycle_while_feedback_out_of_range(uint16_t value) noexcept
    {
      jrk_settings_set_max_duty_cycle_while_feedback_out_of_range(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_duty_cycle_while_feedback_out_of_range().
    uint16_t get_max_duty_cycle_while_feedback_out_of_range() const noexcept
    {
      return jrk_settings_get_max_duty_cycle_while_feedback_out_of_range(pointer);
    }

    /// Wrapper for jrk_settings_set_max_acceleration_forward().
    void set_max_acceleration_forward(uint16_t value) noexcept
    {
      jrk_settings_set_max_acceleration_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_acceleration_forward().
    uint16_t get_max_acceleration_forward() const noexcept
    {
      return jrk_settings_get_max_acceleration_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_max_acceleration_reverse().
    void set_max_acceleration_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_max_acceleration_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_acceleration_reverse().
    uint16_t get_max_acceleration_reverse() const noexcept
    {
      return jrk_settings_get_max_acceleration_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_max_deceleration_forward().
    void set_max_deceleration_forward(uint16_t value) noexcept
    {
      jrk_settings_set_max_deceleration_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_deceleration_forward().
    uint16_t get_max_deceleration_forward() const noexcept
    {
      return jrk_settings_get_max_deceleration_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_max_deceleration_reverse().
    void set_max_deceleration_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_max_deceleration_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_deceleration_reverse().
    uint16_t get_max_deceleration_reverse() const noexcept
    {
      return jrk_settings_get_max_deceleration_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_max_duty_cycle_forward().
    void set_max_duty_cycle_forward(uint16_t value) noexcept
    {
      jrk_settings_set_max_duty_cycle_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_duty_cycle_forward().
    uint16_t get_max_duty_cycle_forward() const noexcept
    {
      return jrk_settings_get_max_duty_cycle_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_max_duty_cycle_reverse().
    void set_max_duty_cycle_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_max_duty_cycle_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_max_duty_cycle_reverse().
    uint16_t get_max_duty_cycle_reverse() const noexcept
    {
      return jrk_settings_get_max_duty_cycle_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_encoded_hard_current_limit_forward().
    void set_encoded_hard_current_limit_forward(uint16_t value) noexcept
    {
      jrk_settings_set_encoded_hard_current_limit_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_encoded_hard_current_limit_forward().
    uint16_t get_encoded_hard_current_limit_forward() const noexcept
    {
      return jrk_settings_get_encoded_hard_current_limit_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_encoded_hard_current_limit_reverse().
    void set_encoded_hard_current_limit_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_encoded_hard_current_limit_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_encoded_hard_current_limit_reverse().
    uint16_t get_encoded_hard_current_limit_reverse() const noexcept
    {
      return jrk_settings_get_encoded_hard_current_limit_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_brake_duration_forward().
    void set_brake_duration_forward(uint32_t value) noexcept
    {
      jrk_settings_set_brake_duration_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_brake_duration_forward().
    uint32_t get_brake_duration_forward() const noexcept
    {
      return jrk_settings_get_brake_duration_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_brake_duration_reverse().
    void set_brake_duration_reverse(uint32_t value) noexcept
    {
      jrk_settings_set_brake_duration_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_brake_duration_reverse().
    uint32_t get_brake_duration_reverse() const noexcept
    {
      return jrk_settings_get_brake_duration_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_soft_current_limit_forward().
    void set_soft_current_limit_forward(uint16_t value) noexcept
    {
      jrk_settings_set_soft_current_limit_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_soft_current_limit_forward().
    uint16_t get_soft_current_limit_forward() const noexcept
    {
      return jrk_settings_get_soft_current_limit_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_soft_current_limit_reverse().
    void set_soft_current_limit_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_soft_current_limit_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_soft_current_limit_reverse().
    uint16_t get_soft_current_limit_reverse() const noexcept
    {
      return jrk_settings_get_soft_current_limit_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_soft_current_regulation_level_forward().
    void set_soft_current_regulation_level_forward(uint16_t value) noexcept
    {
      jrk_settings_set_soft_current_regulation_level_forward(pointer, value);
    }

    /// Wrapper for jrk_settings_get_soft_current_regulation_level_forward().
    uint16_t get_soft_current_regulation_level_forward() const noexcept
    {
      return jrk_settings_get_soft_current_regulation_level_forward(pointer);
    }

    /// Wrapper for jrk_settings_set_soft_current_regulation_level_reverse().
    void set_soft_current_regulation_level_reverse(uint16_t value) noexcept
    {
      jrk_settings_set_soft_current_regulation_level_reverse(pointer, value);
    }

    /// Wrapper for jrk_settings_get_soft_current_regulation_level_reverse().
    uint16_t get_soft_current_regulation_level_reverse() const noexcept
    {
      return jrk_settings_get_soft_current_regulation_level_reverse(pointer);
    }

    /// Wrapper for jrk_settings_set_coast_when_off().
    void set_coast_when_off(bool value) noexcept
    {
      jrk_settings_set_coast_when_off(pointer, value);
    }

    /// Wrapper for jrk_settings_get_coast_when_off().
    bool get_coast_when_off() const noexcept
    {
      return jrk_settings_get_coast_when_off(pointer);
    }

    /// Wrapper for jrk_settings_set_error_enable().
    void set_error_enable(uint16_t value) noexcept
    {
      jrk_settings_set_error_enable(pointer, value);
    }

    /// Wrapper for jrk_settings_get_error_enable().
    uint16_t get_error_enable() const noexcept
    {
      return jrk_settings_get_error_enable(pointer);
    }

    /// Wrapper for jrk_settings_set_error_latch().
    void set_error_latch(uint16_t value) noexcept
    {
      jrk_settings_set_error_latch(pointer, value);
    }

    /// Wrapper for jrk_settings_get_error_latch().
    uint16_t get_error_latch() const noexcept
    {
      return jrk_settings_get_error_latch(pointer);
    }

    /// Wrapper for jrk_settings_set_error_hard().
    void set_error_hard(uint16_t value) noexcept
    {
      jrk_settings_set_error_hard(pointer, value);
    }

    /// Wrapper for jrk_settings_get_error_hard().
    uint16_t get_error_hard() const noexcept
    {
      return jrk_settings_get_error_hard(pointer);
    }

    /// Wrapper for jrk_settings_set_vin_calibration().
    void set_vin_calibration(int16_t value) noexcept
    {
      jrk_settings_set_vin_calibration(pointer, value);
    }

    /// Wrapper for jrk_settings_get_vin_calibration().
    int16_t get_vin_calibration() const noexcept
    {
      return jrk_settings_get_vin_calibration(pointer);
    }

    /// Wrapper for jrk_settings_set_disable_i2c_pullups().
    void set_disable_i2c_pullups(bool value) noexcept
    {
      jrk_settings_set_disable_i2c_pullups(pointer, value);
    }

    /// Wrapper for jrk_settings_get_disable_i2c_pullups().
    bool get_disable_i2c_pullups() const noexcept
    {
      return jrk_settings_get_disable_i2c_pullups(pointer);
    }

    /// Wrapper for jrk_settings_set_analog_sda_pullup().
    void set_analog_sda_pullup(bool value) noexcept
    {
      jrk_settings_set_analog_sda_pullup(pointer, value);
    }

    /// Wrapper for jrk_settings_get_analog_sda_pullup().
    bool get_analog_sda_pullup() const noexcept
    {
      return jrk_settings_get_analog_sda_pullup(pointer);
    }

    /// Wrapper for jrk_settings_set_always_analog_sda().
    void set_always_analog_sda(bool value) noexcept
    {
      jrk_settings_set_always_analog_sda(pointer, value);
    }

    /// Wrapper for jrk_settings_get_always_analog_sda().
    bool get_always_analog_sda() const noexcept
    {
      return jrk_settings_get_always_analog_sda(pointer);
    }

    /// Wrapper for jrk_settings_set_always_analog_fba().
    void set_always_analog_fba(bool value) noexcept
    {
      jrk_settings_set_always_analog_fba(pointer, value);
    }

    /// Wrapper for jrk_settings_get_always_analog_fba().
    bool get_always_analog_fba() const noexcept
    {
      return jrk_settings_get_always_analog_fba(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_method().
    void set_fbt_method(uint8_t value) noexcept
    {
      jrk_settings_set_fbt_method(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_method().
    uint8_t get_fbt_method() const noexcept
    {
      return jrk_settings_get_fbt_method(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_timing_clock().
    void set_fbt_timing_clock(uint8_t value) noexcept
    {
      jrk_settings_set_fbt_timing_clock(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_timing_clock().
    uint8_t get_fbt_timing_clock() const noexcept
    {
      return jrk_settings_get_fbt_timing_clock(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_timing_polarity().
    void set_fbt_timing_polarity(bool value) noexcept
    {
      jrk_settings_set_fbt_timing_polarity(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_timing_polarity().
    bool get_fbt_timing_polarity() const noexcept
    {
      return jrk_settings_get_fbt_timing_polarity(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_timing_timeout().
    void set_fbt_timing_timeout(uint16_t value) noexcept
    {
      jrk_settings_set_fbt_timing_timeout(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_timing_timeout().
    uint16_t get_fbt_timing_timeout() const noexcept
    {
      return jrk_settings_get_fbt_timing_timeout(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_samples().
    void set_fbt_samples(uint8_t value) noexcept
    {
      jrk_settings_set_fbt_samples(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_samples().
    uint8_t get_fbt_samples() const noexcept
    {
      return jrk_settings_get_fbt_samples(pointer);
    }

    /// Wrapper for jrk_settings_set_fbt_divider_exponent().
    void set_fbt_divider_exponent(uint8_t value) noexcept
    {
      jrk_settings_set_fbt_divider_exponent(pointer, value);
    }

    /// Wrapper for jrk_settings_get_fbt_divider_exponent().
    uint8_t get_fbt_divider_exponent() const noexcept
    {
      return jrk_settings_get_fbt_divider_exponent(pointer);
    }

    // End of auto-generated settings C++ accessors.
  };

  /// Represents the variables read from a jrk.  This object just stores plain
  /// old data; it does not have any pointer or handles for other resources.
  class variables : public unique_pointer_wrapper_with_copy<jrk_variables>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit variables(jrk_variables * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    // Beginning of auto-generated variables C++ getters.

    /// Wrapper for jrk_variables_get_input().
    uint16_t get_input() const noexcept
    {
      return jrk_variables_get_input(pointer);
    }

    /// Wrapper for jrk_variables_get_target().
    uint16_t get_target() const noexcept
    {
      return jrk_variables_get_target(pointer);
    }

    /// Wrapper for jrk_variables_get_feedback().
    uint16_t get_feedback() const noexcept
    {
      return jrk_variables_get_feedback(pointer);
    }

    /// Wrapper for jrk_variables_get_scaled_feedback().
    uint16_t get_scaled_feedback() const noexcept
    {
      return jrk_variables_get_scaled_feedback(pointer);
    }

    /// Wrapper for jrk_variables_get_integral().
    int16_t get_integral() const noexcept
    {
      return jrk_variables_get_integral(pointer);
    }

    /// Wrapper for jrk_variables_get_duty_cycle_target().
    int16_t get_duty_cycle_target() const noexcept
    {
      return jrk_variables_get_duty_cycle_target(pointer);
    }

    /// Wrapper for jrk_variables_get_duty_cycle().
    int16_t get_duty_cycle() const noexcept
    {
      return jrk_variables_get_duty_cycle(pointer);
    }

    /// Wrapper for jrk_variables_get_current_low_res().
    uint8_t get_current_low_res() const noexcept
    {
      return jrk_variables_get_current_low_res(pointer);
    }

    /// Wrapper for jrk_variables_get_pid_period_exceeded().
    bool get_pid_period_exceeded() const noexcept
    {
      return jrk_variables_get_pid_period_exceeded(pointer);
    }

    /// Wrapper for jrk_variables_get_pid_period_count().
    uint16_t get_pid_period_count() const noexcept
    {
      return jrk_variables_get_pid_period_count(pointer);
    }

    /// Wrapper for jrk_variables_get_error_flags_halting().
    uint16_t get_error_flags_halting() const noexcept
    {
      return jrk_variables_get_error_flags_halting(pointer);
    }

    /// Wrapper for jrk_variables_get_error_flags_occurred().
    uint16_t get_error_flags_occurred() const noexcept
    {
      return jrk_variables_get_error_flags_occurred(pointer);
    }

    /// Wrapper for jrk_variables_get_vin_voltage().
    uint16_t get_vin_voltage() const noexcept
    {
      return jrk_variables_get_vin_voltage(pointer);
    }

    /// Wrapper for jrk_variables_get_current().
    uint16_t get_current() const noexcept
    {
      return jrk_variables_get_current(pointer);
    }

    /// Wrapper for jrk_variables_get_device_reset().
    uint8_t get_device_reset() const noexcept
    {
      return jrk_variables_get_device_reset(pointer);
    }

    /// Wrapper for jrk_variables_get_up_time().
    uint32_t get_up_time() const noexcept
    {
      return jrk_variables_get_up_time(pointer);
    }

    /// Wrapper for jrk_variables_get_rc_pulse_width().
    uint16_t get_rc_pulse_width() const noexcept
    {
      return jrk_variables_get_rc_pulse_width(pointer);
    }

    /// Wrapper for jrk_variables_get_fbt_reading().
    uint16_t get_fbt_reading() const noexcept
    {
      return jrk_variables_get_fbt_reading(pointer);
    }

    /// Wrapper for jrk_variables_get_raw_current().
    uint16_t get_raw_current() const noexcept
    {
      return jrk_variables_get_raw_current(pointer);
    }

    /// Wrapper for jrk_variables_get_encoded_hard_current_limit().
    uint16_t get_encoded_hard_current_limit() const noexcept
    {
      return jrk_variables_get_encoded_hard_current_limit(pointer);
    }

    /// Wrapper for jrk_variables_get_last_duty_cycle().
    int16_t get_last_duty_cycle() const noexcept
    {
      return jrk_variables_get_last_duty_cycle(pointer);
    }

    /// Wrapper for jrk_variables_get_current_chopping_consecutive_count().
    uint8_t get_current_chopping_consecutive_count() const noexcept
    {
      return jrk_variables_get_current_chopping_consecutive_count(pointer);
    }

    /// Wrapper for jrk_variables_get_current_chopping_occurrence_count().
    uint8_t get_current_chopping_occurrence_count() const noexcept
    {
      return jrk_variables_get_current_chopping_occurrence_count(pointer);
    }

    // End of auto-generated variables C++ getters.

    /// Wrapper for jrk_variables_get_error().
    int16_t get_error() const noexcept
    {
      return jrk_variables_get_error(pointer);
    }

    /// Wrapper for jrk_variables_get_force_mode().
    uint8_t get_force_mode() const noexcept
    {
      return jrk_variables_get_force_mode(pointer);
    }

    /// Wrapper for jrk_variables_get_analog_reading().
    uint16_t get_analog_reading(uint8_t pin) const noexcept
    {
      return jrk_variables_get_analog_reading(pointer, pin);
    }

    /// Wrapper for jrk_variables_get_digital_reading().
    bool get_digital_reading(uint8_t pin) const noexcept
    {
      return jrk_variables_get_digital_reading(pointer, pin);
    }
  };

  /// Represents a jrk that is or was connected to the computer.  Can also be in
  /// a null state where it does not represent a device.
  class device : public unique_pointer_wrapper_with_copy<jrk_device>
  {
  public:
    /// Constructor that takes a pointer from the C API.
    explicit device(jrk_device * p = NULL) noexcept :
      unique_pointer_wrapper_with_copy(p)
    {
    }

    uint32_t get_product() const noexcept
    {
      return jrk_device_get_product(pointer);
    }

    /// Gets the serial number of the device as an ASCII-encoded string.
    /// If the device is null, returns an empty string.
    std::string get_serial_number() const noexcept
    {
      return jrk_device_get_serial_number(pointer);
    }

    /// Get an operating system-specific identifier for this device as an
    /// ASCII-encoded string.  The string will be valid at least as long as the
    /// device object.  If the device is NULL, returns an empty string.
    std::string get_os_id() const noexcept
    {
      return jrk_device_get_os_id(pointer);
    }

    /// Gets the firmware version as a binary-coded decimal number (e.g. 0x0100
    /// means "1.00").  We recommend using
    /// jrk::handle::get_firmware_version_string() instead of this function if
    /// possible.
    uint16_t get_firmware_version() const noexcept
    {
      return jrk_device_get_firmware_version(pointer);
    }

    /// Wrapper for jrk_device_get_cmd_port_name().
    std::string get_cmd_port_name() const
    {
      char * str;
      throw_if_needed(jrk_device_get_cmd_port_name(pointer, &str));
      std::string result = std::string(str);
      jrk_string_free(str);
      return result;
    }

    /// Wrapper for jrk_device_get_ttl_port_name().
    std::string get_ttl_port_name() const
    {
      char * str;
      throw_if_needed(jrk_device_get_ttl_port_name(pointer, &str));
      std::string result = std::string(str);
      jrk_string_free(str);
      return result;
    }
  };

  /// Finds all the supported jrk devices connected to the computer via USB and
  /// returns a list of them.
  inline std::vector<jrk::device> list_connected_devices()
  {
    jrk_device ** device_list;
    size_t size;
    throw_if_needed(jrk_list_connected_devices(&device_list, &size));
    std::vector<device> vector;
    for (size_t i = 0; i < size; i++)
    {
      vector.push_back(device(device_list[i]));
    }
    jrk_list_free(device_list);
    return vector;
  }

  /// Represents an open handle that can be used to read and write data from a
  /// device.  Can also be in a null state where it does not represent a device.
  class handle : public unique_pointer_wrapper<jrk_handle>
  {
  public:
    /// Constructor that takes a pointer from the C API.  This object will free
    /// the pointer when it is destroyed.
    explicit handle(jrk_handle * p = NULL) noexcept
      : unique_pointer_wrapper(p)
    {
    }

    /// Constructor that opens a handle to the specified device.
    explicit handle(const device & device)
    {
      throw_if_needed(jrk_handle_open(device.get_pointer(), &pointer));
    }

    /// Closes the handle and puts this object into the null state.
    void close() noexcept
    {
      pointer_reset();
    }

    /// Wrapper for jrk_handle_get_device();
    device get_device() const
    {
      return device(pointer_copy(jrk_handle_get_device(pointer)));
    }

    /// Wrapper for jrk_get_firmware_version_string().
    std::string get_firmware_version_string()
    {
      return jrk_get_firmware_version_string(pointer);
    }

    /// Wrapper for jrk_set_target().
    void set_target(uint16_t target)
    {
      throw_if_needed(jrk_set_target(pointer, target));
    }

    /// Wrapper for jrk_stop_motor().
    void stop_motor()
    {
      throw_if_needed(jrk_stop_motor(pointer));
    }

    /// Wrapper for jrk_run_motor().
    void run_motor()
    {
      throw_if_needed(jrk_run_motor(pointer));
    }

    /// Wrapper for jrk_clear_errors().
    uint16_t clear_errors()
    {
      uint16_t error_flags;
      throw_if_needed(jrk_clear_errors(pointer, &error_flags));
      return error_flags;
    }

    /// Wrapper for jrk_force_duty_cycle_target().
    void force_duty_cycle_target(int16_t duty_cycle)
    {
      throw_if_needed(jrk_force_duty_cycle_target(pointer, duty_cycle));
    }

    /// Wrapper for jrk_force_duty_cycle().
    void force_duty_cycle(int16_t duty_cycle)
    {
      throw_if_needed(jrk_force_duty_cycle(pointer, duty_cycle));
    }

    /// Wrapper for jrk_get_variables().
    variables get_variables(uint16_t flags)
    {
      jrk_variables * v;
      throw_if_needed(jrk_get_variables(pointer, &v, flags));
      return variables(v);
    }

    /// Wrapper for jrk_get_variable_segment().
    void get_variable_segment(size_t index, size_t length,
      uint8_t * output, uint16_t flags)
    {
      throw_if_needed(jrk_get_variable_segment(
          pointer, index, length, output, flags));
    }

    /// Wrapper for jrk_get_eeprom_settings().
    settings get_eeprom_settings()
    {
      jrk_settings * s;
      throw_if_needed(jrk_get_eeprom_settings(pointer, &s));
      return settings(s);
    }

    /// Wrapper for jrk_set_eeprom_settings().
    void set_eeprom_settings(const settings & settings)
    {
      throw_if_needed(jrk_set_eeprom_settings(pointer, settings.get_pointer()));
    }

    /// Wrapper for jrk_get_ram_settings().
    settings get_ram_settings()
    {
      jrk_settings * s;
      throw_if_needed(jrk_get_ram_settings(pointer, &s));
      return settings(s);
    }

    /// Wrapper for jrk_set_ram_settings().
    void set_ram_settings(const settings & s)
    {
      throw_if_needed(jrk_set_ram_settings(pointer, s.get_pointer()));
    }

    /// Wrapper for jrk_get_ram_setting_segment().
    void get_ram_setting_segment(size_t index, size_t length,
      uint8_t * output)
    {
      throw_if_needed(jrk_get_ram_setting_segment(
          pointer, index, length, output));
    }

    /// Wrapper for jrk_set_ram_setting_segment().
    void set_ram_setting_segment(size_t index, size_t length,
      const uint8_t * input)
    {
      throw_if_needed(jrk_set_ram_setting_segment(
          pointer, index, length, input));
    }

    /// Wrapper for jrk_restore_defaults().
    void restore_defaults()
    {
      throw_if_needed(jrk_restore_defaults(pointer));
    }

    /// Wrapper for jrk_reinitialize().
    void reinitialize()
    {
      throw_if_needed(jrk_reinitialize(pointer));
    }

    /// Wrapper for jrk_reinitialize_and_reset_errors().
    void reinitialize_and_reset_errors()
    {
      throw_if_needed(jrk_reinitialize_and_reset_errors(pointer));
    }

    /// Wrapperfor jrk_start_bootloader()
    void start_bootloader()
    {
      throw_if_needed(jrk_start_bootloader(pointer));
    }

    /// \cond
    void get_debug_data(std::vector<uint8_t> & data)
    {
      size_t size = data.size();
      throw_if_needed(jrk_get_debug_data(pointer, data.data(), &size));
      data.resize(size);
    }
    /// \endcond
  };

  /// Wrapper for jrk_get_recommended_encoded_hard_current_limits().
  inline const std::vector<uint16_t> get_recommended_encoded_hard_current_limits(
    uint32_t product)
  {
    size_t count;
    const uint16_t * table = jrk_get_recommended_encoded_hard_current_limits(product, &count);
    return std::vector<uint16_t>(table, table + count);
  }

  /// Wrapper for jrk_current_limit_decode().
  inline uint32_t current_limit_decode(const settings & settings, uint16_t code)
  {
    return jrk_current_limit_decode(settings.get_pointer(), code);
  }

  /// Wrapper for jrk_current_limit_ma_to_code().
  inline uint16_t current_limit_encode(const settings & settings, uint32_t ma)
  {
    return jrk_current_limit_encode(settings.get_pointer(), ma);
  }

  /// Wrapper for jrk_calculate_raw_current_mv64().
  inline uint32_t calculate_raw_current_mv64(
    const settings & settings, const variables & vars)
  {
    return jrk_calculate_raw_current_mv64(
      settings.get_pointer(), vars.get_pointer());
  }

  /// Wrapper for jrk_diagnose().
  inline std::string diagnose(
    const settings & settings,
    const variables & vars,
    uint32_t flags = 0)
  {
    char * cstr;
    throw_if_needed(jrk_diagnose(
        settings.get_pointer(), vars.get_pointer(), flags, &cstr));
    std::string diagnosis(cstr);
    jrk_string_free(cstr);
    return diagnosis;
  }
}

