#include "jrk_internal.h"

static const uint16_t jrk_umc04a_30v_vilim_table[32] =
{
  0,
  809,
  1528,
  2178,
  2776,
  3335,
  3865,
  4374,
  4868,
  5352,
  5833,
  6314,
  6799,
  7292,
  7797,
  8319,
  8861,
  9428,
  10025,
  10658,
  11331,
  12054,
  12833,
  13679,
  14604,
  15623,
  16752,
  18014,
  19437,
  21058,
  22923,
  25097,
};

static const uint16_t jrk_umc04a_30v_recommended_encoded_hard_current_limits[] =
{
  0,   // VILIM = 0.000 V, Current limit =  0.00 A
  5,   // VILIM = 0.052 V, Current limit =  0.11 A
  6,   // VILIM = 0.060 V, Current limit =  0.52 A
  7,   // VILIM = 0.068 V, Current limit =  0.92 A
  8,   // VILIM = 0.076 V, Current limit =  1.30 A
  9,   // VILIM = 0.084 V, Current limit =  1.68 A
  10,  // VILIM = 0.091 V, Current limit =  2.06 A
  11,  // VILIM = 0.099 V, Current limit =  2.43 A
  12,  // VILIM = 0.106 V, Current limit =  2.81 A
  13,  // VILIM = 0.114 V, Current limit =  3.20 A
  14,  // VILIM = 0.122 V, Current limit =  3.59 A
  15,  // VILIM = 0.130 V, Current limit =  4.00 A
  16,  // VILIM = 0.138 V, Current limit =  4.42 A
  17,  // VILIM = 0.147 V, Current limit =  4.87 A
  18,  // VILIM = 0.157 V, Current limit =  5.33 A
  19,  // VILIM = 0.167 V, Current limit =  5.83 A
  20,  // VILIM = 0.177 V, Current limit =  6.35 A
  21,  // VILIM = 0.188 V, Current limit =  6.92 A
  22,  // VILIM = 0.201 V, Current limit =  7.53 A
  23,  // VILIM = 0.214 V, Current limit =  8.19 A
  24,  // VILIM = 0.228 V, Current limit =  8.91 A
  25,  // VILIM = 0.244 V, Current limit =  9.71 A
  26,  // VILIM = 0.262 V, Current limit = 10.59 A
  27,  // VILIM = 0.281 V, Current limit = 11.57 A
  28,  // VILIM = 0.304 V, Current limit = 12.69 A
  29,  // VILIM = 0.329 V, Current limit = 13.95 A
  30,  // VILIM = 0.358 V, Current limit = 15.41 A
  31,  // VILIM = 0.392 V, Current limit = 17.11 A
  55,  // VILIM = 0.427 V, Current limit = 18.87 A
  56,  // VILIM = 0.456 V, Current limit = 20.32 A
  57,  // VILIM = 0.488 V, Current limit = 21.91 A
  58,  // VILIM = 0.524 V, Current limit = 23.68 A
  59,  // VILIM = 0.563 V, Current limit = 25.65 A
  60,  // VILIM = 0.607 V, Current limit = 27.87 A
  61,  // VILIM = 0.658 V, Current limit = 30.40 A
  62,  // VILIM = 0.716 V, Current limit = 33.32 A
  63,  // VILIM = 0.784 V, Current limit = 36.71 A
  87,  // VILIM = 0.855 V, Current limit = 40.25 A
  88,  // VILIM = 0.913 V, Current limit = 43.14 A
  89,  // VILIM = 0.976 V, Current limit = 46.32 A
  90,  // VILIM = 1.047 V, Current limit = 49.85 A
  91,  // VILIM = 1.126 V, Current limit = 53.79 A
  92,  // VILIM = 1.215 V, Current limit = 58.24 A
  93,  // VILIM = 1.316 V, Current limit = 63.31 A
  94,  // VILIM = 1.433 V, Current limit = 69.14 A
  95,  // VILIM = 1.569 V, Current limit = 75.93 A
};

