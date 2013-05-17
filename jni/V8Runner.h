#ifndef V8RUNNER_H_
#define V8RUNNER_H_
#include <vector>
using namespace std;

#include <jni.h>

#include <v8.h>
#include <v8-debug.h>
using namespace v8;

#include "jv8.h"
namespace jv8 {

class V8Runner {
  Isolate* isolate;
  Persistent<Context> context;
  vector<MappableMethodData*> methodDatas;

public:
  V8Runner();

  Handle<Value> runJS(const char* str);
  void mapMethod (JNIEnv* env,  jobject v8MappableMethod, const char* name);
  void destroy(JNIEnv* env);
  Isolate* getIsolate();
  Handle<Context>& getContext();
};

static Isolate* dbg_isolate;
static Handle<Context> dbg_context;
static bool debuggingInitialized = false;

static void dispatchDebugMessages () {
  Locker l(dbg_isolate);
  Isolate::Scope isolateScope(dbg_isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(dbg_context);

  Debug::ProcessDebugMessages();
}

static void initRemoteDebugging () {
  if (!debuggingInitialized) {
    Debug::SetDebugMessageDispatchHandler(dispatchDebugMessages, true);
    // TODO: Allow specification of port and whether you want to wait for
    //        the debugger to attach.
    Debug::EnableAgent("jv8", 51413, false);
    debuggingInitialized = true;
  }
}

static void setDebuggingRunner (V8Runner* runner) {
  dbg_isolate = runner->getIsolate();

  Locker l(dbg_isolate);
  Isolate::Scope isolateScope(dbg_isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(runner->getContext());

  dbg_context = Persistent<Context>::New(dbg_isolate, runner->getContext());
  initRemoteDebugging();
}

static void disableDebugging () {
  Debug::DisableAgent();
}

} // namespace jv8

#endif // V8RUNNER_H_