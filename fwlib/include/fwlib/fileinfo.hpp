#ifndef FILEINFO_HPP
#define FILEINFO_HPP
#include <filesystem>
#include <string>

namespace fwlib {
struct FileInfo {
  std::string fileName;
  std::uintmax_t size;
  std::filesystem::file_time_type modified;

  std::time_t toTimestamp() {
    using namespace std::chrono;
    return system_clock::to_time_t(time_point_cast<system_clock::duration>(
        modified - std::filesystem::file_time_type::clock::now() +
        system_clock::now()));
  }
};

inline bool operator==(const FileInfo& lhs, const FileInfo& rhs) {
  return lhs.fileName == rhs.fileName;
}
}  // namespace fwlib
#endif  // FILEINFO_HPP
