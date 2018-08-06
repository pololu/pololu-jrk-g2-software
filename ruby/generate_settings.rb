require_relative 'settings'

def puts_long_c_comment(stream, comment)
  comment.split("\n").each do |raw_line|
    c_line = "// #{raw_line}"
    if c_line.size > 80
      raise "Line too long (#{c_line.size}): #{c_line[0..100]}"
    end
    stream.puts c_line.rstrip
  end
end

def product_if_statement(info)
  if info[:products]
    "if (#{info[:products]})"
  else
    nil
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

    s = []
    s << product_if_statement(setting_info)
    s << "{"
    s << "  #{integer_type} #{name} = jrk_settings_get_#{name}(settings);"

    if type == :enum
      english_default = setting_info.fetch(:english_default)
      max = setting_info.fetch(:max)
      default = setting_info.fetch(:default)

      s << "  if (#{name} > #{max})"
      s << "  {"
      s << "    #{name} = #{default};"
      s << "    jrk_sprintf(warnings,"
      s << "      \"Warning: The #{english_name} is invalid \""
      s << "      \"so it will be changed to #{english_default}.\\n\");"
      s << "  }"
    else
      if setting_info[:range]
        min, max = setting_info[:range].minmax
      else
        min = setting_info[:min]
        max = setting_info[:max]
      end

      if min && !(pf == 'u' && min == 0)
        s << "  if (#{name} < #{min})"
        s << "  {\n"
        s << "    #{name} = #{min};"
        s << "    jrk_sprintf(warnings,"
        s << "      \"Warning: The #{english_name} is too low \""
        s << "      \"so it will be changed to %#{pf}.\\n\", #{name});"
        s << "  }\n"
        keep = true
      end

      if max
        s << "  if (#{name} > #{max})"
        s << "  {"
        s << "    #{name} = #{max};"
        s << "    jrk_sprintf(warnings,"
        s << "      \"Warning: The #{english_name} is too high \""
        s << "      \"so it will be changed to %#{pf}.\\n\", #{name});"
        s << "  }"
        keep = true
      end

      next if !keep
    end

    s << "  jrk_settings_set_#{name}(settings, #{name});"
    s << "}"
    s << ''
    s.compact.each { |l| stream.puts l }
  end
end

def generate_buffer_to_settings_code(stream)
  Settings.each do |setting_info|
    next if setting_info[:custom_eeprom]

    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    addr = setting_info.fetch(:address, "JRK_SETTING_#{name.upcase}")
    bit_addr = setting_info.fetch(:bit_address, 0)
    mask = setting_info.fetch(:mask, nil)
    mask ||= 1 if type == :bool
    shift_mask = ""
    shift_mask << " >> #{bit_addr}" if bit_addr != 0
    shift_mask << " & #{mask}" if mask

    s = []
    s << product_if_statement(setting_info)
    s << "{"
    if [:bool, :uint8_t, :int8_t].include?(type)
      s << "  #{type} #{name} = buf[#{addr}]#{shift_mask};"
    else
      raise NotImplementedError if bit_addr != 0 || mask
      s << "  #{type} #{name} = read_#{type}(buf + #{addr});"
    end
    s << "  jrk_settings_set_#{name}(settings, #{name});"
    s << "}"
    s << ""
    s.compact.each { |l| stream.puts l }
  end
end

def generate_settings_to_buffer_code(stream)
  Settings.each do |setting_info|
    next if setting_info[:custom_eeprom]

    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    addr = setting_info.fetch(:address, "JRK_SETTING_#{name.upcase}")
    bit_addr = setting_info.fetch(:bit_address, 0)
    mask = setting_info.fetch(:mask, nil)

    mask_shift_prefix = ""
    mask_shift = ""
    if mask
      mask_shift << " & #{mask}"
    end
    if bit_addr != 0
      if mask
        mask_shift_prefix << "("
        mask_shift << ")"
      end
      mask_shift << " << #{bit_addr}"
    end
    op = "="
    if mask || type == :bool
      op = "|="
    end

    s = []
    s << product_if_statement(setting_info)
    s << "{"
    s << "  #{type} #{name} = jrk_settings_get_#{name}(settings);"
    if [:bool, :uint8_t, :int8_t].include?(type)
      s << "  buf[#{addr}] #{op} #{mask_shift_prefix}#{name}#{mask_shift};"
    else
      s << "  write_#{type}(buf + JRK_SETTING_#{name.upcase}, #{name});"
    end
    s << "}"
    s << ""
    s.compact.each { |l| stream.puts l }
  end
end

def generate_settings_file_parsing_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_info.fetch(:type)

    s = []
    s << "else if (!strcmp(key, \"#{name}\"))"
    s << "{"
    if type == :enum
      s << "  uint32_t #{name};"
      s << "  if (!jrk_name_to_code(jrk_#{name}_names_short, value, &#{name}))"
      s << "  {"
      s << "    return jrk_error_create(\"Unrecognized #{name} value.\");"
      s << "  }"
    elsif type == :bool
      s << "  uint32_t #{name};"
      s << "  if (!jrk_name_to_code(jrk_bool_names, value, &#{name}))"
      s << "  {"
      s << "    return jrk_error_create(\"Unrecognized #{name} value.\");"
      s << "  }"
    else
      int_type_min, int_type_max = setting_int_type_range(setting_info)
      s << "  int64_t #{name};"
      s << "  if (jrk_string_to_i64(value, &#{name}))"
      s << "  {"
      s << "    return jrk_error_create(\"Invalid #{name} value.\");"
      s << "  }"
      s << "  if (#{name} < #{int_type_min} || #{name} > #{int_type_max})"
      s << "  {"
      s << "    return jrk_error_create("
      s << "      \"The #{name} value is out of range.\");"
      s << "  }"
    end
    s << "  jrk_settings_set_#{name}(settings, #{name});"
    s << "}"
    s.compact.each { |l| stream.puts l }
  end
end

def generate_settings_file_printing_code(stream)
  Settings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_info.fetch(:type)
    int_type = setting_integer_type(setting_info)
    pf = setting_printf_format(setting_info)
    s = []
    s << product_if_statement(setting_info)
    s << "{"
    s << "  #{int_type} #{name} = jrk_settings_get_#{name}(settings);"
    if type == :enum
      s << "  const char * value_str = \"\";"
      s << "  jrk_code_to_name(jrk_#{name}_names_short, #{name}, &value_str);"
      s << "  jrk_sprintf(&str, \"#{name}: %s\\n\", value_str);"
    elsif type == :bool
      s << "  jrk_sprintf(&str, \"#{name}: %s\\n\","
      s << "    #{name} ? \"true\" : \"false\");"
    else
      s << "  jrk_sprintf(&str, \"#{name}: %#{pf}\\n\", #{name});"
    end
    s << "}"
    s << ""
    s.compact.each { |l| stream.puts l }
  end
end
