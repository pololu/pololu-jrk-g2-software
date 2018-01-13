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
// max_current_code: The hardware current limiting configuration, from
//   from jrk_variables_get_max_current().
// rsense: Sense resistor resistance, in hundredths of a milliohm.
// current_offset_calibration: from jrk_settings_get_current_offset_calibration()
// current_scale_calibration: from jrk_settings_get_current_scale_calibration()
static int32_t jrk_calculate_measured_current_ma_umc04a(
  uint16_t current_reading,
  uint16_t max_current_code,
  uint32_t rsense,
  int16_t current_offset_calibration,
  int16_t current_scale_calibration
  )
{
  // Fix the calibration constants so our calculations don't overflow.
  if (current_offset_calibration < -3200)
  {
    current_offset_calibration = -3200;
  }
  else if (current_offset_calibration > 3200)
  {
    current_offset_calibration = 3200;
  }

  if (current_scale_calibration < -5000)
  {
    current_scale_calibration = -5000;
  }
  else if (current_scale_calibration > 5000)
  {
    current_scale_calibration = 5000;
  }

  // Convert the reading on the current sense line to units of mV/64.
  uint8_t dac_reference_num = (max_current_code >> 5 & 3);
  uint32_t current = current_reading << dac_reference_num;

  // Subtract the 50mV offset voltage, without making the reading negative.
  uint16_t offset = 3200 + current_offset_calibration;
  if (offset > current)
  {
    current = 0;
  }
  else
  {
    current -= offset;
  }

  // Account for the sense resistor and scaling calibration.  The maximum value
  // for the multiplication product is (0xFFFF*4) * (5000 + 5000) = 0x9C3F63C0.
  current = current * (5000 + current_scale_calibration) / rsense;

  // Return the current in units of mA.
  return current >> 6;
}

int32_t jrk_calculate_measured_current_ma(
  const jrk_settings * settings, const jrk_variables * vars,
  bool * trustable)
{
  if (trustable != NULL) { *trustable = 0; }

  if (settings == NULL || vars == NULL) { return 0; }

  uint32_t product = jrk_settings_get_product(settings);

  if (product == JRK_PRODUCT_UMC04A_30V || product == JRK_PRODUCT_UMC04A_40V)
  {
    uint32_t rsense;
    if (product == JRK_PRODUCT_UMC04A_40V)
    {
      rsense = 200;
    }
    else
    {
      rsense = 100;
    }

    if (jrk_variables_get_current_chopping_consecutive_count(vars) == 0)
    {
      if (trustable != NULL) { *trustable = 1; }
    }

    return jrk_calculate_measured_current_ma_umc04a(
      jrk_variables_get_current_high_res(vars),
      jrk_variables_get_max_current(vars),
      rsense,
      jrk_settings_get_current_offset_calibration(settings),
      jrk_settings_get_current_scale_calibration(settings)
    );
  }

  return 0;
}
