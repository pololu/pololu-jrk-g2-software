#include "jrk_internal.h"

// TODO: remmber 20kHz

static void jrk_print_freq(jrk_string * str, int freq_hz)
{
  if (freq_hz >= 1000)
  {
    // TODO: significant digits
    jrk_sprintf(str, "%d\u00A0MHz", freq_hz / 1000000);
  }
  else if (freq_hz >= 10000)
  {
    // TODO: significant digits
    jrk_sprintf(str, "%d\u00A0kHz", freq_hz / 1000);
  }
  else
  {
    jrk_sprintf(str, "%d\u00A0Hz", freq_hz);
  }
}


jrk_error * jrk_summarize_feedback_settings(
  const jrk_settings * settings,
  uint32_t flags,
  char ** diagnosis)
{
  (void)flags;

  if (diagnosis == NULL)
  {
    return jrk_error_create("Diagnosis output pointer is null.");
  }

  *diagnosis = NULL;

  if (settings == NULL)
  {
    return jrk_error_create("Settings object is null.");
  }

  jrk_string str;
  jrk_string_setup(&str);

  uint8_t feedback_mode = jrk_settings_get_feedback_mode(settings);

  uint16_t feedback_error_max = jrk_settings_get_feedback_error_maximum(settings);
  uint16_t feedback_error_min = jrk_settings_get_feedback_error_minimum(settings);
  bool feedback_error = feedback_error_max != 4095 || feedback_error_min != 0;

  uint16_t feedback_max = jrk_settings_get_feedback_maximum(settings);
  uint16_t feedback_min = jrk_settings_get_feedback_minimum(settings);
  bool invert = jrk_settings_get_feedback_invert(settings);
  bool feedback_scaling = invert || feedback_max != 4095 || feedback_min != 0;

  bool wraparound = jrk_settings_get_feedback_wraparound(settings);

  uint8_t fbt_mode = jrk_settings_get_fbt_mode(settings);
  uint8_t fbt_timing_clock = jrk_settings_get_fbt_timing_clock(settings);
  bool fbt_timing_polarity = jrk_settings_get_fbt_timing_polarity(settings);
  uint8_t fbt_timing_timeout = jrk_settings_get_fbt_timing_timeout(settings);
  uint8_t fbt_averaging_count = jrk_settings_get_fbt_averaging_count(settings);
  if (fbt_averaging_count == 0) { fbt_averaging_count = 1; }
  uint8_t fbt_divider_exponent = jrk_settings_get_fbt_divider_exponent(settings);
  uint16_t pid_period = jrk_settings_get_pid_period(settings);

  if (feedback_mode == JRK_FEEDBACK_MODE_NONE)
  {
    jrk_sprintf(&str,
      "The jrk will operate in open-loop mode.  "
      "The \"Duty cycle target\" will simply be the \"Target\" minus 2048, "
      "constrained to be within \u2212600 to 600.  "
      "A target of 2048 maps to a \"Duty cycle target\" of 0, "
      "meaning the motor will stop.  "
      "Targets of 2648 and above map to 600, which is full speed forward.  "
      "Targets of 1448 and below map to \u2212600, which is full speed reverse."
      );
  }

  if (feedback_mode == JRK_FEEDBACK_MODE_ANALOG)
  {
    jrk_sprintf(&str,
      "The jrk will measure analog voltages on the FBA pin, "
      "average together %u samples, "
      "and set \"Feedback\" to a number between 0 and 4095 "
      "where 0 corresponds to 0\u00A0V and 4095 corresponds to "
      "the jrk's logic voltage of approximately 4.8\u00A0V.  "
      "This mode is normally used for position control with an analog potentiomer.",
      1 << jrk_settings_get_feedback_analog_samples_exponent(settings)
      );

    if (jrk_settings_get_feedback_detect_disconnect(settings))
    {
      jrk_sprintf(&str,
        "  The jrk will periodically drive its AUX pin low and expect "
        "the analog reading on FBA to drop.  "
        "If this doesn't happen, it will signal a \"Feedback disconnect\" error.");
    }
  }

  if (feedback_mode == JRK_FEEDBACK_MODE_FREQUENCY)
  {
    if (fbt_mode == JRK_FBT_MODE_PULSE_COUNTING)
    {
      // count = freq_khz * pid_period * fbt_averaging_count >> fbt_divider_exponent

      // Need roughly 50 counts for decent PID.
      int min_counts = 50;
      int min_freq_hz = 1000 * (min_counts << fbt_divider_exponent)
        / pid_period / fbt_averaging_count;

      // The counts get capped at 2047, and there is a limit to how fast
      // the timer can operate too.  6 MHz seems to work.  It will
      // probably work up to about 48 MHz but let's not imply it will.
      int hardware_limit = 6000000;
      int max_counts = 2000;
      int max_freq_hz = 1000 * (max_counts << fbt_divider_exponent)
        / pid_period / fbt_averaging_count;

      if (min_freq_hz < hardware_limit && max_freq_hz < hardware_limit)
      {
        jrk_sprintf(&str,
          "The frequency on FBT should be between ");
        jrk_print_freq(&str, min_freq_hz);
        jrk_sprintf(&str, " and ");
        jrk_print_freq(&str, max_freq_hz);
        jrk_sprintf(&str, ".");
      }
      else if (min_freq_hz < hardware_limit)
      {
        jrk_sprintf(&str,
          "The frequency on FBT should be between ");
        jrk_print_freq(&str, min_freq_hz);
        jrk_sprintf(&str, " and several MHz.");
      }
      else
      {
        jrk_sprintf(&str,
          "To get a decent number of counts for PID (50), the frequency "
          "on FBT would have to be at least ");
        jrk_print_freq(&str, min_freq_hz);
        jrk_sprintf(&str,
          " but that might be too fast for the jrk to measure.");
      }

      if (fbt_averaging_count > 1 && fbt_divider_exponent > 0)
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "counting the rising edges on the FBT pin "
          "during each %u ms PID period, "
          "add together the counts from the last %u periods, "
          "and then divide by %u "
          "to get a frequency measurement capped at 2047.",
          pid_period, fbt_averaging_count, 1 << fbt_divider_exponent);
      }

      if (fbt_averaging_count > 1 && fbt_divider_exponent == 0)
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "counting the rising edges on the FBT pin "
          "during each %u ms PID period "
          "and add together the counts from the last %u periods "
          "to get a frequency measurement capped at 2047.",
          pid_period, fbt_averaging_count);
      }

      if (fbt_averaging_count == 1 && fbt_divider_exponent > 0)
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "counting the rising edges on the FBT pin "
          "during each %u ms PID period "
          "and divide by %u "
          "to get a frequency measurement capped at 2047.",
          pid_period, 1 << fbt_divider_exponent);
      }

      if (fbt_averaging_count == 1 && fbt_divider_exponent == 0)
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "counting the rising edges on the FBT pin "
          "during each %u ms PID period "
          "to get a frequency measurement capped at 2047.",
          pid_period);
      }
    }

    if (fbt_mode == JRK_FBT_MODE_PULSE_TIMING)
    {
      // TODO: need to see if we can actually achieve really slow speeds with
      // low counts and review the logic here

      int k = 0x4000000;
      int slowest_width = 58982;  // 0x10000 * 0.9
      int slowest_freq = k / slowest_width >> fbt_divider_exponent;
      if (slowest_freq < 50)
      {
        slowest_freq = 50;
        slowest_width = k / slowest_freq >> fbt_divider_exponent;
      }

      int fastest_freq = 1843;  // 2048*0.9
      int fastest_width = k / fastest_freq >> fbt_divider_exponent;
      if (fastest_width < 50)
      {
        fastest_width = 50;
        fastest_freq = k / fastest_width >> fbt_divider_exponent;
      }

      int clock_hz = 1;
      switch (fbt_timing_clock)
      {
      case JRK_FBT_TIMING_CLOCK_48:  clock_hz = 48000000; break;
      case JRK_FBT_TIMING_CLOCK_24:  clock_hz = 24000000; break;
      case JRK_FBT_TIMING_CLOCK_12:  clock_hz = 12000000; break;
      case JRK_FBT_TIMING_CLOCK_6:   clock_hz = 6000000;  break;
      case JRK_FBT_TIMING_CLOCK_3:   clock_hz = 3000000;  break;
      case JRK_FBT_TIMING_CLOCK_1_5: clock_hz = 1500000;  break;
      }

      int min_freq_hz = clock_hz / (slowest_width * 2);
      int max_freq_hz = clock_hz / (fastest_width * 2);

      int timeout_hz = fbt_timing_timeout ? 1000 / fbt_timing_timeout : 10000000;

      if (max_freq_hz <= timeout_hz)
      {
        max_freq_hz = timeout_hz;
        jrk_sprintf(&str,
          "The pulse timing timeout is too low; try changing it to 100 ms.");
      }
      else if (min_freq_hz < max_freq_hz)
      {
        bool limited_by_timeout = false;
        if (min_freq_hz < timeout_hz)
        {
          min_freq_hz = timeout_hz;
          limited_by_timeout = true;
        }

        jrk_sprintf(&str,
          "The frequency on FBT should be between ");
        jrk_print_freq(&str, min_freq_hz);
        if (limited_by_timeout)
        {
          jrk_sprintf(&str, " (limited by the %u\u00A0ms timeout)",
            fbt_timing_timeout);
        }
        jrk_sprintf(&str, " and ");
        jrk_print_freq(&str, max_freq_hz);
        jrk_sprintf(&str, ".");
      }
      else if (fbt_divider_exponent >= 15)
      {
        jrk_sprintf(&str,
          "The pulse divider is too high; try changing it to 32.");
      }
      else
      {
        assert(0);  // Not reachable.
      }

      const char * clock_str = "unknown";
      jrk_code_to_name(jrk_fbt_timing_clock_names_no_units,
        fbt_timing_clock, &clock_str);

      if (fbt_divider_exponent > 0)
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "timing %s pulses on the FBT pin "
          "with a %s\u00A0MHz, 16-bit timer, "
          "calculating 67,108,864 divided by the time, "
          "and then dividing by %u "
          "to get a frequency measurement capped at 2047.",
          fbt_timing_polarity ? "low" : "high",
          clock_str,
          1 << fbt_divider_exponent);
      }
      else
      {
        jrk_sprintf(&str,
          "  The jrk will measure the speed of the motor by "
          "timing %s pulses on the FBT pin "
          "with a %s\u00A0MHz, 16-bit timer "
          "and calculating 67,108,864 divided by the time, "
          "to get a frequency measurement capped at 2047.",
          fbt_timing_polarity ? "low" : "high",
          clock_str);
      }
    }

    //jrk_sprintf(&str,
    //  "  A \"Target\" value of 2048 represents a speed of 0. "
    //  "The \"Feedback\" value will either be 2048 plus the frequency measurement "
    //  "or 2048 minus the frequency measurement, depending on the \"Target\" value.");
  }

  if (feedback_error && feedback_mode != JRK_FEEDBACK_MODE_NONE)
  {
    jrk_sprintf(&str,
      "  Feedback values");
    if (feedback_error_min != 0)
    {
      jrk_sprintf(&str, " less than %u", feedback_error_min);
    }
    if (feedback_error_max != 4095)
    {
      if (feedback_error_min != 0)
      {
        jrk_sprintf(&str, " or");
      }
      jrk_sprintf(&str, " greater than %u", feedback_error_max);
    }
    jrk_sprintf(&str, " will trigger a \"Feedback disconnect\" error");
  }

  if (invert && feedback_mode == JRK_FEEDBACK_MODE_FREQUENCY)
  {
    jrk_sprintf(&str,
      "  The feedback direction is inverted, which will almost certainly "
      "cause problems in this mode.");
  }
  else if (feedback_mode != JRK_FEEDBACK_MODE_NONE)
  {
    if (feedback_scaling)
    {
      jrk_sprintf(&str,
        "  A \"Feedback\" value of %u maps to a "
        "\"Scaled feedback\" value of %u, and "
        "a \"Feedback\" value of %u maps to a "
        "\"Scaled feedback\" value of %u.",
        feedback_max, invert ? 0 : 4095,
        feedback_min, invert ? 4095 : 0
        );
    }
    else
    {
      //jrk_sprintf(&str,
      //  "  The \"Scaled feedback\" will equal the \"Feedback\".");
    }
  }

  if (wraparound && feedback_mode == JRK_FEEDBACK_MODE_ANALOG)
  {
    jrk_sprintf(&str,
      "  Wraparound mode is enabled, so a scaled feedback value of 0 "
      "is considered to be adjacent to 4095.");
  }

  if (str.data == NULL)
  {
    return &jrk_error_no_memory;
  }
  else
  {
    *diagnosis = str.data;
    return NULL;
  }
}
