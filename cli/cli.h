#pragma once

#include <jrk.hpp>
#include <file_util.h>
#include <string_to_int.h>
#include <to_string.h>
#include "config.h"

#include "arg_reader.h"
#include "device_selector.h"
#include "exit_codes.h"
#include "exception_with_exit_code.h"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

void print_status(
  const jrk::variables &,
  const jrk::overridable_settings &,
  const jrk::settings &,
  const std::string & name,
  const std::string & serial_number,
  const std::string & firmware_version,
  const std::string & cmd_port,
  const std::string & ttl_port,
  bool full_output);
