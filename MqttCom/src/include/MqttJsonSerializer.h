#pragma once

#include "MqttManager.h"
#include <nlohmann/json.hpp>

template <typename T>
concept JsonSerialisable = requires(const T & t, nlohmann::json & j) {
    { to_json(j, t) };
};

template <JsonSerialisable T>
struct MqttSerializer<T> {
    static std::string toString(const T& d) { return nlohmann::json(d).dump(); }
    static T fromString(const std::string& p) { return nlohmann::json::parse(p).get<T>(); }
};