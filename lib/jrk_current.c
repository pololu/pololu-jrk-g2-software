#include "jrk_internal.h"

struct jrk_current_setting
{
  // Max current code value as stored in EEPROM.
  uint16_t code;

  // Voltage on the current limit pin in units of 1/64 millivolts.
  uint32_t vilim;
};

struct jrk_current_setting jrk_currents_null[] =
{
  { 0, 0 },
};

// Calculates the measured current in milliamps for a umc04a board.
//
// current_reading: Raw current reading from the device,
//   from jrk_variables_get_curent_high_res().
// current_limit_code: The hardware current limiting configuration, from
//   from jrk_variables_get_max_current().
// rsense: Sense resistor resistance, in units of mohms/16.
// current_offset_calibration: from jrk_settings_get_current_offset_calibration()
// current_scale_calibration: from jrk_settings_get_current_scale_calibration()
static int32_t jrk_calculate_measured_current_ma_umc04a(
  uint16_t current_reading,
  uint16_t current_limit_code,
  int16_t duty_cycle,
  uint8_t rsense,
  int16_t current_offset_calibration,
  int16_t current_scale_calibration
  )
{
  if (duty_cycle == 0)
  {
    return 0;
  }

  // Fix the calibration constants so our calculations don't overflow.
  if (current_offset_calibration < -800)
  {
    current_offset_calibration = -800;
  }
  else if (current_offset_calibration > 800)
  {
    current_offset_calibration = 800;
  }

  if (current_scale_calibration < -1875)
  {
    current_scale_calibration = -1875;
  }
  else if (current_scale_calibration > 1875)
  {
    current_scale_calibration = 1875;
  }

  uint8_t dac_ref = current_limit_code >> 5 & 3;

  // Convert the reading on the current sense line to units of mV/16.
  uint16_t current = current_reading >> ((2 - dac_ref) & 3);

  // Subtract the 50mV offset voltage, without making the reading negative.
  uint16_t offset = 800 + current_offset_calibration;
  if (offset > current)
  {
    current = 0;
  }
  else
  {
    current -= offset;
  }

  // The product will be at most 0xFFFF*(2*1875) = 0x0EA5F15A.
  return current * (1875 + current_scale_calibration) / (duty_cycle * rsense);
}

int32_t jrk_calculate_measured_current_ma(
  const jrk_settings * settings, const jrk_variables * vars)
{
  if (settings == NULL || vars == NULL) { return 0; }

  uint32_t product = jrk_settings_get_product(settings);

  if (product == JRK_PRODUCT_UMC04A_30V || product == JRK_PRODUCT_UMC04A_40V)
  {
    uint8_t rsense;
    if (product == JRK_PRODUCT_UMC04A_40V)
    {
      rsense = 8;  // 2 mOhm
    }
    else
    {
      rsense = 4;  // 1 mOhm
    }

    return jrk_calculate_measured_current_ma_umc04a(
      jrk_variables_get_current_high_res(vars),
      jrk_variables_get_current_limit_code(vars),
      jrk_variables_get_last_duty_cycle(vars),
      rsense,
      jrk_settings_get_current_offset_calibration(settings),
      jrk_settings_get_current_scale_calibration(settings)
    );
  }

  return 0;
}

int32_t jrk_calculate_raw_current_mv64(
  const jrk_settings * settings, const jrk_variables * vars)
{
  if (settings == NULL || vars == NULL) { return 0; }

  uint32_t product = jrk_settings_get_product(settings);

  if (product == JRK_PRODUCT_UMC04A_30V || product == JRK_PRODUCT_UMC04A_40V)
  {
    uint16_t current = jrk_variables_get_current_high_res(vars);
    uint8_t dac_ref = jrk_variables_get_current_limit_code(vars) >> 5 & 3;
    return current << dac_ref;
  }

  return 0;
}
