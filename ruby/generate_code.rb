#!/usr/bin/env ruby

# This Ruby script auto-generates certain parts of our C/C++ code that are repetitive.

require 'pathname'
require_relative 'generate_settings'
require_relative 'generate_overridable_settings'
require_relative 'generate_variables'

def generate_fragment(fragment_name, indent, filename)
  stream = IndentedStream.new(indent)
  puts fragment_name
  case fragment_name
  when 'settings struct members'
    generate_settings_struct_members(stream)
  when 'settings accessor prototypes'
    generate_settings_accessor_prototypes(stream)
  when 'settings accessors'
    generate_settings_accessors(stream)
  when 'settings C++ accessors'
    generate_settings_cpp_accessors(stream)
  when 'settings defaults'
    generate_settings_defaults_code(stream)
  when 'settings fixing code'
    generate_settings_fixing_code(stream)
  when 'buffer-to-settings code'
    generate_buffer_to_settings_code(stream)
  when 'settings-to-buffer code'
    generate_settings_to_buffer_code(stream)
  when 'settings file parsing code'
    generate_settings_file_parsing_code(stream)
  when 'settings file printing code'
    generate_settings_file_printing_code(stream)
  when 'overridable settings struct members'
    generate_overridable_settings_struct_members(stream)
  when 'overridable settings accessor prototypes'
    generate_overridable_settings_accessor_prototypes(stream)
  when 'overridable settings accessors'
    generate_overridable_settings_accessors(stream)
  when 'overridable settings C++ accessors'
    generate_overridable_settings_cpp_accessors(stream)
  when 'buffer-to-overridable-settings code'
    generate_buffer_to_overridable_settings_code(stream)
  when 'settings-to-overridable-buffer code'
    generate_overridable_settings_to_buffer_code(stream)
  when 'variables struct members'
    generate_variables_struct_members(stream)
  when 'variables getter prototypes'
    generate_variables_getter_prototypes(stream)
  when 'variables C++ getters'
    generate_variables_cpp_getters(stream)
  when 'buffer-to-variables code'
    generate_buffer_to_variables_code(stream)
  when 'variables getters'
    generate_variables_getters(stream)
  else
    raise "Unrecognized fragment in #{filename}: #{fragment_name}"
  end
  stream.io.string
end

class IndentedStream
  attr_accessor :indent
  attr_reader :io

  def initialize(indent, io = StringIO.new)
    @indent = indent
    @io = io
  end

  def puts(*args)
    @io.puts if args.empty?
    args.each do |arg|
      arg = arg.to_s
      @io.print indent unless arg.empty?
      @io.puts arg
    end
  end
end

def autogenerate_file_fragments_core(file, &proc)
  new_file = ''

  line_enum = file.each_line

  lines_generated = 0

  loop do
    line = line_enum.next
    new_file << line

    md = line.match(/(\s*)\/\/ Beginning of auto-generated (.+)\./)
    next unless md

    indent = md[1]
    fragment_name = md[2]

    end_line = loop do
      line = line_enum.next
      md = line.match(/\s*\/\/ End of auto-generated (.+)\./)
      next if !md
      if md[1] != fragment_name
        raise "Mismatching ending line: expected #{fragment_name}, got #{md[1]}."
      end
      break line
    end

    if !end_line
      raise "Could not find ending line for auto-generated #{fragment_name} in #{file}."
    end

    new_file << "\n"

    fragment = proc.call(fragment_name, indent, file)
    until fragment.end_with?("\n\n")
      fragment << "\n"
    end

    new_file << fragment
    new_file << end_line
    lines_generated += fragment.split("\n").size
  end

  if lines_generated > 0
    File.open(file, 'wb') do |f|
      f.write new_file
    end
  end

  lines_generated
end

def autogenerate_file_fragments(files, &proc)
  lines_generated = 0
  Array(files).each do |file|
    lines_generated += autogenerate_file_fragments_core(file, &proc)
  end
  puts "Generated #{lines_generated} lines of code."
end

files = Pathname.glob("include/*") + Pathname.glob("lib/*.c")
autogenerate_file_fragments(files, &method(:generate_fragment))
