#ifndef V8VALUE_H_
#define V8VALUE_H_

#include <v8.h>
using namespace v8;

namespace jv8 {

class V8Runner;

class V8Value {
protected:
  V8Runner* runner;
  Persistent<Value> val;
  V8Value () {};

public:
  V8Value (V8Runner* _runner);
  V8Value (V8Runner* _runner, const char* str);
  V8Value (V8Runner* _runner, double num);
  V8Value (V8Runner* _runner, Handle<Value> _val);
  V8Runner* getRunner();
  Handle<Value> getValue ();
  virtual bool isException() { return false; }

  virtual ~V8Value ();
};

} // namespace jv8

#endif // V8VALUE_H_