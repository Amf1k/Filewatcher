#ifndef ISUBSCRIBER_HPP
#define ISUBSCRIBER_HPP

#include "fileinfo.hpp"

namespace fwlib {
enum class FileAction { Create, Delete, Modify };

class ISubscriber {
 public:
  virtual void notify(FileInfo info, FileAction action) = 0;

  //Понятно дело что можно оператор переопределить, но чет не хочется.
  virtual bool equals(ISubscriber* subscriber) = 0;

 protected:
  ISubscriber() = default;
};
}  // namespace fwlib

#endif  // ISUBSCRIBER_HPP
