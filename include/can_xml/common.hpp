#ifndef CAN_XML_COMMON_HPP
#define CAN_XML_COMMON_HPP

#include <string>
#include <cstdint>
#include <ostream>

namespace can_xml {

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

struct CANID {
    can_size_t id;
};

struct CANIDMeta {
    CANID id;
    std::string name;
    bool ros_publish;
    MessageDataType data_type;
};

std::ostream& operator<<(std::ostream& stream, const CANID& id);

std::ostream& operator<<(std::ostream& stream, const MessageDataType& data_type);

}

#endif // CAN_XML_COMMON_HPP
