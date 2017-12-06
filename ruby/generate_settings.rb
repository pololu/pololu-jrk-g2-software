Settings = [
  {
    name: 'input_mode',
    type: :enum,
    max: 'JRK_INPUT_MODE_PULSE_WIDTH',
    default: 'JRK_INPUT_MODE_SERIAL',
    default_is_zero: true,
    english_default: 'serial',
  },
  {
    name: 'input_disconnect_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'input_disconnect_maximum',
    type: :uint16_t,
    range: 0..4095,
    default: 4095,
  },
  {
    name: 'input_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'input_maximum',
    type: :uint16_t,
    range: 0..4095,
    default: 4095,
  },
  {
    name: 'input_neutral_minimum',
    type: :uint16_t,
    range: 0..4095,
    default: 2048,
  },
  {
    name: 'input_neutral_maximum',
    type: :uint16_t,
    range: 0..4095,
    default: 2048,
  },
  {
    name: 'output_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'output_neutral',
    type: :uint16_t,
    range: 0..4095,
    default: 2048,
  },
  {
    name: 'output_maximum',
    type: :uint16_t,
    range: 0..4095,
    default: 4095,
  },
  {
    name: 'input_invert',
    type: :bool,
  },
  {
    name: 'input_scaling_degree',
    type: :enum,
    default: 'JRK_SCALING_DEGREE_LINEAR',
    default_is_zero: true,
    english_default: 'linear',
    max: 'JRK_SCALING_DEGREE_QUINTIC',
  },
  {
    name: 'input_power_with_aux',
    type: :bool,
  },
  {
    name: 'input_analog_samples_exponent',
    type: :uint8_t,
    range: 0..8,
    comment:
      "This setting specifies how many analog samples to take if the input mode\n" \
      "is analog.  The number of samples will be 4*(2^x), where x is this setting.",
    default: 5,
  },
  {
    name: 'feedback_mode',
    type: :enum,
    default: 'JRK_FEEDBACK_MODE_ANALOG',
    english_default: 'analog',
    max: 'JRK_FEEDBACK_MODE_FREQUENCY',
  },
  {
    name: 'feedback_disconnect_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'feedback_disconnect_maximum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'feedback_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'feedback_maximum',
    type: :uint16_t,
    range: 0..4095,
  },
  {
    name: 'feedback_invert',
    type: :bool,
  },
  {
    name: 'feedback_power_with_aux',
    type: :bool,
  },
  {
    name: 'feedback_dead_zone',
    type: :uint8_t,
  },
  {
    name: 'feedback_analog_samples_exponent',
    type: :uint8_t,
    range: 0..8,
    comment:
      "This setting specifies how many analog samples to take if the feedback mode\n" \
      "is analog.  The number of samples will be 4*(2^x), where x is this setting.",
    default: 5,
  },
  {
    name: 'feedback_wraparound',
    type: :bool,
  },
  {
    name: 'serial_mode',
    type: :enum,
    default: 'JRK_SERIAL_MODE_USB_DUAL_PORT',
    default_is_zero: true,
    english_default: 'USB dual port',
    max: 'JRK_SERIAL_MODE_UART',
  },
  {
    name: 'serial_baud_rate',
    type: :uint32_t,
    custom_fix: true,
    custom_eeprom: true,
    comment:
      "This setting specifies the baud rate to use on the RX and TX pins\n" \
      "when the serial mode is UART.  It should be between\n" \
      "JRK_MIN_ALLOWED_BAUD_RATE and JRK_MAX_ALLOWED_BAUD_RATE.",
  },
  {
    name: 'serial_timeout',
    type: :uint16_t,
    comment:
      "This is the time in units of 10 milliseconds before the device considers\n" \
      "it to be an error if it has not received certain commands.  A value of 0\n" \
      "disables the command timeout feature.",
  },
  {
    name: 'serial_enable_crc',
    type: :bool,
  },
  {
    name: 'serial_device_number',
    type: :uint8_t,
    max: 127,
    default: 11,
  },
  {
    name: 'never_sleep',
    type: :bool,
  },
  {
    name: 'proportional_multiplier',
    type: :uint16_t,
    max: 1023,
  },
  {
    name: 'proportional_exponent',
    type: :uint8_t,
    max: 15,
  },
  {
    name: 'integral_multiplier',
    type: :uint16_t,
    max: 1023,
  },
  {
    name: 'integral_exponent',
    type: :uint8_t,
    max: 15,
  },
  {
    name: 'derivative_multiplier',
    type: :uint16_t,
    max: 1023,
  },
  {
    name: 'derivative_exponent',
    type: :uint8_t,
    max: 15,
  },
  {
    name: 'pid_period',
    type: :uint16_t,
    range: 1..8191,
    default: 10,
  },
  {
    name: 'pid_integral_limit',
    type: :uint16_t,
    default: 1000,
  },
  {
    name: 'pid_reset_integral',
    type: :bool,
  },
  {
    name: 'motor_pwm_frequency',
    type: :enum,
    default: 'JRK_MOTOR_PWM_FREQUENCY_20',
    default_is_zero: true,
    english_default: '20 kHz',
    max: 'JRK_MOTOR_PWM_FREQUENCY_5',
  },
  {
    name: 'motor_invert',
    type: :bool,
  },
  {
    name: 'motor_max_duty_cycle_while_feedback_out_of_range',
    type: :uint16_t,
    range: 1..600,
    default: 600,
  },
  {
    name: 'motor_max_acceleration_forward',
    type: :uint16_t,
    range: 1..600,
    default: 600,
  },
  {
    name: 'motor_max_acceleration_reverse',
    type: :uint16_t,
    range: 1..600,
    default: 600,
  },
  {
    name: 'motor_max_deceleration_forward',
    type: :uint16_t,
    range: 1..600,
    default: 600,
  },
  {
    name: 'motor_max_deceleration_reverse',
    type: :uint16_t,
    range: 1..600,
    default: 600,
  },
  {
    name: 'motor_max_duty_cycle_forward',
    type: :uint16_t,
    max: 600,
    default: 600,
  },
  {
    name: 'motor_max_duty_cycle_reverse',
    type: :uint16_t,
    max: 600,
    default: 600,
  },
  {
    name: 'motor_max_current_forward',
    type: :uint8_t,
    default: 81, # TODO: rethink this default
  },
  {
    name: 'motor_max_current_reverse',
    type: :uint8_t,
    default: 81, # TODO: rethink this default
  },
  {
    name: 'motor_current_calibration_forward',
    type: :int8_t,
  },
  {
    name: 'motor_current_calibration_reverse',
    type: :int8_t,
  },
  {
    name: 'motor_brake_duration_forward',
    type: :uint8_t,
    comment: 'Units of 5 ms.',
  },
  {
    name: 'motor_brake_duration_reverse',
    type: :uint8_t,
    comment: 'Units of 5 ms.',
  },
  {
    name: 'motor_coast_when_off',
    type: :bool,
  },
  {
    name: 'error_enable',
    type: :uint16_t,
    comment: 'This setting is a bitmask for errors that are enabled.'
  },
  {
    name: 'error_latch',
    type: :uint16_t,
    comment: 'This setting is a bitmask for errors that are enabled and latched.'
  },
]
# TODO: comments for all these settings (for jrk.h)

