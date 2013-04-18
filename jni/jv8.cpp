#include <jni.h>

#include <v8.h>
using namespace v8;

#include "V8Runner.h"
#include "V8Value.h"
#include "jv8.h"

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
  V8Value* result = new V8Value(runner, runner->runJS(js));
  jclass V8Value_class = env->FindClass("com/vatedroid/V8Value");
  jobject wrappedResult = env->NewObject(V8Value_class, m_V8Value_init_internal);
  env->SetLongField(wrappedResult, f_V8Value_handle, (jlong) result);

  env->ReleaseStringUTFChars(jstr, js);
  return wrappedResult;
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

static JNINativeMethod V8Runner_Methods[] = {
  {"create", "()J", (void *) V8Runner_create},
  {"dispose", "()V", (void *) V8Runner_destroy},
  {"runJS", "(Ljava/lang/String;)Lcom/vatedroid/V8Value;", (void *) V8Runner_runJS},
  {"map", "(Lcom/vatedroid/V8MappableMethod;Ljava/lang/String;)V", (void *) V8Runner_map},
};

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

static void V8Value_init_num (
  JNIEnv *env,
  jobject obj,
  jobject jrunner,
  jdouble num
) {
  V8Runner* runner = (V8Runner*) env->GetLongField(jrunner, f_V8Runner_handle);
  env->SetLongField(obj, f_V8Value_handle, (jlong) new V8Value(runner, num)); 
}

static jstring V8Value_asString (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return env->NewStringUTF(*String::Utf8Value(val->getValue()->ToString()));
}

static jdouble V8Value_asNumber (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->NumberValue();
}

static jboolean V8Value_asBoolean (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->BooleanValue();
}

static jboolean V8Value_isString (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsString();
}

static jboolean V8Value_isNumber (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsNumber();
}

static jboolean V8Value_isBoolean (
  JNIEnv *env,
  jobject obj
) {
  V8Value* val = (V8Value*) env->GetLongField(obj, f_V8Value_handle);
  return val->getValue()->IsBoolean();
}

static void V8Value_dispose (
  JNIEnv *env,
  jobject obj
) {
  delete (V8Value*) env->GetLongField(obj, f_V8Value_handle);
}

static JNINativeMethod V8Value_Methods[] = {
  {"init", "(Lcom/vatedroid/V8Runner;)V", (void *) V8Value_init_void},
  {"init", "(Lcom/vatedroid/V8Runner;Ljava/lang/String;)V", (void *) V8Value_init_str},
  {"init", "(Lcom/vatedroid/V8Runner;D)V", (void *) V8Value_init_num},
  {"dispose", "()V", (void *) V8Value_dispose},
  {"isString", "()Z", (void *) V8Value_isString},
  {"isNumber", "()Z", (void *) V8Value_isNumber},
  {"isBoolean", "()Z", (void *) V8Value_isBoolean},
  {"asString", "()Ljava/lang/String;", (void *) V8Value_asString},
  {"asNumber", "()D", (void *) V8Value_asNumber},
  {"asBoolean", "()Z", (void *) V8Value_asBoolean},
  {"toString", "()Ljava/lang/String;", (void *) V8Value_asString}
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

  CLASS("com/vatedroid/V8Runner")
  MTABLE(V8Runner_Methods)
  FIELD("handle", "J", f_V8Runner_handle)
  CLASS_END()

  CLASS("com/vatedroid/V8Value")
  MTABLE(V8Value_Methods)
  FIELD("handle", "J", f_V8Value_handle)
  METHOD("<init>", "(Lcom/vatedroid/V8Runner;)V", m_V8Value_init)
  METHOD("<init>", "(Lcom/vatedroid/V8Runner;Ljava/lang/String;)V", m_V8Value_init_str)
  METHOD("<init>", "(Lcom/vatedroid/V8Runner;D)V", m_V8Value_init_num)
  METHOD("<init>", "()V", m_V8Value_init_internal)
  CLASS_END()

  CLASS("com/vatedroid/V8MappableMethod")
  METHOD("methodToRun", "([Lcom/vatedroid/V8Value;)Lcom/vatedroid/V8Value;", m_V8MappableMethod_methodToRun)
  CLASS_END()

  return JNI_VERSION_1_6;
}

} // extern "C"