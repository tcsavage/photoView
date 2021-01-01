#include "OpenFileState.hpp"

#include <algorithm>

void OpenFileState::setOpenFile(const fs::path &path) {
    currentFile = path;
    currentDirectory = path.parent_path();
}

std::optional<fs::path> OpenFileState::next() {
    auto iter = std::find_if(
        fs::directory_iterator(currentDirectory), fs::directory_iterator(),
        [&](const fs::directory_entry &entry) { return entry.path() == currentFile; }
    );
    iter++;
    iter = std::find_if(
        iter, fs::directory_iterator(),
        [&](const fs::directory_entry &entry) { return entry.is_regular_file(); }
    );
    if (iter == fs::directory_iterator()) {
        return std::nullopt;
    }
    return iter->path();
}

std::optional<fs::path> OpenFileState::prev() {
    std::optional<fs::path> last = std::nullopt;
    for (auto&& entry : fs::directory_iterator(currentDirectory)) {
        if (entry.path() == currentFile) {
            break;
        }
        last = entry.path();
    }
    return last;
}
