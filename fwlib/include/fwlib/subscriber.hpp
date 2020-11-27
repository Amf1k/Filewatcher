#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP
#include "isubscriber.hpp"
namespace fwlib {
// C++20 и можно получать нормальные сообщения об ошибках компиляции
// https://monosnap.com/file/zfcB9N6VWwATpQHbsJn4n1gtK8hYYV :D
template <class S>
class Subscriber : public ISubscriber {
 public:
  using Callback = void (S::*)(FileInfo, FileAction);

 public:
  explicit Subscriber(S& object, Callback method)
      : _pObject(&object), _pMethod(method) {}

  void notify(FileInfo info, FileAction action) override {
    if (_pObject) {
      (_pObject->*_pMethod)(std::move(info), action);
    }
  }

  bool equals(ISubscriber* subscriber) override {
    auto* pSubscriber = dynamic_cast<Subscriber*>(subscriber);
    return pSubscriber && pSubscriber->_pObject == _pObject &&
           pSubscriber->_pMethod == _pMethod;
  }

 private:
  S* _pObject;
  Callback _pMethod;
};
}  // namespace fwlib

#endif  // SUBSCRIBER_HPP
