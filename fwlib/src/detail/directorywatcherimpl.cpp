#include "detail/directorywatcherimpl.hpp"

#include <algorithm>
#include <filesystem>

fwlib::detail::DirectoryWatcherImpl::DirectoryWatcherImpl()
    : _watchingFlag(true) {}

fwlib::detail::DirectoryWatcherImpl::~DirectoryWatcherImpl() {
  _watchingFlag.store(false);
  for (auto&& [_, future] : _pathWatchers) {
    future.wait();
  }
}

bool fwlib::detail::DirectoryWatcherImpl::addPath(std::string path) {
  std::scoped_lock lock(_mutex);
  if (auto it = _pathWatchers.find(path); it == _pathWatchers.end()) {
    auto [_, result] = _pathWatchers.insert(std::make_pair(
        path, std::async(std::launch::async,
                         &DirectoryWatcherImpl::startWatching, this, path)));
    return result;
  }
  return false;
}

bool fwlib::detail::DirectoryWatcherImpl::removePath(std::string path) {
  std::scoped_lock lock(_mutex);
  if (auto it = _pathWatchers.find(path); it != _pathWatchers.end()) {
    _pathWatchers.erase(it);
    return true;
  }
  return false;
}

std::vector<std::string> fwlib::detail::DirectoryWatcherImpl::paths() const {
  std::vector<std::string> result;
  result.reserve(_pathWatchers.size());
  std::transform(
      _pathWatchers.begin(), _pathWatchers.end(), std::back_inserter(result),
      [](auto&& pair) -> std::string { return std::move(pair.first); });
  return result;
}

void fwlib::detail::DirectoryWatcherImpl::subscribe(
    std::shared_ptr<ISubscriber> subscriber) {
  std::scoped_lock lock(_mutex);
  _subscribers.push_back(subscriber);
}
void fwlib::detail::DirectoryWatcherImpl::unsubscribe(
    std::shared_ptr<ISubscriber> subscriber) {
  std::scoped_lock lock(_mutex);
  auto it = std::find_if(_subscribers.begin(), _subscribers.end(),
                         [subscriber](std::shared_ptr<ISubscriber> sub) {
                           return sub->equals(subscriber.get());
                         });
  if (it != _subscribers.end()) {
    _subscribers.erase(it);
    return;
  }
}
void fwlib::detail::DirectoryWatcherImpl::notify(fwlib::FileInfo info,
                                                 fwlib::FileAction action) {
  //Наврено не самое удачное копирвование, но в целом я думаю идея понятна,
  //как можно быстрее разблокировать мьютекс, сделав копию вектора с
  //подписчиками.
  SubscribersList subscribersToNotify;
  {
    std::scoped_lock lock(_mutex);
    subscribersToNotify.reserve(_subscribers.size());
    std::copy(_subscribers.begin(), _subscribers.end(),
              std::back_inserter(subscribersToNotify));
  }
  for (auto s : subscribersToNotify) {
    s->notify(info, action);
  }
}

std::vector<fwlib::FileInfo> fwlib::detail::DirectoryWatcherImpl::files() {
  //Понятно дело что можно на WinAPI и все такое, но зачем себя мучать, тем
  //более что в задании есть ограничение на слежение за файлами, а не
  //получение списка файлов
  namespace fs = std::filesystem;
  auto pathsForGetFiles = paths();
  std::vector<FileInfo> result;
  for (auto&& pathForGetFile : pathsForGetFiles) {
    auto path = fs::path(pathForGetFile);
    auto de = fs::directory_entry(path);
    for (const auto& file : fs::directory_iterator(de)) {
      if (!file.is_directory()) {
        const auto& filePath = file.path();
        result.emplace_back(FileInfo{filePath.filename().string(),
                                     fs::file_size(filePath),
                                     fs::last_write_time(filePath)});
      }
    }
  }

  return result;
}