static const uint16_t jrk_umc04a_40v_vilim_table[32] =
{
  0,
  1353,
  2592,
  3742,
  4821,
  5845,
  6826,
  7776,
  8703,
  9615,
  10519,
  11423,
  12333,
  13254,
  14192,
  15154,
  16146,
  17175,
  18247,
  19371,
  20555,
  21809,
  23143,
  24572,
  26109,
  27772,
  29583,
  31566,
  33752,
  36179,
  38894,
  41958,
};

static const uint16_t jrk_umc04a_40v_recommended_encoded_hard_current_limits[] =
{
  0,   // VILIM = 0.000 V, Current limit =  0.00 A
  3,   // VILIM = 0.058 V, Current limit =  0.21 A
  4,   // VILIM = 0.075 V, Current limit =  0.63 A
  5,   // VILIM = 0.091 V, Current limit =  1.03 A
  6,   // VILIM = 0.107 V, Current limit =  1.42 A
  7,   // VILIM = 0.122 V, Current limit =  1.79 A
  8,   // VILIM = 0.136 V, Current limit =  2.15 A
  9,   // VILIM = 0.150 V, Current limit =  2.51 A
  10,  // VILIM = 0.164 V, Current limit =  2.86 A
  11,  // VILIM = 0.178 V, Current limit =  3.21 A
  12,  // VILIM = 0.193 V, Current limit =  3.57 A
  13,  // VILIM = 0.207 V, Current limit =  3.93 A
  14,  // VILIM = 0.222 V, Current limit =  4.29 A
  15,  // VILIM = 0.237 V, Current limit =  4.67 A
  16,  // VILIM = 0.252 V, Current limit =  5.06 A
  17,  // VILIM = 0.268 V, Current limit =  5.46 A
  18,  // VILIM = 0.285 V, Current limit =  5.88 A
  19,  // VILIM = 0.303 V, Current limit =  6.32 A
  20,  // VILIM = 0.321 V, Current limit =  6.78 A
  21,  // VILIM = 0.341 V, Current limit =  7.27 A
  22,  // VILIM = 0.362 V, Current limit =  7.79 A
  23,  // VILIM = 0.384 V, Current limit =  8.35 A
  24,  // VILIM = 0.408 V, Current limit =  8.95 A
  25,  // VILIM = 0.434 V, Current limit =  9.60 A
  26,  // VILIM = 0.462 V, Current limit = 10.31 A
  27,  // VILIM = 0.493 V, Current limit = 11.08 A
  28,  // VILIM = 0.527 V, Current limit = 11.93 A
  29,  // VILIM = 0.565 V, Current limit = 12.88 A
  30,  // VILIM = 0.608 V, Current limit = 13.94 A
  31,  // VILIM = 0.656 V, Current limit = 15.14 A
  54,  // VILIM = 0.723 V, Current limit = 16.83 A
  55,  // VILIM = 0.768 V, Current limit = 17.95 A
  56,  // VILIM = 0.816 V, Current limit = 19.15 A
  57,  // VILIM = 0.868 V, Current limit = 20.45 A
  58,  // VILIM = 0.924 V, Current limit = 21.86 A
  59,  // VILIM = 0.986 V, Current limit = 23.41 A
  60,  // VILIM = 1.055 V, Current limit = 25.12 A
  61,  // VILIM = 1.131 V, Current limit = 27.01 A
  62,  // VILIM = 1.215 V, Current limit = 29.14 A
  63,  // VILIM = 1.311 V, Current limit = 31.53 A
  85,  // VILIM = 1.363 V, Current limit = 32.83 A
  86,  // VILIM = 1.446 V, Current limit = 34.91 A
  87,  // VILIM = 1.536 V, Current limit = 37.14 A
  88,  // VILIM = 1.632 V, Current limit = 39.55 A
  89,  // VILIM = 1.736 V, Current limit = 42.15 A
  90,  // VILIM = 1.849 V, Current limit = 44.97 A
  91,  // VILIM = 1.973 V, Current limit = 48.07 A
  92,  // VILIM = 2.110 V, Current limit = 51.49 A
  93,  // VILIM = 2.261 V, Current limit = 55.28 A
  94,  // VILIM = 2.431 V, Current limit = 59.52 A
  95,  // VILIM = 2.622 V, Current limit = 64.31 A
};

