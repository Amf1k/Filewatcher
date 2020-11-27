#include "directorywatcher.hpp"

#include "detail/directorywatcherimpl.hpp"

using namespace fwlib::detail;

void fwlib::DirectoryWatcher::subscribe(
    std::shared_ptr<ISubscriber> subscriber) {
  _pImpl->subscribe(subscriber);
}
void fwlib::DirectoryWatcher::unsubscribe(
    std::shared_ptr<ISubscriber> subscriber) {
  _pImpl->unsubscribe(subscriber);
}

std::vector<fwlib::FileInfo> fwlib::DirectoryWatcher::files() {
  return _pImpl->files();
}

fwlib::DirectoryWatcher::DirectoryWatcher() : DirectoryWatcher("") {}

fwlib::DirectoryWatcher::DirectoryWatcher(std::string_view path)
    : _pImpl(std::make_shared<DirectoryWatcherImpl>()) {
  if (!path.empty()) {
    _pImpl->addPath(path.data());
  }
}

bool fwlib::DirectoryWatcher::addPath(std::string_view path) {
  return _pImpl->addPath(path.data());
}

bool fwlib::DirectoryWatcher::removePath(std::string_view path) {
  return _pImpl->removePath(path.data());
}

std::vector<std::string> fwlib::DirectoryWatcher::paths() const {
  return _pImpl->paths();
}
