#ifndef HARP_CORE_H
#define HARP_CORE_H
#include <stdint.h>
#include <harp_message.h>
#include <registers.h>
#include <arm_regs.h>
#include <functional>  // for std::invoke

#include <hardware/structs/timer.h>
#include <hardware/timer.h>
#include <tusb.h>


/**
 * \brief Harp Core that handles management of common bank registers.
*       Implemented as a singleton to simplify attaching interrupts (and since
*       you can only have one per device.
 */
class HarpCore
{
// Make constructor private to prevent creating instances outside of init().
private:
    HarpCore(uint16_t who_am_i, uint16_t hw_version,
             uint8_t assembly_version, uint16_t harp_version,
             uint16_t fw_version);

    ~HarpCore();

public:
    HarpCore() = delete;  // Disable default constructor.
    HarpCore(HarpCore& other) = delete; // Disable copy constructor.
    void operator=(const HarpCore& other) = delete; // Disable assignment operator.

/**
 * \brief initialize the harp core singleton with parameters.
 */
    static HarpCore& init(uint16_t who_am_i, uint16_t hw_version,
                          uint8_t assembly_version, uint16_t harp_version,
                          uint16_t fw_version);

    static HarpCore& core_;
    static HarpCore& instance() {return core_;}

    // Create a typedef so we can create an array of member function ptrs.
    // MsgHandleMemberFn points to a harp core member fn that takes (msg_t&)
    // https://isocpp.org/wiki/faq/pointers-to-members#array-memfnptrs
    typedef void (HarpCore::*RegReadMemberFn)(RegNames reg);
    typedef void (HarpCore::*RegWriteMemberFn)(msg_t& mst);

/**
 * \brief entry point for handling incoming harp messages. Dispatches message
 *      to the appropriate handler.
 */
    void handle_rx_buffer_message();

/**
 * \brief Write message contents to a register by dispatching message
 *      to the appropriate handler. inline.
 */
    void write_to_reg(msg_t& msg)
    {std::invoke(reg_write_fns_[msg.header.address], this, msg);}

/**
 * \brief Write register contents to the tx buffer by dispatching message
 *      to the appropriate handler. inline.
 */
    void read_from_reg(RegNames reg)
    {std::invoke(reg_read_fns_[reg], this, reg);}


/**
 * \brief data is read from serial port into the the rx_buffer.
 */
    uint8_t rx_buffer_[MAX_PACKET_SIZE];  // TODO: should be private.

/**
 * \brief reference to the struct of reg values for easy access.
 */
    RegValues& regs = regs_.regs_;

private:

/**
 * \brief read handler functions. One-per-harp-register where necessary,
 *      but the generic one can be used in most cases.
 *      Note: these all need to have the same function signature.
 */
    void read_timestamp_second(RegNames reg_name);
    void read_timestamp_microsecond(RegNames reg_name);
    void read_reg_generic(RegNames reg_name); // catch-all.

/**
 * \brief a write handler function per harp register. Handles write
 *      operations to that register.
 */
    void write_timestamp_second(msg_t& msg);
    void write_timestamp_microsecond(msg_t& msg);
    void write_operation_ctrl(msg_t& msg);
    void write_reset_def(msg_t& msg);
    void write_device_name(msg_t& msg);
    void write_serial_number(msg_t& msg);
    void write_clock_config(msg_t& msg);
    void write_timestamp_offset(msg_t& msg);

    void write_to_read_only_reg_error(msg_t& msg);

/**
 *  \brief registers.
 */
    Registers regs_;

    // Function Tables. Order matters since we will index into it with enums.
    RegReadMemberFn reg_read_fns_[REG_COUNT] =
    {
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_timestamp_second,
        &HarpCore::read_timestamp_microsecond,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
        &HarpCore::read_reg_generic,
    };

    RegWriteMemberFn reg_write_fns_[REG_COUNT] =
    {&HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_to_read_only_reg_error,
     &HarpCore::write_timestamp_second,
     &HarpCore::write_timestamp_microsecond,
     &HarpCore::write_operation_ctrl,
     &HarpCore::write_reset_def,
     &HarpCore::write_device_name,
     &HarpCore::write_serial_number,
     &HarpCore::write_clock_config,
     &HarpCore::write_timestamp_offset,
    };
};

#endif //HARP_CORE_H