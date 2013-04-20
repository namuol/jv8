#include <jni.h>

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "V8Value.h"
#include "V8Exception.h"
#include "jv8.h"

// TODO: Integrate this in with build system:
#define NDK_GDB_ENTRYPOINT_WAIT_HACK

namespace jv8 {

//////////////////////////////////////////////////////////////////////////////////////////////
// V8RUNNER CLASS
//////////////////////////////////////////////////////////////////////////////////////////////
static jobject V8Runner_runJS (
  JNIEnv *env,
  jobject jrunner,
  jstring jstr
) {
  const char* js = env->GetStringUTFChars(jstr, NULL);
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  V8Value* returnValue = runner->runJS(js);

  if (returnValue == NULL) {
    jclass V8Exception_class = env->FindClass("com/jovianware/jv8/V8Exception");

    env->ReleaseStringUTFChars(jstr, js);
    env->ThrowNew(V8Exception_class, "Unexpected Error running JS");
    return 0;
  }
  
  if (returnValue->isException()) {
    Isolate* isolate = runner->getIsolate();
    Handle<Context>& context = runner->getContext();
    Locker l(isolate);
    Isolate::Scope isolateScope(isolate);

    HandleScope handle_scope;

    Context::Scope context_scope(context);

    jclass V8Exception_class = env->FindClass("com/jovianware/jv8/V8Exception");
    V8Exception* exception = reinterpret_cast<V8Exception*>(returnValue);

    env->ReleaseStringUTFChars(jstr, js);
    env->ThrowNew(V8Exception_class, *String::Utf8Value(exception->getMessage()->ToString()));
    return 0;
  }

  jclass V8Value_class = env->FindClass("com/jovianware/jv8/V8Value");
  jobject wrappedReturnValue = env->NewObject(V8Value_class, m_V8Value_init_internal);
  env->SetLongField(wrappedReturnValue, f_V8Value_handle, (jlong) returnValue);

  env->ReleaseStringUTFChars(jstr, js);
  return wrappedReturnValue;
}

static jlong V8Runner_create (
  JNIEnv *env,
  jclass klass
) {
  return (jlong) new V8Runner();
}

static void V8Runner_destroy (
  JNIEnv *env,
  jclass obj
) {
  V8Runner* r = (V8Runner*) env->GetLongField(obj, f_V8Runner_handle);
  delete r;
}

static void V8Runner_map (
  JNIEnv *env,
  jobject jrunner,
  jobject jmappableMethod,
  jstring jname
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  const char* name = env->GetStringUTFChars(jname, NULL);
  runner->mapMethod(env, jmappableMethod, name);
  env->ReleaseStringUTFChars(jname, name);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// V8VALUE CLASS
//////////////////////////////////////////////////////////////////////////////////////////////
static void V8Value_init_void (
  JNIEnv *env,
  jobject obj,
  jobject jrunner
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  env->SetLongField(obj, f_V8Value_handle, (jlong) new V8Value(runner));
}

static void V8Value_init_array (
  JNIEnv *env,
  jobject obj,
  jobject jrunner,
  jobjectArray jarray
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  Isolate* isolate = runner->getIsolate();
  Handle<Context>& context = runner->getContext();
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);
  int len = env->GetArrayLength(jarray);
  Local<Array> array = Array::New(len);

  for (int i=0; i<len; ++i) {
    jobject obj = env->GetObjectArrayElement(jarray, i);
    V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
    array->Set(i, val->getValue());
  }
  env->SetLongField(obj, f_V8Value_handle, (jlong) new V8Value(runner, array));
}

static void V8Value_init_num (
  JNIEnv *env,
  jobject obj,
  jobject jrunner,
  jdouble num
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  env->SetLongField(obj, f_V8Value_handle, (jlong) new V8Value(runner, num)); 
}

static void V8Value_init_str (
  JNIEnv *env,
  jobject obj,
  jobject jrunner,
  jstring jstr
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  const char* cstr = env->GetStringUTFChars(jstr, NULL);
  env->SetLongField(obj, f_V8Value_handle, (jlong) new V8Value(runner, cstr)); 
  env->ReleaseStringUTFChars(jstr, cstr);
}

static jboolean V8Value_isArray (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsArray();
}

static jboolean V8Value_isBoolean (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsBoolean();
}

static jboolean V8Value_isNumber (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsNumber();
}

static jboolean V8Value_isString (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsString();
}

static jobjectArray V8Value_toArray (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  V8Runner* runner = val->getRunner();

  jclass V8Value_class = env->FindClass("com/jovianware/jv8/V8Value");
  Array* array = Array::Cast(*val->getValue());
  jobjectArray jarray = (jobjectArray) env->NewObjectArray(array->Length(), env->FindClass("com/jovianware/jv8/V8Value"), NULL);
  for (int i=0; i<array->Length(); ++i) {
    jobject wrappedValue = env->NewObject(V8Value_class, m_V8Value_init_internal);
    env->SetLongField(wrappedValue, f_V8Value_handle, (jlong) new V8Value(runner, array->Get(i)));
    env->SetObjectArrayElement(jarray, i, wrappedValue);
  }
  return jarray;
}

static jboolean V8Value_toBoolean (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->BooleanValue();
}

static jdouble V8Value_toNumber (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->ToNumber()->Value();
}

static jstring V8Value_toString (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);

