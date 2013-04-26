#include <jni.h>

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "V8Value.h"
#include "V8Exception.h"

#define V8_VALUE_INIT(_runner) {\
  runner = _runner;\
  Locker l(runner->isolate);\
  Isolate::Scope isolateScope(runner->isolate);\
  HandleScope handle_scope;\
  Context::Scope context_scope(runner->context);\
}

namespace jv8 {

V8Exception::V8Exception (V8Runner* _runner, const TryCatch& tryCatch) {
  V8_VALUE_INIT(_runner)
  val = Persistent<Value>::New(runner->isolate, tryCatch.Exception());
  message = Persistent<Value>::New(runner->isolate, tryCatch.Exception());
  stackTrace = Persistent<Value>::New(runner->isolate, tryCatch.StackTrace());
}

V8Exception::~V8Exception () {
  Locker l(runner->isolate);
  Isolate::Scope isolateScope(runner->isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(runner->context);

  message.Dispose(runner->isolate);
  stackTrace.Dispose(runner->isolate);
}

Handle<Value> V8Exception::getMessage () {
  Locker l(runner->isolate);
  Isolate::Scope isolateScope(runner->isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(runner->context);
  return message;
}

Handle<Value> V8Exception::getStackTrace () {
  Locker l(runner->isolate);
  Isolate::Scope isolateScope(runner->isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(runner->context);
  return stackTrace;
}
} // namespace jv8