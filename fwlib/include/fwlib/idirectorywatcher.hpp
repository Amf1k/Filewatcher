#ifndef IDIRECTORYWATCHER_HPP
#define IDIRECTORYWATCHER_HPP

#include <vector>

#include "fileinfo.hpp"
#include "isubscriber.hpp"

namespace fwlib {
class IDirectoryWatcher {
 public:
  virtual bool addPath(std::string_view path) = 0;
  virtual bool removePath(std::string_view path) = 0;
  virtual std::vector<std::string> paths() const = 0;
  virtual void subscribe(std::shared_ptr<ISubscriber> subscriber) = 0;
  virtual void unsubscribe(std::shared_ptr<ISubscriber> subscriber) = 0;
  virtual std::vector<FileInfo> files() = 0;

 protected:
  IDirectoryWatcher() = default;
};
}  // namespace fwlib

#endif  // IDIRECTORYWATCHER_HPP
