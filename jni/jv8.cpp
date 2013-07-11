#include <jni.h>
#include <string.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "jv8.h"

// TODO: Integrate this in with build system:
//#define NDK_GDB_ENTRYPOINT_WAIT_HACK

namespace jv8 {

//////////////////////////////////////////////////////////////////////////////////////////////
// V8RUNNER CLASS
//////////////////////////////////////////////////////////////////////////////////////////////

#define THROW_V8EXCEPTION(env, tryCatch)\
  if (!tryCatch.HasCaught()) {\
    env->ThrowNew(V8Exception_class, "Unexpected Error running JS");\
  } else {\
    Persistent<Value> ex = Persistent<Value>::New(isolate, tryCatch.StackTrace());\
    env->ThrowNew(V8Exception_class, *String::Utf8Value(ex->ToString()));\
    ex.Dispose(isolate);\
  }\

static void V8Runner_setDebuggingRunner (
  JNIEnv *env,
  jclass V8runner_class,
  jobject jrunner,
  jint port,
  jboolean waitForConnection
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  if (runner != NULL)  {
    setDebuggingRunner(runner, port, waitForConnection);
  } else {
    disableDebugging();
  }
}

static jobject V8Runner_runJS (
  JNIEnv *env,
  jobject jrunner,
  jstring jname,
  jstring jjs
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);

  Isolate* isolate = runner->getIsolate();
  Handle<Context>& context = runner->getContext();
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);

  const char* js = env->GetStringUTFChars(jjs, NULL);
  const char* name = env->GetStringUTFChars(jname, NULL);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New(js);

  env->ReleaseStringUTFChars(jjs, js);

  TryCatch tryCatch;

  // Compile the source code.
  Handle<Script> script = Script::Compile(source, String::New(name));
  env->ReleaseStringUTFChars(jname, name);
  
  Handle<Value> result;

  if (script.IsEmpty()) {
    THROW_V8EXCEPTION(env, tryCatch)
    return NULL;
  }

  // Run the script to get the result.
  result = script->Run();

  if (result.IsEmpty()) {
    THROW_V8EXCEPTION(env, tryCatch)
    return NULL;
  }

  return newV8Value(env, result);
}

static jlong V8Runner_create (
  JNIEnv *env,
  jclass klass
) {
  return (jlong) new V8Runner();
}

static void V8Runner_dispose (
  JNIEnv *env,
  jclass obj
) {
  V8Runner* r = (V8Runner*) env->GetLongField(obj, f_V8Runner_handle);
  if (r) {
    r->destroy(env);
    delete r;
  }
}

static void V8Runner_map (
  JNIEnv *env,
  jobject jrunner,
  jstring jname,
  jobject jmappableMethod
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  const char* name = env->GetStringUTFChars(jname, NULL);
  runner->mapMethod(env, jmappableMethod, name);
  env->ReleaseStringUTFChars(jname, name);
}

static jobject V8Runner_callFunction (
  JNIEnv *env,
  jobject jrunner,
  jobject jfunction,
  jobjectArray jargs
) {

  if (needsToCacheClassData) {
    cacheClassData(env);
  }

  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  Persistent<Value>* functionPersistent = (Persistent<Value>*) env->GetLongField(jfunction, f_V8Function_handle);
  Persistent<Function> function = Persistent<Function>::Cast( *functionPersistent );

  std::vector<Handle<Value> > args;
  int length = env->GetArrayLength(jargs);
  for (int i=0; i<length; i++) {
    jobject obj = env->GetObjectArrayElement(jargs, i);
    Handle<Value> handle = v8ValueFromJObject(env, obj);
    args.push_back(handle);
  }

  Handle<Value> returnedJSValue = runner->callFunction(function, args);
  return newV8Value(env, returnedJSValue);
}

/**
 * Releases the v8 function handler.
 */
static void V8Function_terminate(
  JNIEnv* env,
  jobject jfunction
) {

  if (needsToCacheClassData) {
    cacheClassData(env);
  }

  Persistent<Value>* functionPersistent = (Persistent<Value>*) env->GetLongField(jfunction, f_V8Function_handle);
  if(functionPersistent != NULL){
    Persistent<Function> function = Persistent<Function>::Cast( *functionPersistent );
    function.Dispose();
    function.Clear();
  }
}

} // namespace jv8

static JNINativeMethod V8Runner_Methods[] = {
  {(char*)"create", (char*)"()J", (void *) jv8::V8Runner_create},
  {(char*)"dispose", (char*)"()V", (void *) jv8::V8Runner_dispose},
  {(char*)"runJS", (char*)"(Ljava/lang/String;Ljava/lang/String;)Lcom/jovianware/jv8/V8Value;", (void *) jv8::V8Runner_runJS},
  {(char*)"map", (char*)"(Ljava/lang/String;Lcom/jovianware/jv8/V8MappableMethod;)V", (void *) jv8::V8Runner_map},
  {(char*)"setDebuggingRunner", (char*)"(Lcom/jovianware/jv8/V8Runner;IZ)V", (void *) jv8::V8Runner_setDebuggingRunner},
  {(char*)"callFunction", (char*)"(Lcom/jovianware/jv8/V8Function;[Lcom/jovianware/jv8/V8Value;)Lcom/jovianware/jv8/V8Value;", (void *)jv8::V8Runner_callFunction}
};

static JNINativeMethod V8Function_Methods[] = {
  {(char*)"terminate", (char*)"()V", (void *) jv8::V8Function_terminate}
};

//////////////////////////////////////////////////////////////////////////////////////////////
// GLUE CODE
//////////////////////////////////////////////////////////////////////////////////////////////
// Lookup class, and Handle:
#define ENV_INIT(vm)\
  JNIEnv* env;\
  if ((vm)->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {\
    return -1;\
  }\
  jclass klass;\

#define CLASS(className, globalRef)\
  klass = NULL;\
  klass = (env)->FindClass(className);\
  if (!klass) { return -1; }\
  globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(klass));\

#define MTABLE(method_table)\
  env->RegisterNatives(\
    klass,\
    (method_table),\
    sizeof((method_table)) / sizeof((method_table)[0])\
  );\

#define FIELD(fieldName, signature, field)\
  (field) = env->GetFieldID(klass, (fieldName), (signature));\
  if ((field) == NULL) return -1;\

// Lookup everything else:
#define METHOD(method, signature, methodID)\
  (methodID) = env->GetMethodID(klass, (method), (signature));\

#define CLASS_END()\
  env->DeleteLocalRef(klass);\

// We need to tell the JNI environment to find our method names and properly
//  map them to our C++ methods.
extern "C" {
jint JNI_OnLoad (
  JavaVM* vm,
  void* reserved
) {
  // Sometimes stuff breaks before `ndk-gdb` has a chance to notice.
  // Vigorously spin our wheels until `ndk-gdb` catches us.
  #ifdef NDK_GDB_ENTRYPOINT_WAIT_HACK
  int i=0;
  while(!i);
  #endif
  ENV_INIT(vm)

  CLASS("com/jovianware/jv8/V8Runner", jv8::V8Runner_class)
  MTABLE(V8Runner_Methods)
  FIELD("handle", "J", jv8::f_V8Runner_handle)
  CLASS_END()

  CLASS("com/jovianware/jv8/V8Function", jv8::V8Function_class)
  MTABLE(V8Function_Methods)
  CLASS_END()

  CLASS("com/jovianware/jv8/V8Exception", jv8::V8Exception_class)
  CLASS_END()

  return JNI_VERSION_1_6;
}

} // extern "C"