static const uint16_t jrk_umc05a_30v_vilim_table[32] =
{
  0,
  935,
  1791,
  2585,
  3330,
  4037,
  4715,
  5371,
  6011,
  6641,
  7265,
  7890,
  8518,
  9154,
  9802,
  10466,
  11152,
  11862,
  12603,
  13379,
  14197,
  15063,
  15985,
  16971,
  18033,
  19183,
  20434,
  21804,
  23314,
  24991,
  26868,
  28986,
};

static const uint16_t jrk_umc05a_30v_recommended_encoded_hard_current_limits[] =
{
  0,   // VILIM = 0.000 V, Current limit =  0.00 A
  4,   // VILIM = 0.052 V, Current limit =  0.06 A
  5,   // VILIM = 0.063 V, Current limit =  0.39 A
  6,   // VILIM = 0.074 V, Current limit =  0.71 A
  7,   // VILIM = 0.084 V, Current limit =  1.02 A
  8,   // VILIM = 0.094 V, Current limit =  1.32 A
  9,   // VILIM = 0.104 V, Current limit =  1.61 A
  10,  // VILIM = 0.114 V, Current limit =  1.91 A
  11,  // VILIM = 0.123 V, Current limit =  2.20 A
  12,  // VILIM = 0.133 V, Current limit =  2.49 A
  13,  // VILIM = 0.143 V, Current limit =  2.79 A
  14,  // VILIM = 0.153 V, Current limit =  3.09 A
  15,  // VILIM = 0.164 V, Current limit =  3.41 A
  16,  // VILIM = 0.174 V, Current limit =  3.73 A
  17,  // VILIM = 0.185 V, Current limit =  4.06 A
  18,  // VILIM = 0.197 V, Current limit =  4.41 A
  19,  // VILIM = 0.209 V, Current limit =  4.77 A
  20,  // VILIM = 0.222 V, Current limit =  5.15 A
  21,  // VILIM = 0.235 V, Current limit =  5.56 A
  22,  // VILIM = 0.250 V, Current limit =  5.99 A
  23,  // VILIM = 0.265 V, Current limit =  6.46 A
  24,  // VILIM = 0.282 V, Current limit =  6.95 A
  25,  // VILIM = 0.300 V, Current limit =  7.49 A
  26,  // VILIM = 0.319 V, Current limit =  8.08 A
  27,  // VILIM = 0.341 V, Current limit =  8.72 A
  28,  // VILIM = 0.364 V, Current limit =  9.43 A
  29,  // VILIM = 0.390 V, Current limit = 10.21 A
  30,  // VILIM = 0.420 V, Current limit = 11.09 A
  31,  // VILIM = 0.453 V, Current limit = 12.09 A
  54,  // VILIM = 0.500 V, Current limit = 13.49 A
  55,  // VILIM = 0.530 V, Current limit = 14.41 A
  56,  // VILIM = 0.564 V, Current limit = 15.41 A
  57,  // VILIM = 0.599 V, Current limit = 16.48 A
  58,  // VILIM = 0.639 V, Current limit = 17.66 A
  59,  // VILIM = 0.681 V, Current limit = 18.94 A
  60,  // VILIM = 0.729 V, Current limit = 20.36 A
  61,  // VILIM = 0.781 V, Current limit = 21.93 A
  62,  // VILIM = 0.840 V, Current limit = 23.69 A
  63,  // VILIM = 0.906 V, Current limit = 25.67 A
  85,  // VILIM = 0.941 V, Current limit = 26.74 A
  86,  // VILIM = 0.999 V, Current limit = 28.47 A
  87,  // VILIM = 1.061 V, Current limit = 30.32 A
  88,  // VILIM = 1.127 V, Current limit = 32.31 A
  89,  // VILIM = 1.199 V, Current limit = 34.47 A
  90,  // VILIM = 1.277 V, Current limit = 36.81 A
  91,  // VILIM = 1.363 V, Current limit = 39.38 A
  92,  // VILIM = 1.457 V, Current limit = 42.21 A
  93,  // VILIM = 1.562 V, Current limit = 45.36 A
  94,  // VILIM = 1.679 V, Current limit = 48.88 A
  95,  // VILIM = 1.812 V, Current limit = 52.85 A
};

