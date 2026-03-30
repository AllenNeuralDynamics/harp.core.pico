#ifndef REG_SPEC_H
#define REG_SPEC_H
#include <stdint.h>
#include <reg_types.h>
#include <harp_message.h>

using read_reg_fn = void (*)(uint8_t);
using write_reg_fn = void (*)(msg_t& msg);


/**
 * \brief Register Specification.
 */
struct RegSpec
{
    volatile void* const base_ptr;
    const uint8_t num_bytes;
    const reg_type_t payload_type;
    read_reg_fn read_fn_ptr;
    write_reg_fn write_fn_ptr;

    // Default Constructor
    RegSpec()
    :base_ptr{nullptr}, num_bytes{0}, payload_type{reg_type_t::U8},
     read_fn_ptr{nullptr}, write_fn_ptr{nullptr}{}

    // Assignment Operator.
    RegSpec& operator=(const RegSpec& other) = default;

    // Named Constructors
    static RegSpec U8(volatile void* const base_ptr,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint8_t), reg_type_t::U8,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec U8Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::U8,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S8(volatile void* const base_ptr,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(int8_t), reg_type_t::S8,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S8Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::S8,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec U16(volatile void* const base_ptr,
                       read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint16_t), reg_type_t::U16,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec U16Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::U16,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S16(volatile void* const base_ptr,
                       read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint16_t), reg_type_t::S16,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S16Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::S16,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec U32(volatile void* const base_ptr,
                       read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint32_t), reg_type_t::U32,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec U32Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::U32,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S32(volatile void* const base_ptr,
                       read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint32_t), reg_type_t::S32,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec S32Array(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::S32,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec Float(volatile void* const base_ptr,
                         read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, sizeof(uint32_t), reg_type_t::Float,
                    read_fn_ptr, write_fn_ptr);}

    static RegSpec FloatArray(volatile void* const base_ptr, uint8_t num_bytes,
                      read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
    {return RegSpec(base_ptr, num_bytes, reg_type_t::Float,
                    read_fn_ptr, write_fn_ptr);}

/**
 * \brief constructor.
 */
    RegSpec(volatile void* const base_ptr, uint8_t num_bytes,
            reg_type_t payload_type,
            read_reg_fn read_fn_ptr, write_reg_fn write_fn_ptr)
        :base_ptr{base_ptr}, num_bytes{num_bytes}, payload_type{payload_type},
         read_fn_ptr{read_fn_ptr}, write_fn_ptr{write_fn_ptr}{}
};

#endif //REG_SPEC_H
