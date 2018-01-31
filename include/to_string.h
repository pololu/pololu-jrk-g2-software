#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

// TODO: use this or delete it
static inline std::string convert_error_flags_to_hex_string(uint16_t flags)
{
  std::ostringstream ss;
  ss << "0x" << std::hex << std::setfill('0') << std::setw(4) << flags;
  return ss.str();
}

static inline std::string convert_mv_to_v_string(
  uint32_t mv, bool full_output = false)
{
  std::ostringstream ss;

  if (full_output)
  {
    ss << (mv / 1000) << "." << (mv / 100 % 10) << (mv / 10 % 10) << (mv % 10);
  }
  else
  {
    uint32_t dv = (mv + 50) / 100;
    ss << (dv / 10) << "." << (dv % 10);
  }
  ss << " V";
  return ss.str();
}

static inline std::string convert_up_time_to_string(uint32_t up_time)
{
  std::ostringstream ss;
  uint32_t seconds = up_time / 1000;
  uint32_t minutes = seconds / 60;
  uint16_t hours = minutes / 60;

  ss << hours <<
    ":" << std::setfill('0') << std::setw(2) << minutes % 60 <<
    ":" << std::setfill('0') << std::setw(2) << seconds % 60;
  return ss.str();
}

static inline std::string convert_current_limit_ma_to_string(uint32_t ma)
{
  std::ostringstream ss;
  uint32_t hundredths = ((ma % 1000) + 5) / 10;
  ss << (ma / 1000) << "."
     << std::setfill('0') << std::setw(2) << hundredths << " A";
  return ss.str();
}

// Converts a 12-bit analog reading between 0 and 4095 to the estimated voltage
// in volts, as a string.  We estimate VDD to be 4800 mV.
static inline std::string convert_analog_12bit_to_v_string(uint16_t reading)
{
  uint16_t mv = ((uint32_t)reading * 4800 + 2048) / 4096; // 4096 = 4.8 V
  return convert_mv_to_v_string(mv, true);
}

// Converts a 12-bit RC reading between 0 and 4095 with units of 2/3 us to the
// estimated pulse width in us, as a string.
static inline std::string convert_rc_12bit_to_us_string(uint16_t reading)
{
  uint16_t us = ((uint32_t)reading * 4 + 3) / 6;
  std::ostringstream ss;
  ss << us << " \u03BCs";
  return ss.str();
}
