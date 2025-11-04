#pragma once
#include "json.hpp"
#include <cstdint>
#include <string>

namespace JsonUtils {

    inline uint64_t GetUint64(const nlohmann::json& j, const std::string& key, uint64_t defaultValue = 0) {
        if (!j.contains(key))
            return defaultValue;

        const auto& value = j[key];

        if (value.is_number_unsigned()) {
            return value.get<uint64_t>();
        }
        else if (value.is_number_integer()) {
            // Convert safely if non-negative
            int64_t signedVal = value.get<int64_t>();
            return signedVal >= 0 ? static_cast<uint64_t>(signedVal) : defaultValue;
        }

        return defaultValue;
    }

}
