def compute_vilim_relative(config, dac_level)
  r_ilim_top = config.fetch(:r_ilim_top)
  r_ilim_bot = config.fetch(:r_ilim_bot)

  v_dac_top = 1.0
  r_dac_top = (32 - dac_level) * 5000
  r_dac_bot = dac_level * 5000

  v_dacout = v_dac_top * r_dac_bot / (r_dac_bot + r_dac_top)
  r_dacout = 1 / (1 / r_dac_top.to_f + 1 / r_dac_bot.to_f)

  v_dacout.to_f * r_ilim_bot / (r_ilim_bot + r_ilim_top + r_dacout)
end

def compute_vilim_and_ilim(config, code)
  rsense = config.fetch(:rsense)
  reference = [1.024, 2.048, 4.096][code >> 5 & 3]
  vilim = reference * compute_vilim_relative(config, code & 0x1F)
  current_limit = [0.0, vilim - 0.05].max / (20 * rsense)
  [vilim, current_limit]
end

def print_recommended_codes(config)
  table = []
  last_code = 0
  last_current_limit = -1000
  (0..95).each do |code|
    vilim, current_limit = compute_vilim_and_ilim(config, code)

    next if current_limit <= last_current_limit

    # Skip this code if the DAC reference level changed and the code we are looking at does
    # not give a significantly higher current limits.
    next if (code & 0x60) != (last_code & 0x60) && current_limit <= last_current_limit + 1.0

    table << [code, vilim, current_limit]
    last_code = code
    last_current_limit = current_limit
  end

  table.each do |code, vilim, current_limit|
    puts "  %-3s  // VILIM = %5.3f V, Current limit = %5.2f A" %
         ["#{code},", vilim, current_limit]
  end
end

def print_vilim_table(config)
  (0..31).each do |level|
    vilim = (0x10000 * compute_vilim_relative(config, level)).to_i
    puts "  #{vilim},"
  end
end

umc04a_30v_config = {
  rsense: 0.001,
  r_ilim_top: 41200,
  r_ilim_bot: 30100,
}

umc04a_40v_config = {
  rsense: 0.002,
  r_ilim_top: 30100,
  r_ilim_bot: 68100,
}

puts "static const uint16_t jrk_umc04a_30v_vilim_table[32] ="
puts "{"
print_vilim_table(umc04a_30v_config)
puts "};"
puts

puts "static const uint16_t jrk_umc04a_40v_vilim_table[32] ="
puts "{"
print_vilim_table(umc04a_40v_config)
puts "};"
puts

puts "static const uint16_t jrk_umc04a_30v_recommended_codes[] ="
puts "{"
print_recommended_codes(umc04a_30v_config)
puts "};"
puts

puts "static const uint16_t jrk_umc04a_40v_recommended_codes[] ="
puts "{"
print_recommended_codes(umc04a_40v_config)
puts "};"
puts
