#include "can_xml/common.hpp"

#include <bitset>

namespace can_xml {

std::ostream& operator<<(std::ostream& stream, const CANID& id) {
    return stream << "0b"
        << std::bitset<1>(id.id >> 10) << "'"
        << std::bitset<7>(id.id >> 3) << "'"
        << std::bitset<3>(id.id);
}


std::ostream& operator<<(std::ostream& stream, const MessageDataType& data_type) {
    stream << "MessageDataType::";
    switch (data_type) {
    case MessageDataType::Version: return stream << "Version";     break;
    case MessageDataType::State:   return stream << "State";       break;
    case MessageDataType::Bool:    return stream << "Bool";        break;
    case MessageDataType::UInt32:  return stream << "UInt32";      break;
    case MessageDataType::Int32:   return stream << "Int32";       break;
    case MessageDataType::Float32: return stream << "Float32";     break;
    default:                       return stream << "Unspecified"; break;
    }
}

} // namespace can_xml
