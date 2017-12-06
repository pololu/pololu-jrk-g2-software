Settings = [
  {
    name: 'input_mode',
    type: :enum,
    max: 'JRK_INPUT_MODE_PULSE_WIDTH',
    default: 'JRK_INPUT_MODE_SERIAL',
    english_default: 'serial',
  },
  {
    name: 'input_minimum',
    type: :uint16_t,
    range: 0..4095,
  },
]
# TODO: comments for all these settings (for jrk.h)

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
  when :uint8_t, :uint16_t, :uint32_t
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
  stream.puts
end

def generate_settings_accessor_prototypes(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "// Sets the #{name} setting."
    if setting_info[:comment]
      stream.puts "//"
      stream.puts long_c_comment(setting_info.fetch(:comment))
    end
    stream.puts "JRK_API"
    stream.puts "void jrk_settings_set_#{name}(jrk_settings *, #{type} #{name});\n"
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
    stream.puts "void set_#{name}(#{type} #{name}) noexcept"
    stream.puts "{"
    stream.puts "  jrk_settings_set_#{name}(pointer, #{name});"
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

def generate_settings_fixing_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    english_name = setting_info.fetch(:english_name) { name.gsub('_', ' ') }
    type = setting_info.fetch(:type)
    integer_type = setting_integer_type(setting_info)
    pf = setting_printf_format(setting_info)

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
      min = setting_info.fetch(:min) { setting_info.fetch(:range).min }
      max = setting_info.fetch(:max) { setting_info.fetch(:range).max }

      unless pf == 'u' && min == 0
        stream.puts "  if (#{name} < #{min})"
        stream.puts "  {"
        stream.puts "    #{name} = #{min};"
        stream.puts "    jrk_sprintf(warnings,"
        stream.puts "      \"Warning: The #{english_name} was too low \""
        stream.puts "      \"so it will be changed to %#{pf}.\\n\", #{name});"
        stream.puts "  }"
      end

      stream.puts "  if (#{name} > #{max})"
      stream.puts "  {"
      stream.puts "    #{name} = #{max};"
      stream.puts "    jrk_sprintf(warnings,"
      stream.puts "      \"Warning: The #{english_name} was too low \""
      stream.puts "      \"so it will be changed to %#{pf}.\\n\", #{max});"
      stream.puts "  }"
    end

    stream.puts "  jrk_settings_set_#{name}(settings, #{name});"
    stream.puts "}"
    stream.puts
  end
end

def generate_buffer_to_settings_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "{"
    if type == :uint8_t
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
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "{"
    stream.puts "  #{type} #{name} = jrk_settings_get_#{name}(settings);"
    if type == :uint8_t
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
  stream.puts
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
      stream.puts "  jrk_sprintf(&str, \"#{name}: %s\\n\", value_str);";
    else
      stream.puts "  jrk_sprintf(&str, \"#{name}: %#{pf}\\n\", #{name});"
    end
    stream.puts "}"
    stream.puts
  end
end
