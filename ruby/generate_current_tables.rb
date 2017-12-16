#!/usr/bin/env ruby

table = []

(0..124).to_a.reverse.each do |raw|
  next if raw >= 32 && (raw % 2) != 0
  next if raw >= 64 && (raw % 4) != 0

  ma = (raw * 32.0 / 0.12 - 833).round
  ma = 0 if ma < 0

  table << [raw, ma]
end

# If there are multiple entries with a 0 milliamp value, tweak the milliamp
# values to make them distinct, because some users might care about which one is
# selected.
zero_count = 0
table.reverse_each do |entry|
  if entry[1] == 0
    entry[1] = zero_count
    zero_count += 1
  end
end

table.each do |raw, ma|
  puts "  { %3d, %5d }," % [raw, ma]
end
