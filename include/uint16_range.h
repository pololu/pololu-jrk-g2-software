#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <stdint.h>

struct uint16_range
{
  uint16_t max = 0;
  uint16_t min = 0;
  uint16_t average = 0;

  static uint16_range from_samples(const std::vector<uint16_t> & samples)
  {
    assert(samples.size() > 0);

    uint16_range r;

    uint32_t sum = 0;
    r.min = UINT16_MAX;
    r.max = 0;

    for (uint16_t s : samples)
    {
      sum += s;
      if (s < r.min) { r.min = s; }
      if (s > r.max) { r.max = s; }
    }
    r.average = (sum + samples.size() / 2) / samples.size();

    return r;
  }

  // Returns a new range with the specified average, with min and max both
  // equidistant from average, if the limits allow.  The upper_limit is the
  // maximum allowed value for the min, max, and average in the range.
  static uint16_range from_center_and_range(uint16_t center,
    uint16_t range, uint16_t upper_limit)
  {
    uint16_range r;

    if (center > upper_limit) { center = upper_limit; }
    if (range > upper_limit) { range = upper_limit; }

    r.average = center;

    uint16_t half_range = range / 2;

    // Adjust the center so that it is within the inclusive range
    //   [half_range, upper_limit - half_range], so that
    // the calculations for min and max don't overflow.
    if (center < half_range) { center = half_range; }
    if (center > upper_limit - half_range) { center = upper_limit - half_range; }
    assert(center >= half_range && center <= upper_limit - half_range);

    r.min = center - half_range;
    r.max = center + half_range;

    if ((range % 2) == 1)
    {
      // Widen the range by one to correct the rounding error we introduced
      // when we computed half_range above.
      if (r.min > 0) { r.min--; }
      else if (r.max < upper_limit) { r.max++; }
    }

    return r;
  }

  uint16_t range() const
  {
    assert(max >= min);
    return max - min;
  }

  uint16_t distance_to(const uint16_range & other) const
  {
    if (other.min > max)
    {
      return other.min - max;
    }
    else if (min > other.max)
    {
      return min - other.max;
    }
    else
    {
      return 0;
    }
  }

  bool intersects(const uint16_range & other) const
  {
    return distance_to(other) == 0;
  }

  bool is_entirely_above(const uint16_range & other) const
  {
    return min > other.max;
  }

  std::string min_max_string() const
  {
    return std::to_string(min) + "\u2013" + std::to_string(max);
  }
};
