#include "bootloader.h"

// TODO: real jrk names in this file

enum PloaderTypeIds
{
  ID_NONE = 0,
  ID_JRK_2017_BOOTLOADER,
  ID_JRK_2017_APP,
};

const std::vector<PloaderAppType> ploaderAppTypes = {
  {
    /* id */ ID_JRK_2017_APP,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B7,
    /* name */ "Pololu Jrk 2017",
    /* composite */ true,
    /* interfaceNumber */ 0,
  },
};

const std::vector<PloaderType> ploaderTypes = {
  {
    /* id */ ID_JRK_2017_BOOTLOADER,
    /* usbVendorId */ 0x1FFB,
    /* usbProductId */ 0x00B6,
    /* name */ "Jrk 2017 Bootloader",
    /* shortName */ "2017",
    /* appAddress */ 0x2000,
    /* appSize */ 0x6000,
    /* writeBlockSize */ 0x40,
    /* erasingFlashAffectsEeprom */ true,
    /* supportsFlashPlainWriting */ false,
    /* supportsFlashReading */ false,
    /* eepromAddress */ 0,
    /* eepromAddressHexFile */ 0xF00000,
    /* eepromSize */ 0x100,
    /* supportsEepromAccess */ true,
    /* deviceCode */ NULL,
    /* matchingAppTypes */ { ID_JRK_2017_APP },
  },
};

// TODO: remove this
const std::vector<PloaderUserType> ploaderUserTypes = {
  {
    /* codeName */ "jrk",
    /* name */ "Jrk Controller",
    /* memberIds */ { ID_JRK_2017_APP, ID_JRK_2017_BOOTLOADER },
  },
};
