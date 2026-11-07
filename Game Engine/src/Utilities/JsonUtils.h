#pragma once
#include <nlohmann/json.hpp>
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace JsonUtils {

    inline uint64_t GetUint64(const nlohmann::json& jsonData, const std::string& key, uint64_t defaultValue = 0) {
        if (!jsonData.contains(key))
            return defaultValue;

        const auto& value = jsonData[key];

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

    inline uint32_t GetUint32(const nlohmann::json& jsonData, const std::string& key, uint32_t defaultValue = 0) {
        if (!jsonData.contains(key))
            return defaultValue;

        const auto& value = jsonData[key];

        if (value.is_number_unsigned()) {
            return value.get<uint32_t>();
        }
        else if (value.is_number_integer()) {
            // Convert safely if non-negative
            int32_t signedVal = value.get<int32_t>();
            return signedVal >= 0 ? static_cast<uint32_t>(signedVal) : defaultValue;
        }

        return defaultValue;
    }

    inline void WriteVec3(const glm::vec3& value, nlohmann::json& outputStream, const char* label) {
        nlohmann::json vectorValueObject;
        vectorValueObject["x"] = value.x;
        vectorValueObject["y"] = value.y;
        vectorValueObject["z"] = value.z;
        outputStream[label] = vectorValueObject;
    }

    inline glm::vec3 GetVec3(const nlohmann::json& jsonData, const std::string& key, glm::vec3 defaultValue = glm::vec3(0.0f)) {
        if (!jsonData.contains(key))
            return defaultValue;

        const auto& jsonValue = jsonData[key];
        return glm::vec3(jsonValue.value("x", 0.0f), jsonValue.value("y", 0.0f), jsonValue.value("z", 0.0f));
    }

    inline glm::quat GetQuaternion(const nlohmann::json& jsonData, const std::string& key, glm::quat defaultValue = glm::quat()) {
        if (!jsonData.contains(key))
            return defaultValue;

        const auto& jsonValue = jsonData[key];
        return glm::quat(jsonValue.value("w", 0.0f), jsonValue.value("x", 0.0f), jsonValue.value("y", 0.0f), jsonValue.value("z", 0.0f));
    }

    inline void WriteQuaternion(const glm::quat& value, nlohmann::json& outputStream, const char* label) {
        nlohmann::json vectorValueObject;
        vectorValueObject["x"] = value.x;
        vectorValueObject["y"] = value.y;
        vectorValueObject["z"] = value.z;
        vectorValueObject["w"] = value.w;
        outputStream[label] = vectorValueObject;
    }

}
