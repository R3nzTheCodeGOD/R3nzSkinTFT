#include "Logger.hpp"

#include "imgui/imgui.h"

void Logger::clear() noexcept
{
    this->buffer.clear();
    this->lineOffsets.clear();
    this->lineOffsets.push_back(0);
}

void Logger::addLog(const char* fmt, ...) noexcept
{
    auto old_size{ this->buffer.size() };
    va_list args;
    va_start(args, fmt);
    this->buffer.appendfv(fmt, args);
    va_end(args);
    for (auto new_size{ this->buffer.size() }; old_size < new_size; old_size++)
        if (this->buffer[old_size] == '\n')
            this->lineOffsets.push_back(old_size + 1);
}

void Logger::draw() noexcept
{
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &this->autoScroll);
        ImGui::EndPopup();
    }

    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        this->clear();
    ImGui::SameLine();
    if (ImGui::Button("Copy"))
        ImGui::LogToClipboard();
    ImGui::SameLine();
    this->filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const auto buf{ this->buffer.begin() };
    const auto buf_end{ this->buffer.end() };
    if (this->filter.IsActive()) {
        for (auto line_no{ 0 }; line_no < this->lineOffsets.Size; line_no++) {
            const auto line_start{ buf + this->lineOffsets[line_no] };
            const auto line_end{ (line_no + 1 < this->lineOffsets.Size) ? (buf + this->lineOffsets[line_no + 1] - 1) : buf_end };
            if (this->filter.PassFilter(line_start, line_end))
                ImGui::TextUnformatted(line_start, line_end);
        }
    } else {
        ImGuiListClipper clipper;
        clipper.Begin(this->lineOffsets.Size);
        while (clipper.Step()) {
            for (auto line_no{ clipper.DisplayStart }; line_no < clipper.DisplayEnd; line_no++) {
                const auto line_start{ buf + this->lineOffsets[line_no] };
                const auto line_end{ (line_no + 1 < this->lineOffsets.Size) ? (buf + this->lineOffsets[line_no + 1] - 1) : buf_end };
                ImGui::TextUnformatted(line_start, line_end);
            }
        }
        clipper.End();
    }
    ImGui::PopStyleVar();

    if (this->autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}