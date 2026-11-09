#ifndef XML_PARSE_HPP
#define XML_PARSE_HPP

#include "can_xml/shared.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>
#include <filesystem>

using tinyxml2::XMLElement;

/// Returns a trimmed substring
std::string trim(const std::string& string) {
    size_t start = 0;
    size_t end = 0;
    // find left side
    for (; string[start] == ' '; start++) {}
    // find right side
    for (end = string.length(); string[end - 1] == ' '; end--) {}

    return string.substr(start, end - start);
}

bool is_valid_topic_and_identifier(const std::string& string) {
    if (string.length() == 0)
        return false;
    // first character
    if (!std::isalpha(string[0]) && !(string[0] == '_'))
        return false;
    bool last_character_was_underscore = string[0] == '_';
    // remaining characters
    for (size_t i = 1; i < string.length(); i++) {
        if (std::isalnum(string[i])) {
            last_character_was_underscore = false;
        } else if (string[i] == '_' && !last_character_was_underscore) {
            last_character_was_underscore = true;
        } else {
            // ROS2 topic names must not contain any number of repeated
            // underscores
            return false;
        }
    }
    return true;
}

const char* read_attribute(const XMLElement* can_id, const char* tag_name,
                           const std::string& closest_id_name) {
    const char* attribute = can_id->Attribute(tag_name);
    if (!attribute) {
        std::cerr << "Missing tag " << tag_name << " near " << closest_id_name
            << std::endl;
        std::abort();
    }
    return attribute;
}

std::string read_ident_attribute(const XMLElement* can_id, const char* tag_name,
                                 const std::string& closest_id_name) {
    const char* text = read_attribute(can_id, tag_name, closest_id_name);

    std::string name = trim(text);
    if (!is_valid_topic_and_identifier(name)) {
        std::cerr << "Tag `" << tag_name << "=\"" << name << "\"` near "
            << closest_id_name << " is not a valid C++ identifier/ROS2 topic"
            << std::endl;
        std::abort();
    }
    return name;
}

bool read_optional_bool_attribute(const XMLElement* can_id, const char* tag_name,
                        bool default_value, const std::string& closest_id_name) {
    const char* text = can_id->Attribute(tag_name);
    if (!text)
        return default_value;
    else if (std::strcmp(text, "true") == 0)
        return true;
    else if (std::strcmp(text, "false") == 0)
        return false;
    else {
        std::cerr << "Tag `" << tag_name << "=\"" << text << "\"` near "
            << closest_id_name << R"( must be "false", "true", or left unspecified)"
            << " which will default to: " << (default_value ? "\"true\"" : "\"false\"")
            << std::endl;
        std::abort();
    }
}

int read_int_attribute(const XMLElement* can_id, const char* tag_name, int bit_width,
                       const std::string& closest_id_name) {
    const char* text = read_attribute(can_id, tag_name, closest_id_name);
    int integer = -1;
    size_t pos = 0;
    try {
        integer = std::stoi(text, &pos);
    } catch (...) {
        // empty catch here because `integer` will remain out of range
        // and surface that it's messed up below
    }

    if (integer >= 0 && integer < 2 << (bit_width - 1) && pos ==
        std::strlen(text)) {
        return integer;
    } else {
        std::cerr << "Tag `" << tag_name << "=\"" << text << "\"` near "
            << closest_id_name << " must be an integer between 0 and "
            << (2 << (bit_width - 1)) - 1 << std::endl;
        std::abort();
    }
}

MessageDataType read_message_type(const XMLElement* can_id, MessageDataType default_type,
                                  const std::string& closest_id_name) {
    const char* text = can_id->Attribute("data-type");
    if (!text)
        return default_type;
    else if (std::strcmp(text, "unspecified") == 0)
        return MessageDataType::Unspecified;
    else if (std::strcmp(text, "version") == 0)
        return MessageDataType::Version;
    else if (std::strcmp(text, "state") == 0)
        return MessageDataType::State;
    else if (std::strcmp(text, "bool") == 0)
        return MessageDataType::Bool;
    else if (std::strcmp(text, "uint32") == 0)
        return MessageDataType::UInt32;
    else if (std::strcmp(text, "int32") == 0)
        return MessageDataType::Int32;
    else if (std::strcmp(text, "float32") == 0)
        return MessageDataType::Float32;
    else {
        std::cerr << "Tag `" << "data-type" << "=\"" << text << "\"` near "
            << closest_id_name
            << " must be one of [unspecified, version, state, bool, uint32, int32, float32]"
            << std::endl;
        std::abort();
    }

}

CANID determine_can_id(unsigned int device_id, unsigned int sub_id) {
    return CANID {(device_id << 3) | sub_id};
}

std::ostream& operator<<(std::ostream& stream, const CANID& id) {
    return stream << "0b"
    << std::bitset<1>(id.id >> 10) << "'"
    << std::bitset<7>(id.id >> 3) << "'"
    << std::bitset<3>(id.id);
}

std::ostream& operator<<(std::ostream& stream, const MessageDataType& data_type) {
    stream << "MessageDataType::";
    switch (data_type) {
        default: return stream << "Unspecified"; break;
        case MessageDataType::Version: return stream << "Version"; break;
        case MessageDataType::State: return stream << "State"; break;
        case MessageDataType::Bool: return stream << "Bool"; break;
        case MessageDataType::UInt32: return stream << "UInt32"; break;
        case MessageDataType::Int32: return stream << "Int32"; break;
        case MessageDataType::Float32: return stream << "Float32"; break;
        }
}

struct CANIDMeta {
    CANID id;
    std::string name;
    bool ros_publish;
    MessageDataType data_type;
};

std::vector<CANIDMeta> parse_xml(XMLElement* root) {
    std::vector<CANIDMeta> can_ids{};

    std::string closest_id_name = "start of document";
    XMLElement* can_id = root->FirstChildElement("id");
    while (can_id) {
        std::string name =
            read_ident_attribute(can_id, "name", closest_id_name);

        // bool no_ros_echo = read_bool_attribute(
        //     can_id, "no-ros-echo", closest_id_name
        // );
        closest_id_name = name;
        int device_id = read_int_attribute(
            can_id, "device-id", 7, closest_id_name
        );
        int sub_id = read_int_attribute(
            can_id, "sub-id", 3, closest_id_name
        );
        bool ros_publish = read_optional_bool_attribute(
            can_id, "ros-publish", true, closest_id_name
        );
        MessageDataType data_type = read_message_type(
            can_id, MessageDataType::Unspecified, closest_id_name
        );

        can_ids.push_back(CANIDMeta {
            .id = determine_can_id(device_id, sub_id),
            .name = name,
            .ros_publish = ros_publish,
            .data_type = data_type,
        });

        can_id = can_id->NextSiblingElement("id");
    }

    return can_ids;
}

#endif // XML_PARSE_HPP
