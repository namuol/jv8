#ifndef JV8_H_
#define JV8_H_
#include <jni.h>

#include <v8.h>
using namespace v8;

#include "V8Value.h"

namespace jv8 {

static jfieldID f_V8Value_handle,
                f_V8Runner_handle;

static jmethodID m_V8Value_init,
                 m_V8Value_init_str,
                 m_V8Value_init_num,
                 m_V8Value_init_internal,
                 m_V8MappableMethod_methodToRun;

struct MappableMethodData {
  jobject methodObject;
  V8Runner* runner;
  JavaVM* jvm;
};

static Handle<Value>
registerCallback (const Arguments& args) {
  Isolate* isolate = args.GetIsolate();

  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope(isolate);

  MappableMethodData* data = (MappableMethodData*) External::Cast(*args.Data())->Value();
  JNIEnv* env;
  jint result = data->jvm->AttachCurrentThread(&env, NULL);
  jobject methodObject = data->methodObject;
  jclass V8MappableMethod_class = env->GetObjectClass(methodObject);
  if (!m_V8MappableMethod_methodToRun) {
    m_V8MappableMethod_methodToRun = env->GetMethodID(V8MappableMethod_class, "methodToRun", "([Lcom/jovianware/jv8/V8Value;)Lcom/jovianware/jv8/V8Value;");
  }

  jclass V8Value_class = env->FindClass("com/jovianware/jv8/V8Value");
  if (!m_V8Value_init_internal) {
    m_V8Value_init_internal = env->GetMethodID(V8Value_class, "<init>", "()V");
  }
  if (!f_V8Value_handle) {
    f_V8Value_handle = env->GetFieldID(V8Value_class, "handle", "J");
  }

  V8Runner* runner = data->runner;
  jobjectArray jargs = (jobjectArray) env->NewObjectArray(args.Length(), env->FindClass("com/jovianware/jv8/V8Value"), NULL);
  for (int i=0; i<args.Length(); ++i) {
    jobject wrappedArg = env->NewObject(V8Value_class, m_V8Value_init_internal);
    env->SetLongField(wrappedArg, f_V8Value_handle, (jlong) new V8Value(data->runner, args[i]));
    env->SetObjectArrayElement(jargs, i, wrappedArg);
  }

  jobject jresult = env->CallObjectMethod(methodObject, m_V8MappableMethod_methodToRun, jargs);

  // TODO Handle return value from Java.
  return Undefined();
}

} // namespace jv8

#endif // JV8_H_