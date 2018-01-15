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
