#include <jni.h>
#include <string.h>

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

static jobject V8Runner_runJS (
  JNIEnv *env,
  jobject jrunner,
  jstring jstr
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);

  Isolate* isolate = runner->getIsolate();
  Handle<Context>& context = runner->getContext();
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);

  const char* js = env->GetStringUTFChars(jstr, NULL);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New(js);

  env->ReleaseStringUTFChars(jstr, js);

  TryCatch tryCatch;

  // Compile the source code.
  Handle<Script> script = Script::Compile(source, String::New("program"));
  
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

} // namespace jv8

static JNINativeMethod V8Runner_Methods[] = {
  {"create", "()J", (void *) jv8::V8Runner_create},
  {"dispose", "()V", (void *) jv8::V8Runner_dispose},
  {"runJS", "(Ljava/lang/String;)Lcom/jovianware/jv8/V8Value;", (void *) jv8::V8Runner_runJS},
  {"map", "(Ljava/lang/String;Lcom/jovianware/jv8/V8MappableMethod;)V", (void *) jv8::V8Runner_map},
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

  CLASS("com/jovianware/jv8/V8Exception", jv8::V8Exception_class)
  CLASS_END()

  return JNI_VERSION_1_6;
}

} // extern "C"
