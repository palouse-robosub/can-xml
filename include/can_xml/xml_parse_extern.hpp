#ifndef CAN_XML_XML_PARSE_EXTERN_HPP
#define CAN_XML_XML_PARSE_EXTERN_HPP

#include "can_xml/common.hpp"

#include <filesystem>
#include <vector>

namespace can_xml {

std::vector<CANIDMeta> parse_xml(const std::filesystem::path& root);

}

#endif // CAN_XML_XML_PARSE_ENTERN_HPP
