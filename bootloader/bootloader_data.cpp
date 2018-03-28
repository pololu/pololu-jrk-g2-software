#include "bootloader.h"

// TODO: real jrk names in this file

enum bootloader_type_ids
{
  ID_NONE = 0,
  ID_JRK_UMC04A_30V_BOOTLOADER,
  ID_JRK_UMC04A_40V_BOOTLOADER,
  ID_JRK_UMC05A_30V_BOOTLOADER,
  ID_JRK_UMC05A_40V_BOOTLOADER,
};

const std::vector<bootloader_type> bootloader_types = {
  {
    /* id */ ID_JRK_UMC04A_30V_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00B6,
    /* name */ "Jrk G2 18v2x Bootloader",
    /* short_name */ "18v2x",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
  {
    /* id */ ID_JRK_UMC04A_40V_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00B8,
    /* name */ "Jrk G2 24v2x Bootloader",
    /* short_name */ "24x2x",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
  {
    /* id */ ID_JRK_UMC05A_30V_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00BE,
    /* name */ "Jrk G2 18v1x Bootloader",
    /* short_name */ "18v1x",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
  {
    /* id */ ID_JRK_UMC05A_40V_BOOTLOADER,
    /* usb_vendor_id */ 0x1FFB,
    /* usb_product_id */ 0x00C0,
    /* name */ "Jrk G2 24v1x Bootloader",
    /* short_name */ "24x1x",
    /* app_address */ 0x2000,
    /* app_size */ 0x6000,
    /* write_block_size */ 0x40,
    /* eeprom_address */ 0,
    /* eeprom_address_hex_file */ 0xF00000,
    /* eeprom_size */ 0x100,
  },
};
