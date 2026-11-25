#ifndef CAN_XML_XML_PARSE_HPP
#define CAN_XML_XML_PARSE_HPP

#include <tinyxml2.h>

#include <vector>

#include "can_xml/common.hpp"

namespace can_xml {

std::vector<can_xml::CANIDMeta> parse_xml(tinyxml2::XMLElement* root);

} // namespace can_xml

#endif // CAN_XML_XML_PARSE_HPP