def puts_long_c_comment(stream, comment)
  comment.split("\n").each do |raw_line|
    c_line = "// #{raw_line}"
    if c_line.size > 80
      raise "Line too long (#{c_line.size}): #{c_line[0..100]}"
    end
    stream.puts c_line
  end
end

def setting_integer_type(setting_info)
  if setting_info[:type] == :enum
    :uint8_t
  else
    setting_info.fetch(:type)
  end
end

def setting_int_type_range(setting_info)
  type = setting_integer_type(setting_info)
  case type
  when :uint8_t then [0, 'UINT8_MAX']
  when :uint16_t then [0, 'UINT16_MAX']
  when :uint32_t then [0, 'UINT32_MAX']
  when :int8_t then ['INT8_MIN', 'INT8_MAX']
  when :int16_t then ['INT16_MIN', 'INT16_MAX']
  when :int32_t then ['INT32_MIN', 'INT32_MAX']
  else raise "Unknown integer type: #{type}"
  end
end

def setting_printf_format(setting_info)
  case setting_integer_type(setting_info)
  when :uint8_t, :uint16_t, :uint32_t, :bool
    'u'
  else
    'd'
  end
end

def generate_settings_struct_members(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "#{type} #{name};";
  end
end

def generate_settings_accessor_prototypes(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "// Sets the #{name} setting."
    if setting_info[:comment]
      stream.puts "//"
      puts_long_c_comment(stream, setting_info.fetch(:comment))
    end
    stream.puts "JRK_API"
    stream.puts "void jrk_settings_set_#{name}(jrk_settings *,"
    stream.puts "  #{type} #{name});\n"
    stream.puts
    stream.puts "// Gets the #{name} setting, which is described in"
    stream.puts "// jrk_settings_set_#{name}."
    stream.puts "JRK_API"
    stream.puts "#{type} jrk_settings_get_#{name}(const jrk_settings *);\n"
    stream.puts
  end
end

def generate_settings_accessors(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "void jrk_settings_set_#{name}(jrk_settings * settings, #{type} #{name})\n"
    stream.puts "{"
    stream.puts "  if (settings == NULL) { return; }"
    stream.puts "  settings->#{name} = #{name};"
    stream.puts "}"
    stream.puts
    stream.puts "#{type} jrk_settings_get_#{name}(const jrk_settings * settings)\n"
    stream.puts "{"
    stream.puts "  if (settings == NULL) { return 0; }"
    stream.puts "  return settings->#{name};"
    stream.puts "}"
    stream.puts
  end
end

def generate_settings_cpp_accessors(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "/// Wrapper for jrk_settings_set_#{name}()."
    stream.puts "void set_#{name}(#{type} value) noexcept"
    stream.puts "{"
    stream.puts "  jrk_settings_set_#{name}(pointer, value);"
    stream.puts "}"
    stream.puts
    stream.puts "/// Wrapper for jrk_settings_get_#{name}()."
    stream.puts "#{type} get_#{name}() const noexcept"
    stream.puts "{"
    stream.puts "  return jrk_settings_get_#{name}(pointer);"
    stream.puts "}"
    stream.puts
  end
end

def generate_settings_defaults_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    default = setting_info[:default]
    next unless default && default != 0 && !setting_info[:default_is_zero]
    stream.puts "jrk_settings_set_#{name}(settings, #{default});"
  end
end

def generate_settings_fixing_code(stream)
  Settings.each do |setting_info|
    next if setting_info[:custom_fix]

    name = setting_info.fetch(:name)
    english_name = setting_info.fetch(:english_name) { name.gsub('_', ' ') }
    type = setting_info.fetch(:type)
    integer_type = setting_integer_type(setting_info)
    pf = setting_printf_format(setting_info)

    if type == :bool
      # bools always hold a valid value so there are no automatic fixes for them
      next
    end

    stream.puts "{"
    stream.puts "  #{integer_type} #{name} = jrk_settings_get_#{name}(settings);"

    if type == :enum
      english_default = setting_info.fetch(:english_default)
      max = setting_info.fetch(:max)
      default = setting_info.fetch(:default)

      stream.puts "  if (#{name} > #{max})"
      stream.puts "  {"
      stream.puts "    #{name} = #{default};"
      stream.puts "    jrk_sprintf(warnings,"
      stream.puts "      \"Warning: The #{english_name} was invalid \""
      stream.puts "      \"so it will be changed to #{english_default}.\");"
      stream.puts "  }"
    else
      if setting_info[:range]
        min, max = setting_info[:range].minmax
      else
        min = setting_info[:min]
        max = setting_info[:max]
      end

      if min && !(pf == 'u' && min == 0)
        stream.puts "  if (#{name} < #{min})"
        stream.puts "  {"
        stream.puts "    #{name} = #{min};"
        stream.puts "    jrk_sprintf(warnings,"
        stream.puts "      \"Warning: The #{english_name} was too low \""
        stream.puts "      \"so it will be changed to %#{pf}.\\n\", #{name});"
        stream.puts "  }"
      end

      if max
        stream.puts "  if (#{name} > #{max})"
        stream.puts "  {"
        stream.puts "    #{name} = #{max};"
        stream.puts "    jrk_sprintf(warnings,"
        stream.puts "      \"Warning: The #{english_name} was too high \""
        stream.puts "      \"so it will be changed to %#{pf}.\\n\", #{max});"
        stream.puts "  }"
      end
    end

    stream.puts "  jrk_settings_set_#{name}(settings, #{name});"
    stream.puts "}"
    stream.puts
  end
end

def generate_buffer_to_settings_code(stream)
  Settings.each do |setting_info|
    next if setting_info[:custom_eeprom]

    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "{"
    if type == :bool
      stream.puts "  bool #{name} = buf[JRK_SETTING_#{name.upcase}] & 1;"
    elsif [:uint8_t, :int8_t].include?(type)
      stream.puts "  #{type} #{name} = buf[JRK_SETTING_#{name.upcase}];"
    else
      stream.puts "  #{type} #{name} = read_#{type}(buf + JRK_SETTING_#{name.upcase});"
    end
    stream.puts "  jrk_settings_set_#{name}(settings, #{name});"
    stream.puts "}"
    stream.puts
  end
end

def generate_settings_to_buffer_code(stream)
  Settings.each do |setting_info|
    next if setting_info[:custom_eeprom]

    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "{"
    stream.puts "  #{type} #{name} = jrk_settings_get_#{name}(settings);"
    if [:uint8_t, :bool, :int8_t].include?(type)
      stream.puts "  buf[JRK_SETTING_#{name.upcase}] = #{name};"
    else
      stream.puts "  write_#{type}(buf + JRK_SETTING_#{name.upcase}, #{name});"
    end
    stream.puts "}"
    stream.puts
  end
end

def generate_settings_file_parsing_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_info.fetch(:type)

    stream.puts "else if (!strcmp(key, \"#{name}\"))"
    stream.puts "{"
    if type == :enum
      stream.puts "  uint32_t #{name};"
      stream.puts "  if (!jrk_name_to_code(jrk_#{name}_names_short, value, &#{name}))"
      stream.puts "  {"
      stream.puts "    return jrk_error_create(\"Unrecognized #{name} value.\");"
      stream.puts "  }"
    elsif type == :bool
      stream.puts "  uint32_t #{name};"
      stream.puts "  if (!jrk_name_to_code(jrk_bool_names, value, &#{name}))"
      stream.puts "  {"
      stream.puts "    return jrk_error_create(\"Unrecognized #{name} value.\");"
      stream.puts "  }"
      stream.puts "  jrk_settings_set_#{name}(settings, #{name});"
    else
      int_type_min, int_type_max = setting_int_type_range(setting_info)
      stream.puts "  int64_t #{name};"
      stream.puts "  if (jrk_string_to_i64(value, &#{name}))"
      stream.puts "  {"
      stream.puts "    return jrk_error_create(\"Invalid #{name} value.\");"
      stream.puts "  }"
      stream.puts "  if (#{name} < #{int_type_min} || #{name} > #{int_type_max})"
      stream.puts "  {"
      stream.puts "    return jrk_error_create("
      stream.puts "      \"The #{name} value is out of range.\");"
      stream.puts "  }"
    end
    stream.puts "  jrk_settings_set_#{name}(settings, #{name});"
    stream.puts "}"
  end
end

def generate_settings_file_printing_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_info.fetch(:type)
    int_type = setting_integer_type(setting_info)
    pf = setting_printf_format(setting_info)
    stream.puts "{"
    stream.puts "  #{int_type} #{name} = jrk_settings_get_#{name}(settings);"
    if type == :enum
      stream.puts "  const char * value_str = \"\";"
      stream.puts "  jrk_code_to_name(jrk_#{name}_names_short, #{name}, &value_str);"
      stream.puts "  jrk_sprintf(&str, \"#{name}: %s\\n\", value_str);"
    elsif type == :bool
      stream.puts "  jrk_sprintf(&str, \"#{name}: %s\\n\", "
      stream.puts "    #{name} ? \"true\" : \"false\");"
    else
      stream.puts "  jrk_sprintf(&str, \"#{name}: %#{pf}\\n\", #{name});"
    end
    stream.puts "}"
    stream.puts
  end
end
