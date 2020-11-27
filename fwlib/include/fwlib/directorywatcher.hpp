#ifndef FWLIB_DIRECTORYWATCHER_HPP
#define FWLIB_DIRECTORYWATCHER_HPP

#include <memory>
#include <string_view>

#include "idirectorywatcher.hpp"

namespace fwlib {
namespace detail {
class DirectoryWatcherImpl;
}

class DirectoryWatcher : public IDirectoryWatcher {
 public:
  DirectoryWatcher();
  explicit DirectoryWatcher(std::string_view path);
  bool addPath(std::string_view path) override;
  bool removePath(std::string_view path) override;
  std::vector<std::string> paths() const override;
  void subscribe(std::shared_ptr<ISubscriber> subscriber) override;
  void unsubscribe(std::shared_ptr<ISubscriber> subscriber) override;
  std::vector<FileInfo> files() override;

 private:
  // Pimpl, избегаем перекомпиляции при изменении имплементации + скрываем
  // имплемитирующий класс, чтобы нельзя было воспользоваться им снаружи.
  std::shared_ptr<detail::DirectoryWatcherImpl> _pImpl;
};
}  // namespace fwlib
#endif  // FWLIB_DIRECTORYWATCHER_HPP
