require_relative 'settings'

Overridable = Settings.select { |s| s[:overridable] }

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
