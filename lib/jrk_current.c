#include "jrk_internal.h"

struct jrk_current_setting
{
  // Current value as stored in the jrk's EEPROM.
  uint8_t raw;

  // Current value in milliamps.
  uint32_t ma;
};

struct jrk_current_setting jrk_currents_null[] =
{
  { 0, 0 },
};

// Generated with the ruby/generate_current_tables.rb script.
struct jrk_current_setting jrk_currents_2017[] =
{
  { 124, 32234 },
  { 120, 31167 },
  { 116, 30100 },
  { 112, 29034 },
  { 108, 27967 },
  { 104, 26900 },
  { 100, 25834 },
  {  96, 24767 },
  {  92, 23700 },
  {  88, 22634 },
  {  84, 21567 },
  {  80, 20500 },
  {  76, 19434 },
  {  72, 18367 },
  {  68, 17300 },
  {  64, 16234 },
  {  62, 15700 },
  {  60, 15167 },
  {  58, 14634 },
  {  56, 14100 },
  {  54, 13567 },
  {  52, 13034 },
  {  50, 12500 },
  {  48, 11967 },
  {  46, 11434 },
  {  44, 10900 },
  {  42, 10367 },
  {  40,  9834 },
  {  38,  9300 },
  {  36,  8767 },
  {  34,  8234 },
  {  32,  7700 },
  {  31,  7434 },
  {  30,  7167 },
  {  29,  6900 },
  {  28,  6634 },
  {  27,  6367 },
  {  26,  6100 },
  {  25,  5834 },
  {  24,  5567 },
  {  23,  5300 },
  {  22,  5034 },
  {  21,  4767 },
  {  20,  4500 },
  {  19,  4234 },
  {  18,  3967 },
  {  17,  3700 },
  {  16,  3434 },
  {  15,  3167 },
  {  14,  2900 },
  {  13,  2634 },
  {  12,  2367 },
  {  11,  2100 },
  {  10,  1834 },
  {   9,  1567 },
  {   8,  1300 },
  {   7,  1034 },
  {   6,   767 },
  {   5,   500 },
  {   4,   234 },
  {   3,     3 },
  {   2,     2 },
  {   1,     1 },
  {   0,     0 },
};

static struct jrk_current_setting * jrk_current_table(uint32_t product)
{
  switch (product)
  {
  case JRK_PRODUCT_2017:
    return jrk_currents_2017;
  default:
    return jrk_currents_null;
  }
}

uint32_t jrk_current_limit_raw_to_ma(uint32_t product, uint32_t raw)
{
  struct jrk_current_setting * p = jrk_current_table(product);
  while (p->raw > raw) { p++; }
  return p->ma;
}

uint32_t jrk_current_limit_ma_to_raw(uint32_t product, uint32_t ma)
{
  struct jrk_current_setting * p = jrk_current_table(product);
  while (p->ma > ma) { p++; }
  return p->raw;
}

uint32_t jrk_achievable_current_limit(uint32_t product, uint32_t ma)
{
  struct jrk_current_setting * p = jrk_current_table(product);
  while (p->ma > ma) { p++; }
  return p->ma;
}
