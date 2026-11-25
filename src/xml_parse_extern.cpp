#include "can_xml/xml_parse_extern.hpp"

#include "can_xml/xml_parse.hpp"

#include <tinyxml2.h>
#include <stdexcept>

namespace can_xml {

std::vector<CANIDMeta> parse_xml(const std::filesystem::path& file) {
    tinyxml2::XMLDocument document;
    const auto result = document.LoadFile(file.string().c_str());
    if (result != tinyxml2::XML_SUCCESS)
        throw std::runtime_error("failed to load can id file '" + file.string() + "', " + document.ErrorStr());
    return parse_xml(document.RootElement());
}

} // namespace can_xml