static const uint16_t jrk_umc05a_40v_vilim_table[32] =
{
  0,
  1124,
  2169,
  3150,
  4081,
  4970,
  5828,
  6662,
  7478,
  8282,
  9081,
  9879,
  10680,
  11490,
  12313,
  13153,
  14016,
  14907,
  15831,
  16793,
  17800,
  18860,
  19979,
  21167,
  22434,
  23793,
  25256,
  26841,
  28567,
  30457,
  32541,
  34854,
};

static const uint16_t jrk_umc05a_40v_recommended_encoded_hard_current_limits[] =
{
  0,   // VILIM = 0.000 V, Current limit =  0.00 A
  4,   // VILIM = 0.064 V, Current limit =  0.28 A
  5,   // VILIM = 0.078 V, Current limit =  0.55 A
  6,   // VILIM = 0.091 V, Current limit =  0.82 A
  7,   // VILIM = 0.104 V, Current limit =  1.08 A
  8,   // VILIM = 0.117 V, Current limit =  1.34 A
  9,   // VILIM = 0.129 V, Current limit =  1.59 A
  10,  // VILIM = 0.142 V, Current limit =  1.84 A
  11,  // VILIM = 0.154 V, Current limit =  2.09 A
  12,  // VILIM = 0.167 V, Current limit =  2.34 A
  13,  // VILIM = 0.180 V, Current limit =  2.59 A
  14,  // VILIM = 0.192 V, Current limit =  2.85 A
  15,  // VILIM = 0.206 V, Current limit =  3.11 A
  16,  // VILIM = 0.219 V, Current limit =  3.38 A
  17,  // VILIM = 0.233 V, Current limit =  3.66 A
  18,  // VILIM = 0.247 V, Current limit =  3.95 A
  19,  // VILIM = 0.262 V, Current limit =  4.25 A
  20,  // VILIM = 0.278 V, Current limit =  4.56 A
  21,  // VILIM = 0.295 V, Current limit =  4.89 A
  22,  // VILIM = 0.312 V, Current limit =  5.24 A
  23,  // VILIM = 0.331 V, Current limit =  5.61 A
  24,  // VILIM = 0.351 V, Current limit =  6.01 A
  25,  // VILIM = 0.372 V, Current limit =  6.44 A
  26,  // VILIM = 0.395 V, Current limit =  6.89 A
  27,  // VILIM = 0.419 V, Current limit =  7.39 A
  28,  // VILIM = 0.446 V, Current limit =  7.93 A
  29,  // VILIM = 0.476 V, Current limit =  8.52 A
  30,  // VILIM = 0.508 V, Current limit =  9.17 A
  31,  // VILIM = 0.545 V, Current limit =  9.89 A
  54,  // VILIM = 0.624 V, Current limit = 11.49 A
  55,  // VILIM = 0.661 V, Current limit = 12.23 A
  56,  // VILIM = 0.701 V, Current limit = 13.02 A
  57,  // VILIM = 0.744 V, Current limit = 13.87 A
  58,  // VILIM = 0.789 V, Current limit = 14.79 A
  59,  // VILIM = 0.839 V, Current limit = 15.78 A
  60,  // VILIM = 0.893 V, Current limit = 16.85 A
  61,  // VILIM = 0.952 V, Current limit = 18.04 A
  62,  // VILIM = 1.017 V, Current limit = 19.34 A
  63,  // VILIM = 1.089 V, Current limit = 20.78 A
  85,  // VILIM = 1.179 V, Current limit = 22.58 A
  86,  // VILIM = 1.249 V, Current limit = 23.97 A
  87,  // VILIM = 1.323 V, Current limit = 25.46 A
  88,  // VILIM = 1.402 V, Current limit = 27.04 A
  89,  // VILIM = 1.487 V, Current limit = 28.74 A
  90,  // VILIM = 1.579 V, Current limit = 30.57 A
  91,  // VILIM = 1.678 V, Current limit = 32.55 A
  92,  // VILIM = 1.785 V, Current limit = 34.71 A
  93,  // VILIM = 1.904 V, Current limit = 37.07 A
  94,  // VILIM = 2.034 V, Current limit = 39.68 A
  95,  // VILIM = 2.178 V, Current limit = 42.57 A
};

