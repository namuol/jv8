#include <jni.h>

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "V8Value.h"

#define V8_VALUE_INIT(_runner) {\
  runner = _runner;\
  Locker l(runner->isolate);\
  Isolate::Scope isolateScope(runner->isolate);\
  HandleScope handle_scope;\
  Context::Scope context_scope(runner->context);\
}

V8Value::V8Value (V8Runner* _runner) {
  V8_VALUE_INIT(_runner)
  val = Persistent<Value>::New(runner->isolate, Undefined());
}

V8Value::V8Value (V8Runner* _runner, const char* str) {
  V8_VALUE_INIT(_runner)
  val = Persistent<Value>::New(runner->isolate, String::New(str));
}

V8Value::V8Value (V8Runner* _runner, double num) {
  V8_VALUE_INIT(_runner)
  val = Persistent<Value>::New(runner->isolate, Number::New(num));
}

V8Value::V8Value (V8Runner* _runner, Handle<Value> _val) {
  V8_VALUE_INIT(_runner)
  val = Persistent<Value>::New(runner->isolate, _val);
}

V8Value::~V8Value () {
  val.Dispose(runner->isolate);
}

Handle<Value> V8Value::getValue() {
  Locker l(runner->isolate);
  Isolate::Scope isolateScope(runner->isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(runner->context);
  return val;
}