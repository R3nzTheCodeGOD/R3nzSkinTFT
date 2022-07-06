#pragma once

#include "imgui/imgui.h"

class Logger {
public:
    inline Logger() noexcept { this->clear(); }

    void clear() noexcept;
    void addLog(const char* fmt, ...) noexcept;
    void draw() noexcept;
private:
    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;
    bool autoScroll{ true };
};