// Given a jrk product code and 5-bit DAC level, this returns the expected
// voltage we will see on the pin of the motor driver that sets the current
// limit.  The units are set such that 0x10000 is the DAC reference, and 0 is
// GND.
static uint16_t jrk_get_vilim(uint32_t product, uint8_t dac_level)
{
  const uint16_t * table;
  if (product == JRK_PRODUCT_UMC04A_30V)
  {
    table = jrk_umc04a_30v_vilim_table;
  }
  else if (product == JRK_PRODUCT_UMC04A_40V)
  {
    table = jrk_umc04a_40v_vilim_table;
  }
  else if (product == JRK_PRODUCT_UMC05A_30V)
  {
    table = jrk_umc05a_30v_vilim_table;
  }
  else if (product == JRK_PRODUCT_UMC05A_40V)
  {
    table = jrk_umc05a_40v_vilim_table;
  }
  else
  {
    return 0;
  }
  return table[dac_level & 0x1F];
}

const uint16_t * jrk_get_recommended_encoded_hard_current_limits(
  uint32_t product, size_t * count)
{
  const uint16_t * table = 0;
  size_t size = 0;

  if (product == JRK_PRODUCT_UMC04A_30V)
  {
    table = jrk_umc04a_30v_recommended_encoded_hard_current_limits;
    size = sizeof(jrk_umc04a_30v_recommended_encoded_hard_current_limits);
  }
  else if (product == JRK_PRODUCT_UMC04A_40V)
  {
    table = jrk_umc04a_40v_recommended_encoded_hard_current_limits;
    size = sizeof(jrk_umc04a_40v_recommended_encoded_hard_current_limits);
  }
  else if (product == JRK_PRODUCT_UMC05A_30V)
  {
    table = jrk_umc05a_30v_recommended_encoded_hard_current_limits;
    size = sizeof(jrk_umc05a_30v_recommended_encoded_hard_current_limits);
  }
  else if (product == JRK_PRODUCT_UMC05A_40V)
  {
    table = jrk_umc05a_40v_recommended_encoded_hard_current_limits;
    size = sizeof(jrk_umc05a_40v_recommended_encoded_hard_current_limits);
  }
  else
  {
    table = NULL;
    size = 0;
  }

  if (count) { *count = size / sizeof(uint16_t); }
  return table;
}

static uint8_t jrk_get_rsense_numerator(uint32_t product)
{
  if (product == JRK_PRODUCT_UMC04A_30V)
  {
    return 1;
  }
  else if (product == JRK_PRODUCT_UMC04A_40V)
  {
    return 2;
  }
  else if (product == JRK_PRODUCT_UMC05A_30V)
  {
    return 5;
  }
  else if (product == JRK_PRODUCT_UMC05A_40V)
  {
    return 5;
  }
  else
  {
    return 1;
  }
}

static uint8_t jrk_get_rsense_denominator(uint32_t product)
{
  if (product == JRK_PRODUCT_UMC05A_30V)
  {
    return 3;
  }
  else if (product == JRK_PRODUCT_UMC05A_40V)
  {
    return 2;
  }
  else
  {
    return 1;
  }
}

