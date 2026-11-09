
typedef uint32_t can_size_t;

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

/// Returns an empty string by default
constexpr const char* name_of(const can_size_t id);
    // switch (id) {
    //     case 0b00000001000: return "CurrentState";
    //     case 0b00001100011: return "BarometerAlt";
    //         // ...
    //     default: return "";
    // }

/// ros-publish is enabled by default
constexpr bool ros_publish_of(const can_size_t id);
// switch (id) {
//     case 0b0'0000001'000: return true;
//     case 0b0'0000011'000: return false;
//     // ...
//     default: return true;
// }

/// Unspecified by default
constexpr MessageDataType message_data_type_of(const can_size_t id);
// switch (id) {
//     case 0b0'0000001'000: return MessageDataType::Version;
//     case 0b0'0000011'000: return MessageDataType::Float32;
//     // ...
//     default: return MessageDataType::Unspecified;
// }

struct CANID {
    can_size_t id;
    CANID(can_size_t id) : id(id) { }
    bool operator==(const CANID& rhs) const { return id == rhs.id; }
    bool operator==(const can_size_t& rhs) const { return id == rhs; }
    bool operator!=(const CANID& rhs) const { return id != rhs.id; }
    bool operator!=(const can_size_t& rhs) const { return id != rhs; }
    [[nodiscard]] constexpr can_size_t device_id() const { return device_id_of(id); }
    [[nodiscard]] constexpr can_size_t sub_id() const { return sub_id_of(id); }
    [[nodiscard]] constexpr const char* name() const { return name_of(id); }
    [[nodiscard]] constexpr bool ros_publish() const { return ros_publish_of(id); }
    [[nodiscard]] constexpr MessageDataType message_data_type() const { return message_data_type_of(id); }
};

static bool operator==(can_size_t lhs, const CANID & rhs) { return rhs == lhs; }
static bool operator!=(can_size_t lhs, const CANID & rhs) { return rhs != lhs; }
