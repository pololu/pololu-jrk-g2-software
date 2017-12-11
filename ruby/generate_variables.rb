require_relative 'variables'

def generate_variables_struct_members(stream)
  Variables.each do |info|
    name = info.fetch(:name)
    type = info.fetch(:type)
    stream.puts "#{type} #{name};"
  end
end

def generate_variables_getter_prototypes(stream)
  Variables.each do |info|
    name = info.fetch(:name)
    type = info.fetch(:type)
    stream.puts "// Gets the #{name} variable."
    if info[:comment]
      stream.puts "//"
      puts_long_c_comment(stream, info.fetch(:comment))
    end
    stream.puts "JRK_API"
    stream.puts "#{type} jrk_variables_get_#{name}(const jrk_variables *);"
    stream.puts
  end
end

def generate_variables_cpp_getters(stream)
  Variables.each do |info|
    name = info.fetch(:name)
    type = info.fetch(:type)
    stream.puts "/// Wrapper for jrk_variables_get_#{name}()."
    stream.puts "#{type} get_#{name}() const noexcept"
    stream.puts "{"
    stream.puts "  return jrk_variables_get_#{name}(pointer);"
    stream.puts "}"
    stream.puts
  end
end

def generate_buffer_to_variables_code(stream)
  Variables.each do |info|
    name = info.fetch(:name)
    type = info.fetch(:type)
    addr = info.fetch(:address, "JRK_VAR_#{name.upcase}")
    bit_addr = info.fetch(:bit_address, 0)

    if type == :bool
      shift = " >> #{bit_addr}" if bit_addr != 0
      stream.puts "vars->#{name} = buf[#{addr}]#{shift} & 1;"
    elsif [:uint8_t, :int8_t].include?(type)
      stream.puts "vars->#{name} = buf[#{addr}];"
    else
      stream.puts "vars->#{name} = read_#{type}(buf + #{addr});"
    end
  end
end

def generate_variables_getters(stream)
  Variables.each do |info|
    name = info.fetch(:name)
    type = info.fetch(:type)
    stream.puts "#{type} jrk_variable_get_#{name}(const jrk_variables * vars)\n"
    stream.puts "{"
    stream.puts "  if (vars == NULL) { return 0; }"
    stream.puts "  return vars->#{name};"
    stream.puts "}"
    stream.puts
  end
end
