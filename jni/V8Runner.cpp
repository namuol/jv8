#include <vector>
using namespace std;

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "V8Value.h"
#include "V8Exception.h"
#include "jv8.h"

namespace jv8 {

V8Runner::V8Runner () {
  isolate = Isolate::New();
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  context = Persistent<Context>(Context::New());
}

V8Value* V8Runner::runJS (const char* js) {
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New(js);

  TryCatch tryCatch;

  // Compile the source code.
  Handle<Script> script = Script::Compile(source, String::New("program"));

  if (script.IsEmpty()) {
    if (!tryCatch.HasCaught()) {
      return NULL;
    }
    return new V8Exception(this, tryCatch);
  }

  // Run the script to get the result.
  Handle<Value> result = script->Run();

  if (result.IsEmpty()) {
    if (!tryCatch.HasCaught()) {
      return NULL;
    }
    return new V8Exception(this, tryCatch);
  }
  
  return new V8Value(this, result);
}

void V8Runner::mapMethod (JNIEnv* env,  jobject v8MappableMethod, const char* name) {
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);
  
  MappableMethodData* data = new MappableMethodData();

  data->methodObject = env->NewGlobalRef(v8MappableMethod);
  data->runner = this;
  env->GetJavaVM(&data->jvm);
  methodDatas.push_back(data);

  //context->DetachGlobal();
  Handle<Object> global = context->Global();
  global->Set(String::New(name), FunctionTemplate::New(&registerCallback, External::New(data))->GetFunction());
  //context->ReattachGlobal(global);
}

void V8Runner::destroy (JNIEnv* env) {
  for (int i=0; i<methodDatas.size(); ++i) {
    env->DeleteGlobalRef(methodDatas[i]->methodObject);
    delete methodDatas[i];
  }
}

Isolate* V8Runner::getIsolate() {
  return isolate;
}

Handle<Context>& V8Runner::getContext() {
  return context;
}

} // namespace jv8