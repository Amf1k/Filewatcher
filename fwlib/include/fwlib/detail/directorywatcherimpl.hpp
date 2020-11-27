#ifndef DIRECTORYWATCHERIMPL_HPP
#define DIRECTORYWATCHERIMPL_HPP

#include <future>
#include <mutex>
#include <string_view>
#include <vector>

#include "isubscriber.hpp"

struct WatchInfo;

namespace fwlib::detail {
class DirectoryWatcherImpl {
 public:
  DirectoryWatcherImpl();
  virtual ~DirectoryWatcherImpl();
  bool addPath(std::string path);
  bool removePath(std::string path);
  std::vector<std::string> paths() const;
  void subscribe(std::shared_ptr<ISubscriber> subscriber);
  void unsubscribe(std::shared_ptr<ISubscriber> subscriber);
  std::vector<FileInfo> files();
  void notify(FileInfo info, FileAction action);

 private:
  void startWatching(std::string_view path);

 private:
  using SubscribersList = std::vector<std::shared_ptr<ISubscriber>>;
  SubscribersList _subscribers;
  std::unordered_map<std::string, std::future<void>> _pathWatchers;
  std::mutex _mutex;
  std::atomic_bool _watchingFlag;
};
}  // namespace fwlib::detail

#endif  // DIRECTORYWATCHERIMPL_HPP
