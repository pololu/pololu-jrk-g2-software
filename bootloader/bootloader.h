#pragma once

#include "firmware_archive.h"
#include <libusbp.hpp>
#include <vector>

typedef std::vector<uint8_t> memory_image;

#define UPLOAD_TYPE_STANDARD 0
#define UPLOAD_TYPE_DEVICE_SPECIFIC 1
#define UPLOAD_TYPE_PLAIN 2

enum memory_set
{
  MEMORY_SET_ALL,
  MEMORY_SET_FLASH,
  MEMORY_SET_EEPROM,
};

// Represents a type of USB device that can be used to start a bootloader.
// Currently, just native USB interfaces are supported, but we could add more
// fields in the future to support USB serial ports and USB HIDs.
class bootloader_app_type
{
public:
  uint32_t id;

  uint16_t usb_vendor_id, usb_product_id;

  const char * name;

  bool composite;

  uint8_t interfaceNumber;

  bool operator ==(const bootloader_app_type & other) const
  {
    return id == other.id;
  }
};

extern const std::vector<bootloader_app_type> bootloader_app_types;

// Represents a specific device connected to the system
// that we could use to start a bootloader.
class bootloader_app_instance
{
public:
  bootloader_app_type type;
  std::string serialNumber;

  bootloader_app_instance()
  {
  }

  bootloader_app_instance(const bootloader_app_type type,
    libusbp::generic_interface gi,
    std::string serialNumber)
    : type(type), serialNumber(serialNumber), usbInterface(gi)
  {
  }

  operator bool() const
  {
    return usbInterface;
  }

  void launchBootloader();

private:
  libusbp::generic_interface usbInterface;
};

// Represents a type of bootloader.
class bootloader_type
{
public:
  uint32_t id;
  uint16_t usb_vendor_id, usb_product_id;

  // A pointer to an ASCII string with the name of the bootloader.
  // Should be the same as the USB product string descriptor.
  const char * name;

  const char * short_name;

  // The address of the first byte of the app (used in the USB protocol).
  uint32_t appAddress;

  // The number of bytes in the app.
  uint32_t appSize;

  // The number of bytes you have to write at once while writing to flash.
  uint16_t writeBlockSize;

  // True if the erase action does something to the EEPROM.
  bool erasingFlashAffectsEeprom;

  // True if you can write plaintext data to flash.
  bool supportsFlashPlainWriting;

  // True if you can read from flash.
  bool supportsFlashReading;

  // The address of the first byte of EEPROM (used in the USB protocol).
  uint32_t eepromAddress;

  // The address used for the first byte of EEPROM in the HEX file.
  uint32_t eepromAddressHexFile;

  // The number of bytes in EEPROM.  Set this to a non-zero number for devices
  // that have EEPROM, even if the bootloader does not support accessing
  // EEPROM.
  uint32_t eepromSize;

  bool supportsEepromAccess;

  const uint8_t * deviceCode;

  std::vector<uint32_t> matchingAppTypes;

  bool memorySetIncludesFlash(memory_set ms) const;
  bool memorySetIncludesEeprom(memory_set ms) const;

  // Raises an exception if reading from the specified memories is not allowed.
  void ensureReading(memory_set ms) const;

  // Raises an exception if erasing the specified memories is not allowed.
  void ensureErasing(memory_set ms) const;

  // Raises an exception if reading and writing from EEPROM is not allowed.
  void ensureEepromAccess() const;

  // Raises an exception if reading from flash is not allowed.
  void ensureFlashReading() const;

  // Raises an exception if writing plain data to flash is not allowed.
  void ensureFlashPlainWriting() const;

  // Returns a vector of the app types that correspond to this bootloader.
  // When trying to write to this bootloader, these are the apps that you
  // should consider restarting.
  std::vector<bootloader_app_type> getMatchingAppTypes() const;

  bool operator ==(const bootloader_type & other) const
  {
    return id == other.id;
  }
};

extern const std::vector<bootloader_type> bootloader_types;

// Represents a specific bootloader connected to the system
// and ready to be used.
class bootloader_instance
{
public:
  bootloader_type type;
  std::string serialNumber;

  bootloader_instance()
  {
  }

  bootloader_instance(const bootloader_type type,
    libusbp::generic_interface gi,
    std::string serialNumber)
    : type(type), serialNumber(serialNumber), usbInterface(gi)
  {
  }

  operator bool()
  {
    return usbInterface;
  }

  std::string get_short_name() const
  {
    return type.short_name;
  }

  std::string get_serial_number() const
  {
    return serialNumber;
  }

  std::string get_os_id() const
  {
    return usbInterface.get_os_id();
  }

  uint16_t get_vendor_id() const
  {
    return type.usb_vendor_id;
  }

  uint16_t get_product_id() const
  {
    return type.usb_product_id;
  }

  libusbp::generic_interface usbInterface;
};

const bootloader_app_type * bootloader_app_type_lookup(
  uint16_t usb_vendor_id, uint16_t usb_product_id);

const bootloader_type * bootloader_type_lookup(
  uint16_t usb_vendor_id, uint16_t usb_product_id);

// Detects all the known apps that are currently connected to the computer.
std::vector<bootloader_app_instance> bootloader_list_apps();

// Detects all the known bootloaders that are currently connected to the
// computer.
std::vector<bootloader_instance> bootloader_list_bootloaders();

class bootloder_status_listener
{
public:
  virtual void set_status(const char * status,
    uint32_t progress, uint32_t maxProgress) = 0;
};

class bootloader_handle
{
public:
  bootloader_handle(bootloader_instance);

  bootloader_handle() { }

  operator bool() const noexcept { return handle; }

  void close()
  {
    *this = bootloader_handle();
  }

  // Sends a request to the bootloader to initialize a particular type of
  // upload.  This is usually required before erasing or writing to flash.
  void initialize(uint16_t uploadType);

  // Calls initialize with some upload type that is known to work.  This is
  // useful for users that don't care about the upload type because they just
  // want to erase the device.
  void initialize();

  // Erases the entire application flash region.
  void eraseFlash();

  // Sends the Restart command, which causes the device to reset.  This is
  // usually used to allow a newly-loaded application to start running.
  void restartDevice();

  // Erases flash and performs any other steps needed to apply the firmware
  // image to the device
  void applyImage(const firmware_archive::image & image);

  bootloader_type type;

  void setStatusListener(bootloder_status_listener * listener)
  {
    this->listener = listener;
  }

private:
  void writeFlashBlock(const uint32_t address, const uint8_t * data, size_t size);
  void writeEepromBlock(const uint32_t address, const uint8_t * data, size_t size);
  void eraseEepromFirstByte();

  void reportError(const libusbp::error & error, std::string context)
    __attribute__((noreturn));

  bootloder_status_listener * listener;

  libusbp::generic_handle handle;
};

namespace bootloader
{
  std::vector<bootloader_instance> list_connected_devices();
}
