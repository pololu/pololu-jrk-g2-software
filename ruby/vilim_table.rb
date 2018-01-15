def print_vilim_table(config)
  r_ilim_top = config.fetch(:r_ilim_top)
  r_ilim_bot = config.fetch(:r_ilim_bot)
  v_dac_top = 0x10000

  (0..31).each do |level|
    r_dac_top = (32 - level) * 5000
    r_dac_bot = level * 5000

    v_dacout = v_dac_top.to_f * r_dac_bot / (r_dac_bot + r_dac_top)
    r_dacout = 1 / (1 / r_dac_top.to_f + 1 / r_dac_bot.to_f)

    v_ilim = v_dacout.to_f * r_ilim_bot / (r_ilim_bot + r_ilim_top + r_dacout)

    puts "  #{v_ilim.to_i},"
  end
end

umc04a_30v_config = {
  r_ilim_top: 41200,
  r_ilim_bot: 30100,
}

umc04a_40v_config = {
  r_ilim_top: 30100,
  r_ilim_bot: 68100,
}

puts "umc04a 30V"
print_vilim_table(umc04a_30v_config)
puts

puts "umc04a 40V"
print_vilim_table(umc04a_40v_config)
puts

