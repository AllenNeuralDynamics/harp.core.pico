#include <harp_c_app.h>

HarpCApp& HarpCApp::init(uint16_t who_am_i,
                         uint8_t hw_version_major, uint8_t hw_version_minor,
                         uint8_t assembly_version,
                         uint8_t fw_version_major, uint8_t fw_version_minor,
                         uint16_t serial_number, const char name[],
                         const uint8_t tag[],
                         RegSpec* app_reg_specs, size_t app_reg_count,
                         void (* update_fn)(void), void (* reset_fn)(void))
{
    static HarpCApp app(who_am_i, hw_version_major, hw_version_minor,
                        assembly_version,
                        fw_version_major, fw_version_minor, serial_number,
                        name, tag, app_reg_specs, app_reg_count, update_fn,
                        reset_fn);
    return app;
}

HarpCApp::HarpCApp(uint16_t who_am_i,
                   uint8_t hw_version_major, uint8_t hw_version_minor,
                   uint8_t assembly_version,
                   uint8_t fw_version_major, uint8_t fw_version_minor,
                   uint16_t serial_number, const char name[],
                   const uint8_t tag[],
                   RegSpec* app_reg_specs, size_t app_reg_count,
                   void (*update_fn)(void), void (* reset_fn)(void))
:app_reg_specs_{app_reg_specs},
 app_reg_count_{app_reg_count},
 update_fn_{update_fn},
 reset_fn_{reset_fn},
 HarpCore(who_am_i, hw_version_major, hw_version_minor, assembly_version,
          fw_version_major, fw_version_minor, serial_number, name, tag)
{
    // Call base class constructor.
    // Create a ptr to the first (and only) derived class instance created.
    if (self == nullptr)
        self = this;
}

HarpCApp::~HarpCApp(){self = nullptr;}

void HarpCApp::handle_buffered_app_message()
{
    msg_t msg = get_buffered_msg();
    // Ignore out-of-range msgs.
    if (msg.header.address < APP_REG_START_ADDRESS ||
        msg.header.address >= (APP_REG_START_ADDRESS + app_reg_count_))
        return;
    uint8_t app_reg_address = msg.header.address - APP_REG_START_ADDRESS;
    switch (msg.header.type)
    {
        // Note: handler functions take the full address, but they live in
        // pairs in a separate struct indexed by app register address.
        case READ:
            app_reg_specs_[app_reg_address].read_fn_ptr(msg.header.address);
            break;
        case WRITE:
            reinterpret_cast<write_reg_fn>(app_reg_specs_[app_reg_address].write_fn_ptr)(msg);
            break;
        default:
        {
            break;
        }
    }
    clear_msg();
}

void HarpCApp::dump_app_registers()
{
    for (uint8_t address = APP_REG_START_ADDRESS;
         address < app_reg_count_ + APP_REG_START_ADDRESS; ++address)
    {
        const RegSpec& spec = reg_address_to_spec(address);
        // Extended-length (blob) registers are excluded from DUMP by default.
        if (spec.payload_type == reg_type_t::Blob)
            continue;
        spec.read_fn_ptr(address);
    }
}

void HarpCApp::handle_buffered_ext_app_message()
{
    extended_msg_header_t& header = get_buffered_ext_msg_header();
    extended_msg_t msg{header};
    // Only app registers are extended-length-capable in this implementation.
    if (header.address < APP_REG_START_ADDRESS ||
        header.address >= (APP_REG_START_ADDRESS + app_reg_count_))
    {
        drain_ext_payload(msg);
        // WRITE_ERROR for an extended-length write carries U32 0x00000000.
        constexpr uint32_t err_payload = 0;
        send_harp_reply(WRITE_ERROR, header.address,
                        &err_payload, sizeof(err_payload), reg_type_t::U32);
        clear_msg();
        return;
    }
    const uint8_t app_reg_index = header.address - APP_REG_START_ADDRESS;
    switch (header.base_type())
    {
        case WRITE:
        {
            const RegSpec& spec = app_reg_specs_[app_reg_index];
            write_ext_reg_fn fn = (spec.payload_type == reg_type_t::Blob)
                ? reinterpret_cast<write_ext_reg_fn>(spec.write_fn_ptr)
                : nullptr;
            if (fn == nullptr)
            {
                drain_ext_payload(msg);
                // WRITE_ERROR for an extended-length write carries U32 0x00000000.
                constexpr uint32_t err_payload = 0;
                send_harp_reply(WRITE_ERROR, header.address,
                                &err_payload, sizeof(err_payload), reg_type_t::U32);
            }
            else
            {
                fn(msg);
            }
            break;
        }
        case READ:
            // Rejected: READ requests MUST NOT set the ExtendedLength flag.
            // Drain any trailing CRC bytes to keep the CDC stream aligned.
            drain_ext_payload(msg);
            send_harp_reply(READ_ERROR, header.address, nullptr, 0,
                            header.payload_type);
            break;
        default:
            break;
    }
    clear_msg();
}
