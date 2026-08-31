#ifndef CORE_REGISTERS_H
#define CORE_REGISTERS_H
#include <stdint.h>
#include <reg_types.h>
#include <reg_spec.h>
#include <core_reg_bits.h>
#include <cstring>  // for strcpy


// R_OPERATION_CTRL bitfields.
#define DUMP_OFFSET (3)
#define MUTE_RPL_OFFSET (4)
#define VISUAL_EN_OFFSET (5)
#define OPLEDEN_OFFSET (6)
#define ALIVE_EN_OFFSET (7)

// RESET_DEV bitfields
#define RST_DEV_OFFSET (0)
#define RST_DFU_OFFSET (5)
#define BOOT_DEF_OFFSET (6)
#define BOOT_EE_OFFSET (7)

/**
 * \brief enum where the name is the name of the register and the
 *        value is the address according to the harp protocol spec.
 */
enum CoreRegName : uint8_t
{
    WHO_AM_I = 0,
    HW_VERSION_H = 1, // major hardware version
    HW_VERSION_L = 2, // minor hardware version
    ASSEMBLY_VERSION = 3,
    HARP_VERSION_H = 4,
    HARP_VERSION_L = 5,
    FW_VERSION_H = 6,
    FW_VERSION_L = 7,
    TIMESTAMP_SECOND = 8,
    TIMESTAMP_MICRO = 9,
    OPERATION_CTRL = 10,
    RESET_DEF = 11,
    DEVICE_NAME = 12,
    SERIAL_NUMBER = 13,
    CLOCK_CONFIG = 14,
    TIMESTAMP_OFFSET = 15,
    UUID = 16,
    TAG = 17,
};

/// Number of core registers.
inline constexpr size_t CORE_REG_COUNT = CoreRegName::TAG - CoreRegName::WHO_AM_I + 1;

// Byte-align struct data so we can send it out serially byte-by-byte.
#pragma pack(push, 1)
struct CoreRegValues
{
    const uint16_t R_WHO_AM_I;
    const uint8_t R_HW_VERSION_H;
    const uint8_t R_HW_VERSION_L;
    const uint8_t R_ASSEMBLY_VERSION;
    const uint8_t R_HARP_VERSION_H;
    const uint8_t R_HARP_VERSION_L ;
    const uint8_t R_FW_VERSION_H;
    const uint8_t R_FW_VERSION_L;
    volatile uint32_t R_TIMESTAMP_SECOND;
    volatile uint16_t R_TIMESTAMP_MICRO;
    volatile uint8_t R_OPERATION_CTRL;
    volatile uint8_t R_RESET_DEF;
    volatile char R_DEVICE_NAME[25];
    volatile uint16_t R_SERIAL_NUMBER;
    volatile uint8_t R_CLOCK_CONFIG;
    volatile uint8_t R_TIMESTAMP_OFFSET;
    volatile uint8_t R_UUID[16];
    uint8_t R_TAG[8];

    // Custom Constructor to initialize strings.
    CoreRegValues(uint16_t who_am_i,
                  uint8_t hw_version_major, uint8_t hw_version_minor,
                  uint8_t assembly_version,
                  uint8_t harp_version_major, uint8_t harp_version_minor,
                  uint8_t fw_version_major, uint8_t fw_version_minor,
                  uint16_t serial_number, const char name[],
                  const uint8_t tag[])
    :R_WHO_AM_I{who_am_i},
     R_HW_VERSION_H{hw_version_major},
     R_HW_VERSION_L{hw_version_minor},
     R_ASSEMBLY_VERSION{assembly_version},
     R_HARP_VERSION_H{harp_version_major},
     R_HARP_VERSION_L{harp_version_minor},
     R_FW_VERSION_H{fw_version_major},
     R_FW_VERSION_L{fw_version_minor},
     R_OPERATION_CTRL{0},
     R_SERIAL_NUMBER{serial_number},
     R_UUID{0} // all zeros.
    {
        strcpy((char*)R_DEVICE_NAME, name);
        strcpy((char*)R_TAG, (char*)tag);
    }

    // Syntactic Sugar. Make bitfields for certain registers easier to access.
    OperationCtrlBits& r_operation_ctrl_bits = *((OperationCtrlBits*)(&R_OPERATION_CTRL));
    ResetDefBits& r_reset_def_bits = *((ResetDefBits*)(&R_RESET_DEF));
    ClockConfigBits& r_clock_config_bits = *((ClockConfigBits*)(&R_CLOCK_CONFIG));
};
#pragma pack(pop)

#endif //CORE_REGISTERS_H
