#ifndef V8EXCEPTION_H_
#define V8EXCEPTION_H_

#include <v8.h>
using namespace v8;

namespace jv8 {

class V8Runner;

class V8Exception : public V8Value {
  Persistent<Value> message;
  Persistent<Value> stackTrace;

public:
  V8Exception (V8Runner* _runner, const TryCatch& tryCatch);

  Handle<Value> getMessage ();
  Handle<Value> getStackTrace ();
  virtual bool isException() { return true; }

  virtual ~V8Exception ();
};

} // namespace jv8

#endif // V8EXCEPTION_H_