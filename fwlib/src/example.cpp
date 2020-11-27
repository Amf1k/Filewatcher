#include <iostream>

#include "directorywatcher.hpp"
#include "subscriber.hpp"
using namespace fwlib;

class Target {
 public:
  void handler(FileInfo info, FileAction action) {
    std::cout << info.fileName << " " << info.size << " "
              << info.modified.time_since_epoch().count() << " " << std::endl;
  }
};

int main() {
  Target t;
  DirectoryWatcher watcher;
  watcher.addPath("C:/Projects/nsis-scripts");
  auto paths = watcher.paths();
  for (auto path : paths) {
    std::cout << path << std::endl;
  }
  auto files = watcher.files();
  for (auto file : files) {
    std::cout << file.fileName << " " << file.size << std::endl;
  }
  watcher.subscribe(std::make_shared<Subscriber<Target>>(t, &Target::handler));

  while (true) {
  }
  return 0;
};
