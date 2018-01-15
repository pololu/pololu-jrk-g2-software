#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

static std::string convert_error_flags_to_hex_string(uint16_t flags)
{
  std::ostringstream ss;
  ss << "0x" << std::hex << std::setfill('0') << std::setw(4) << flags;
  return ss.str();
}

static std::string convert_mv_to_v_string(uint32_t mv)
{
  std::ostringstream ss;
  uint32_t dv = (mv + 50) / 100;

  ss << (dv / 10) << "." << (dv % 10) << " V";
  return ss.str();
}

static std::string convert_up_time_to_hms_string(uint32_t up_time)
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

static std::string convert_current_limit_ma_to_string(uint32_t ma)
{
  std::ostringstream ss;
  ss << (ma / 1000) << "."
    << std::setfill('0') << std::setw(3) << (ma % 1000) << " A";
  return ss.str();
}
