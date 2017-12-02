#ifndef _JRK_PROTOCOL_H
#define _JRK_PROTOCOL_H

#define JRK_VENDOR_ID 0x1FFB
// TODO: use a more real product name here when we decide it
#define JRK_PRODUCT_ID_2017 0x00B7

#define JRK_MAX_USB_RESPONSE_SIZE 128

#define JRK_CMD_REINITIALIZE 0x10
#define JRK_CMD_SET_SETTING 0x13
#define JRK_CMD_GET_DEBUG_DATA 0x20
#define JRK_CMD_GET_VARIABLE_SERIAL 0x80
#define JRK_CMD_GET_ERROR_FLAGS_HALTING_SERIAL 0xB3
#define JRK_CMD_GET_ERROR_FLAGS_OCCURRED_SERIAL 0xB5
#define JRK_CMD_SET_TARGET_USB 0x84
#define JRK_CMD_MOTOR_OFF_USB 0x87
#define JRK_CMD_SET_TARGET_SERIAL 0xC0
#define JRK_CMD_SET_TARGET_LOW_RES_FWD 0xE0
#define JRK_CMD_SET_TARGET_LOW_RES_REV 0xE1
#define JRK_CMD_GET_SETTING 0xE3
#define JRK_CMD_GET_VARIABLES 0xE5
#define JRK_CMD_TEMP_SET_VARIABLE 0xE6
#define JRK_CMD_OVERRIDE_DUTY_CYCLE 0xE9
#define JRK_CMD_MOTOR_OFF_SERIAL 0xFF
#define JRK_CMD_START_BOOTLOADER 0xFF

#define JRK_RESET_POWER_UP 0
#define JRK_RESET_BROWNOUT 1
#define JRK_RESET_RESET_LINE 2
#define JRK_RESET_WATCHDOG 4
#define JRK_RESET_SOFTWARE 8
#define JRK_RESET_STACK_OVERFLOW 16
#define JRK_RESET_STACK_UNDERFLOW 32

#endif
