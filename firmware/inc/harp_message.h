#ifndef HARP_MESSAGE_H
#define HARP_MESSAGE_H
#include <reg_types.h>
#include <utility> // for std::to_underlying

#define MAX_PACKET_SIZE (255) // unused?

enum msg_type_t: uint8_t
{
    ERROR_MASK      = 0x08, ///< OR'd with a base type to form an error reply.
    EXTENDED_LENGTH = 0x10, ///< Flag (bit 4): Length is 32-bit LE; Checksum is CRC-32/ISO-HDLC.

    READ  = 1,
    WRITE = 2,
    EVENT = 3,
    READ_ERROR  = READ  | ERROR_MASK,
    WRITE_ERROR = WRITE | ERROR_MASK,
    BLOB = WRITE | EXTENDED_LENGTH, ///< Extended-length blob WRITE (payload_type = reg_type_t::blob).
};

/// \brief Returns true if the ExtendedLength flag (bit 4) is set in the given type byte.
/// \details When set, the Length field is 32-bit LE and the trailing Checksum is CRC-32/ISO-HDLC.
inline bool is_extended_length(uint8_t type_byte)
{ return bool(type_byte & static_cast<uint8_t>(EXTENDED_LENGTH)); }


// Byte-align struct data so we can either:
// memcopy it to struct or cast the rx buffer to struct
#pragma pack(push, 1)
struct msg_header_t
{
    using enum reg_type_t;

    msg_type_t type;
    uint8_t raw_length;
    uint8_t address;
    uint8_t port; // should default to 255.
    reg_type_t payload_type;

    // (Inline) Member functions:
    bool has_timestamp()
    {return bool(std::to_underlying(payload_type) &
                 std::to_underlying(HAS_TIMESTAMP));}

    uint8_t payload_length()
    {return has_timestamp()? raw_length - 10: raw_length - 4;}

    uint8_t payload_base_index_offset()
    {return has_timestamp()? 11: 5;}

    uint8_t checksum_index_offset()
    {return 2 + raw_length;}

    uint8_t msg_size()
    {return raw_length + 2;}
};
#pragma pack(pop)

// Reference-only convenience classes.
// The data needs to exist elsewhere (i.e: in the RX buffer).
struct msg_t
{
    msg_header_t& header;
    void* payload;  // unknown type until we parse the header.
    uint8_t& checksum;

    // Custom reference-only constructor that refers to data in existing
    // memory locations.
    msg_t(msg_header_t& header, void* payload, uint8_t& checksum)
        :header{header}, payload{payload}, checksum{checksum}
    {}

    // (Inline) Member functions:
    bool has_timestamp()
    {return header.has_timestamp();}

    uint8_t payload_length()
    {return header.payload_length();}
};

struct timestamped_msg_t: public msg_t
{
    uint32_t& timestamp_sec;
    uint16_t& timestamp_usec;

    // Custom reference-only constructor that refers to data in existing
    // memory locations.
    timestamped_msg_t(msg_header_t& header, uint32_t& timestamp_sec,
                      uint16_t& timestamp_usec, void* payload,
                      uint8_t& checksum)
    : msg_t(header, payload, checksum),
    timestamp_sec{timestamp_sec}, timestamp_usec{timestamp_usec}
    {}
};

// Extended-length message header (8 bytes, packed).
// When the ExtendedLength flag (bit 4) is set in the type byte, the Length
// field is 32-bit LE and the trailing Checksum is CRC-32/ISO-HDLC.
//
// The semantics of raw_length are identical to msg_header_t::raw_length:
// it counts bytes after the raw_length field itself, up to and including the
// CRC-32.  For a write request with no timestamp and N payload bytes:
//   raw_length = address(1) + port(1) + payload_type(1) + payload(N) + crc32(4) = N + 7
//
// Layout (PC -> device, write request, no timestamp):
//   offset 0   : type        (1 B) — base type OR'd with EXTENDED_LENGTH flag
//   offset 1-4 : raw_length  (4 B) — 32-bit LE
//   offset 5   : address     (1 B)
//   offset 6   : port        (1 B) — 255
//   offset 7   : payload_type(1 B)
//   offset 8+  : payload     (N B) — streamed, NOT buffered
//   offset 8+N : crc32       (4 B) — CRC-32/ISO-HDLC over all preceding bytes
#pragma pack(push, 1)
struct extended_msg_header_t
{
    using enum reg_type_t;

    msg_type_t type;       ///< Base type OR'd with EXTENDED_LENGTH flag.
    uint32_t   raw_length; ///< 32-bit LE: counts address..crc32 (same semantics as msg_header_t::raw_length).
    uint8_t    address;
    uint8_t    port;       ///< should default to 255.
    reg_type_t payload_type;

    /// \brief True if the payload_type byte has the HAS_TIMESTAMP flag set.
    bool has_timestamp() const
    { return bool(std::to_underlying(payload_type) & std::to_underlying(HAS_TIMESTAMP)); }

    /// \brief Underlying message type with the ExtendedLength flag masked out.
    msg_type_t base_type() const
    { return msg_type_t(uint8_t(type) & ~0x10u); }

    /// \brief Number of payload bytes.
    /// \details raw_length = addr(1)+port(1)+payload_type(1)+[timestamp(6)]+payload(N)+crc32(4)
    uint32_t payload_length() const
    { return raw_length - (has_timestamp() ? 13u : 7u); }

    /// \brief Total extended message size in bytes (type + raw_length field + raw_length contents).
    uint32_t ext_msg_size() const
    { return 5u + raw_length; }
};
#pragma pack(pop)

static_assert(sizeof(extended_msg_header_t) == 8,
              "extended_msg_header_t must be exactly 8 bytes");

// Reference-only convenience struct for an extended-length message.
// Payload is NOT buffered; stream it with HarpCore::copy_ext_chunk().
struct extended_msg_t
{
    extended_msg_header_t& header;

    explicit extended_msg_t(extended_msg_header_t& header) : header{header} {}

    uint32_t payload_length() const { return header.payload_length(); }
};

#endif // HARP_MESSAGE_H
