#pragma once

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct OpenFileState {
    fs::path currentFile;
    fs::path currentDirectory;

    void setOpenFile(const fs::path &path);

    std::optional<fs::path> next();
    std::optional<fs::path> prev();
};
