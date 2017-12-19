#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

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