  // We have to setup an isolate context here, perhaps because ToString actually performs some JS 
  //  magic behind the scenes to coerce the `Value` into a String.
  V8Runner* runner = val->getRunner();
  Isolate* isolate = runner->getIsolate();
  Handle<Context>& context = runner->getContext();
  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope;

  Context::Scope context_scope(context);

  return env->NewStringUTF(*String::Utf8Value(val->getValue()->ToString()));
}

static void V8Value_dispose (
  JNIEnv *env,
  jobject obj
) {
  delete (V8Value*) env->GetLongField(obj, f_V8Value_handle);
}

} // namespace jv8

static JNINativeMethod V8Runner_Methods[] = {
  {"create", "()J", (void *) jv8::V8Runner_create},
  {"dispose", "()V", (void *) jv8::V8Runner_destroy},
  {"runJS", "(Ljava/lang/String;)Lcom/jovianware/jv8/V8Value;", (void *) jv8::V8Runner_runJS},
  {"map", "(Lcom/jovianware/jv8/V8MappableMethod;Ljava/lang/String;)V", (void *) jv8::V8Runner_map},
};

static JNINativeMethod V8Value_Methods[] = {
  {"init", "(Lcom/jovianware/jv8/V8Runner;)V", (void *) jv8::V8Value_init_void},
  {"init", "(Lcom/jovianware/jv8/V8Runner;[Lcom/jovianware/jv8/V8Value;)V", (void *) jv8::V8Value_init_array},
  {"init", "(Lcom/jovianware/jv8/V8Runner;D)V", (void *) jv8::V8Value_init_num},
  {"init", "(Lcom/jovianware/jv8/V8Runner;Ljava/lang/String;)V", (void *) jv8::V8Value_init_str},
  {"dispose", "()V", (void *) jv8::V8Value_dispose},
  {"isArray", "()Z", (void *) jv8::V8Value_isArray},
  {"isBoolean", "()Z", (void *) jv8::V8Value_isBoolean},
  {"isNumber", "()Z", (void *) jv8::V8Value_isNumber},
  {"isString", "()Z", (void *) jv8::V8Value_isString},
  {"toArray", "()[Lcom/jovianware/jv8/V8Value;", (void *) jv8::V8Value_toArray},
  {"toNumber", "()D", (void *) jv8::V8Value_toNumber},
  {"toBoolean", "()Z", (void *) jv8::V8Value_toBoolean},
  {"toString", "()Ljava/lang/String;", (void *) jv8::V8Value_toString}
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

#define CLASS(className)\
  klass = NULL;\
  klass = (env)->FindClass(className);\
  if (!klass) { return -1; }\

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

  CLASS("com/jovianware/jv8/V8Runner")
  MTABLE(V8Runner_Methods)
  FIELD("handle", "J", jv8::f_V8Runner_handle)
  CLASS_END()

  CLASS("com/jovianware/jv8/V8Value")
  MTABLE(V8Value_Methods)
  FIELD("handle", "J", jv8::f_V8Value_handle)
  METHOD("<init>", "(Lcom/jovianware/jv8/V8Runner;)V", jv8::m_V8Value_init)
  METHOD("<init>", "(Lcom/jovianware/jv8/V8Runner;Ljava/lang/String;)V", jv8::m_V8Value_init_str)
  METHOD("<init>", "(Lcom/jovianware/jv8/V8Runner;D)V", jv8::m_V8Value_init_num)
  METHOD("<init>", "()V", jv8::m_V8Value_init_internal)
  CLASS_END()

  CLASS("com/jovianware/jv8/V8MappableMethod")
  METHOD("methodToRun", "([Lcom/jovianware/jv8/V8Value;)Lcom/jovianware/jv8/V8Value;", jv8::m_V8MappableMethod_methodToRun)
  CLASS_END()

  return JNI_VERSION_1_6;
}

} // extern "C"
