#ifndef V8VALUE_H_
#define V8VALUE_H_

#include <v8.h>
using namespace v8;

class V8Runner;

class V8Value {
  V8Runner* runner;
  Persistent<Value> val;

public:
  V8Value (V8Runner* _runner);
  V8Value (V8Runner* _runner, const char* str);
  V8Value (V8Runner* _runner, double num);
  V8Value (V8Runner* _runner, Handle<Value> _val);

  Handle<Value> getValue ();

  ~V8Value ();
};

#endif // V8VALUE_H_