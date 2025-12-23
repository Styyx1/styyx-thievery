#pragma once

#include <cstdint>
#include <optional>
#include <string>

// API versioning is useful
constexpr uint32_t THIEVERY_API_VERSION = 1;

struct Thievery
{
    uint32_t version;

    bool (*IsWidgetVisible)();
    void (*SetWidgetVisible)(bool a_visible);
};
// exported function name and signature
extern "C" __declspec(dllexport) Thievery* RequestThieveryAPI();