// Calculates the measured current in milliamps for a "type 1" board:
// umc04a or umc05a.
//
// This is the same as the calculation that is done in the firmware to produce
// 'current' variable (see jrk_variables_get_current()), except that it does not
// cap the value at 0xFFFF.
//
// raw_current: From jrk_variables_get_raw_current().
// encoded_current_limit: The hardware current limiting configuration, from
//   from jrk_variables_get_max_current().
// rsense_numerator divided by rsense_denominator:
//   Sense resistor resistance, in units of mohms.
// current_offset_calibration: from jrk_settings_get_current_offset_calibration()
// current_scale_calibration: from jrk_settings_get_current_scale_calibration()
static uint32_t jrk_calculate_measured_current_ma_type1(
  uint16_t raw_current,
  uint16_t encoded_current_limit,
  int16_t duty_cycle,
  uint8_t rsense_numerator,
  uint8_t rsense_denominator,
  int16_t current_offset_calibration,
  int16_t current_scale_calibration
  )
{
  if (duty_cycle == 0)
  {
    return 0;
  }

  uint16_t offset = 800 + current_offset_calibration;
  uint16_t scale = 1875 + current_scale_calibration;

  uint8_t dac_ref = encoded_current_limit >> 5 & 3;

  // Convert the reading on the current sense line to units of mV/16.
  uint16_t current = raw_current >> ((2 - dac_ref) & 3);

  // Subtract the 50mV offset voltage, without making the reading negative.
  if (offset > current)
  {
    current = 0;
  }
  else
  {
    current -= offset;
  }

  uint16_t duty_cycle_unsigned;
  if (duty_cycle < 0)
  {
    duty_cycle_unsigned = -duty_cycle;
  }
  else
  {
    duty_cycle_unsigned = duty_cycle;
  }

  // Divide by the duty cycle and apply scaling factors to get the current in
  // milliamps.  The product will be at most 0xFFFF*(2*1875) = 0x0EA5F15A.
  uint32_t current32 = current * scale * rsense_denominator /
    (duty_cycle_unsigned * rsense_numerator);

  return current32;
}

uint32_t jrk_current_limit_decode(
  const jrk_settings * settings, uint16_t encoded_limit)
{
  uint32_t product = jrk_settings_get_product(settings);
  if (!settings || !product) { return 0; }

  if (product == JRK_PRODUCT_UMC04A_30V || product == JRK_PRODUCT_UMC04A_40V ||
    product == JRK_PRODUCT_UMC05A_30V || product == JRK_PRODUCT_UMC05A_40V)
  {
    // These jrks ignore the top 8 bits and treat codes as zero if the top 3
    // bits are invalid.
    encoded_limit &= 0xFF;
    if (encoded_limit > 95) { encoded_limit = 0; }

    return jrk_calculate_measured_current_ma_type1(
      jrk_get_vilim(product, encoded_limit & 0x1F),
      encoded_limit,
      600,
      jrk_get_rsense_numerator(product),
      jrk_get_rsense_denominator(product),
      jrk_settings_get_current_offset_calibration(settings),
      jrk_settings_get_current_scale_calibration(settings)
    );
  }

  return 0;
}

uint16_t jrk_current_limit_encode(const jrk_settings * settings, uint32_t ma)
{
  uint32_t product = jrk_settings_get_product(settings);
  if (product == 0) { return 0; }

  size_t count;
  const uint16_t * table =
    jrk_get_recommended_encoded_hard_current_limits(product, &count);

  // Assumption: The table is in ascending order, so we want to return the last
  // one that is less than or equal to the desired current limit.
  // Assumption: 0 is a valid encoded current limit and a good default to use.

  uint16_t code = 0;
  uint16_t found_ma = 0;

  for (size_t i = 0; i < count; i++)
  {
    uint8_t candidate = table[i];
    uint32_t candidate_ma = jrk_current_limit_decode(settings, candidate);

    if (candidate_ma > ma)
    {
      // The table is in ascending order so we won't find any better matches
      // from now on.
      break;
    }

    // We want to find a current limit less than or equal to the target current.
    // The first part of the if condition takes care of that.  The second part
    // of the if condition ensures that if multiple encoded values map to 0 mA
    // then we will pick the smallest one.
    if (candidate_ma <= ma && candidate_ma > found_ma)
    {
      code = candidate;
      found_ma = candidate_ma;
    }
  }
  return code;
}

uint32_t jrk_calculate_raw_current_mv64(
  const jrk_settings * settings, const jrk_variables * vars)
{
  uint32_t product = jrk_settings_get_product(settings);
  if (!product || !vars) { return 0; }

  if (product == JRK_PRODUCT_UMC04A_30V || product == JRK_PRODUCT_UMC04A_40V ||
    product == JRK_PRODUCT_UMC05A_30V || product == JRK_PRODUCT_UMC05A_40V)
  {
    uint16_t current = jrk_variables_get_raw_current(vars);
    uint8_t dac_ref = jrk_variables_get_encoded_hard_current_limit(vars) >> 5 & 3;
    return current << dac_ref;
  }

  return 0;
}
