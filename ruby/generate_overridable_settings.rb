require_relative 'generate_settings'

OverridableSettings = Settings.select { |s| s[:overridable] }

def generate_overridable_settings_struct_members(stream)
  OverridableSettings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "#{type} #{name};";
  end
end

def generate_overridable_settings_accessor_prototypes(stream)
  OverridableSettings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "// Sets the #{name} setting"
    stream.puts "// in a jrk_overridable_settings object."
    stream.puts "//"
    stream.puts "// See jrk_settings_set_#{name}() for more info."
    stream.puts "JRK_API"
    stream.puts "void jrk_overridable_settings_set_#{name}(jrk_overridable_settings *,"
    stream.puts "  #{type} #{name});\n"
    stream.puts
    stream.puts "// Gets the #{name} setting"
    stream.puts "// in a jrk_overridable_settings object."
    stream.puts "//"
    stream.puts "// See jrk_settings_set_#{name}() for more info."
    stream.puts "JRK_API"
    stream.puts "#{type} jrk_overridable_settings_get_#{name}(const jrk_overridable_settings *);\n"
    stream.puts
  end
end

def generate_overridable_settings_accessors(stream)
  OverridableSettings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "void jrk_overridable_settings_set_#{name}("
    stream.puts "  jrk_overridable_settings * settings,"
    stream.puts "  #{type} #{name})\n"
    stream.puts "{"
    stream.puts "  if (settings == NULL) { return; }"
    stream.puts "  settings->#{name} = #{name};"
    stream.puts "}"
    stream.puts
    stream.puts "#{type} jrk_overridable_settings_get_#{name}("
    stream.puts "  const jrk_overridable_settings * settings)\n"
    stream.puts "{"
    stream.puts "  if (settings == NULL) { return 0; }"
    stream.puts "  return settings->#{name};"
    stream.puts "}"
    stream.puts
  end
end

def generate_overridable_settings_cpp_accessors(stream)
  OverridableSettings.each do |setting_info|
    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    stream.puts "/// Wrapper for jrk_overridable_settings_set_#{name}()."
    stream.puts "void set_#{name}(#{type} value) noexcept"
    stream.puts "{"
    stream.puts "  jrk_overridable_settings_set_#{name}(pointer, value);"
    stream.puts "}"
    stream.puts
    stream.puts "/// Wrapper for jrk_overridable_settings_get_#{name}()."
    stream.puts "#{type} get_#{name}() const noexcept"
    stream.puts "{"
    stream.puts "  return jrk_overridable_settings_get_#{name}(pointer);"
    stream.puts "}"
    stream.puts
  end
end

def generate_buffer_to_overridable_settings_code(stream)
  OverridableSettings.each do |setting_info|
    next if setting_info[:custom_eeprom]

    name = setting_info.fetch(:name)
    type = setting_integer_type(setting_info)
    addr = setting_info.fetch(:address, "JRK_SETTING_#{name.upcase}")
    offset = 'JRK_OVERRIDABLE_SETTINGS_START'
    bit_addr = setting_info.fetch(:bit_address, 0)

    stream.puts "{"
    if type == :bool
      shift = " >> #{bit_addr}" if bit_addr != 0
      stream.puts "  bool #{name} ="
      stream.puts "    buf[#{addr} - #{offset}]#{shift} & 1;"
    elsif [:uint8_t, :int8_t].include?(type)
      stream.puts "  #{type} #{name} ="
      stream.puts "    buf[#{addr} - #{offset}];"
    else
      stream.puts "  #{type} #{name} = read_#{type}("
      stream.puts "    buf + (#{addr} - #{offset}));"
    end
    stream.puts "  jrk_overridable_settings_set_#{name}(settings, #{name});"
    stream.puts "}"
    stream.puts
  end
end
