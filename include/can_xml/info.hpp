#ifndef CAN_XML_INFO_HPP
#define CAN_XML_INFO_HPP

using can_size_t = uint32_t;

enum class MessageDataType {
    Unspecified,
    Version,
    State,
    Bool,
    UInt32,
    Int32,
    Float32,
};

/// Returns a device id from 0 to 127
constexpr can_size_t device_id_of(const can_size_t id) {
    return (id >> 3) & 0b1111111;
}

/// Returns a sub id from 0 to 7
constexpr can_size_t sub_id_of(const can_size_t id) {
    return id & 0b111;
}

struct CANID {
    can_size_t id;
};

//static bool operator==(can_size_t lhs, const CANID & rhs) { return rhs == lhs; }
//static bool operator!=(can_size_t lhs, const CANID & rhs) { return rhs != lhs; }

#endif // CAN_XML_INFO_HPP
