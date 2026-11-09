#include <tinyxml2.h>

#include "can_xml/xml_parse.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>
#include <filesystem>

// Generation of guppy_can.hpp
// SHARED_CAN_CODE gets replaced by CMake with the contents of shared.hpp
void generate_header(std::vector<CANIDMeta>& can_ids) {
    std::filesystem::create_directory("generated");
    std::ofstream header("generated/guppy_can.hpp");

    header << R"(/// GENERATED -- Do not edit directly
/// CAN IDs are specified in can-ids.xml
#ifndef GUPPY_CAN_HPP
#define GUPPY_CAN_HPP
#include <cstdint>

namespace guppy_can {
@SHARED_CAN_CODE@
)";
    // constexpr's
    for (const CANIDMeta& meta : can_ids) {
        header << "static const CANID " << meta.name << " = { " << meta.id << " };\n";
    }
    // name_of
    header << "\nconstexpr const char* name_of(const can_size_t id) {\n"
    << "    switch (id) {\n";
    for (const CANIDMeta& meta : can_ids) {
        header << "        case " << meta.id << ": return " << '"' << meta.name << '"' << ";\n";
    }
    header << "        default: return \"\";\n";
    header << "    }\n}\n";
    // ros_publish_of
    header << "\nconstexpr bool ros_publish_of(const can_size_t id) {\n";
    header << "    switch (id) {\n";
    for (const CANIDMeta& meta : can_ids) {
        header << "        case " << meta.id << ": return " << (meta.ros_publish ? "true" : "false") << ";\n";
    }
    header << "        default: return true;\n";
    header << "    }\n}\n";
    // message_data_type_of
    header << "\nconstexpr MessageDataType message_data_type_of(const can_size_t id) {\n";
    header << "    switch (id) {\n";
    for (const CANIDMeta& meta : can_ids) {
        header << "        case " << meta.id << ": return " << meta.data_type << ";\n";
    }
    header << "        default: return MessageDataType::Unspecified;\n";
    header << "    }\n}\n";
    header << R"(
} // namespace guppy_can

#endif // GUPPY_CAN_HPP)";
}

int main() {
    tinyxml2::XMLDocument doc;
    doc.Parse(R"(@CAN_IDS_XML@)");

    XMLElement* root = doc.RootElement();
    // if (!root) {
    //     std::cerr << "Could not find " << path << " in working directory"
    //         << " or XML is invalid" << std::endl;
    //     // Possible improvement: show canonical path it searched here
    //     //                       remember to handle possible errors on that
    //     std::abort();
    // }

    auto can_ids = parse_xml(root);

    generate_header(can_ids);

    std::cout << "Processing finished successfully" << std::endl;
    return 0;
}
