#ifndef JV8_H_
#define JV8_H_
#include <jni.h>

#include <v8.h>
using namespace v8;

namespace jv8 {

static jfieldID f_V8Runner_handle,
                f_V8Boolean_val,
                f_V8Number_val,
                f_V8String_val,
                sf_V8Value_TYPE_BOOLEAN,
                sf_V8Value_TYPE_NUMBER,
                sf_V8Value_TYPE_STRING;

static jmethodID m_V8String_init_str,
                 m_V8Number_init_num,
                 m_V8Boolean_init_bool,
                 m_V8MappableMethod_methodToRun,
                 m_V8Value_getTypeID;

static jclass V8Runner_class,
              V8Value_class,
              V8String_class,
              V8Number_class,
              V8Boolean_class,
              V8Exception_class,
              V8MappableMethod_class;

static bool needsToCacheClassData = true;

static int V8VALUE_TYPE_BOOLEAN,
           V8VALUE_TYPE_NUMBER,
           V8VALUE_TYPE_STRING;

struct MappableMethodData {
  jobject methodObject;
  JavaVM* jvm;
};

#define REQUIRE_CLASS(className, globalRef)\
  if (!globalRef) {\
    jclass klass = NULL;\
    klass = (env)->FindClass(className);\
    globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(klass));\
    env->DeleteLocalRef(klass);\
  }\

#define REQUIRE_METHOD(klass, method, signature, methodID)\
  if (!methodID) {\
    (methodID) = env->GetMethodID(klass, (method), (signature));\
  }\

#define REQUIRE_FIELD(klass, field, signature, fieldID)\
  if (!fieldID) {\
    (fieldID) = env->GetFieldID(klass, (field), (signature));\
  }\

#define REQUIRE_STATIC_FIELD(klass, field, signature, fieldID)\
  if (!fieldID) {\
    (fieldID) = env->GetStaticFieldID(klass, (field), (signature));\
  }\

static inline void
cacheClassData(JNIEnv* env) {

  REQUIRE_CLASS("com/jovianware/jv8/V8Value", V8Value_class)
  REQUIRE_METHOD(V8Value_class, "getTypeID", "()I", m_V8Value_getTypeID)
  REQUIRE_STATIC_FIELD(V8Value_class, "TYPE_BOOLEAN", "I", sf_V8Value_TYPE_BOOLEAN)
  REQUIRE_STATIC_FIELD(V8Value_class, "TYPE_NUMBER", "I", sf_V8Value_TYPE_NUMBER)
  REQUIRE_STATIC_FIELD(V8Value_class, "TYPE_STRING", "I", sf_V8Value_TYPE_STRING)

  V8VALUE_TYPE_BOOLEAN = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_BOOLEAN);
  V8VALUE_TYPE_NUMBER = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_NUMBER);
  V8VALUE_TYPE_STRING = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_STRING);

  REQUIRE_CLASS("com/jovianware/jv8/V8String", V8String_class)
  REQUIRE_METHOD(V8String_class, "<init>", "(Ljava/lang/String;)V", m_V8String_init_str)
  REQUIRE_FIELD(V8String_class, "val", "Ljava/lang/String;", f_V8String_val)

  REQUIRE_CLASS("com/jovianware/jv8/V8Number", V8Number_class)
  REQUIRE_METHOD(V8Number_class, "<init>", "(D)V", m_V8Number_init_num)
  REQUIRE_FIELD(V8Number_class, "val", "D", f_V8Number_val)

  REQUIRE_CLASS("com/jovianware/jv8/V8Boolean", V8Boolean_class)
  REQUIRE_METHOD(V8Boolean_class, "<init>", "(Z)V", m_V8Boolean_init_bool)
  REQUIRE_FIELD(V8Boolean_class, "val", "Z", f_V8Boolean_val)

  REQUIRE_CLASS("com/jovianware/jv8/V8MappableMethod", V8MappableMethod_class)
  REQUIRE_METHOD(V8MappableMethod_class, "methodToRun", "([Lcom/jovianware/jv8/V8Value;)Lcom/jovianware/jv8/V8Value;", m_V8MappableMethod_methodToRun)

  needsToCacheClassData = false;
}

static inline jobject
newV8Value (
  JNIEnv* env,
  Handle<Value> value
) {
  if (needsToCacheClassData) {
    cacheClassData(env);
  }

  jobject wrappedReturnValue;

  if (value->IsNumber()) {
    wrappedReturnValue = env->NewObject(V8Number_class,
      m_V8Number_init_num,
      value->NumberValue()
    );
  } else if (value->IsString()) {
    wrappedReturnValue = env->NewObject(V8String_class,
      m_V8String_init_str,
      env->NewStringUTF( *String::Utf8Value(value->ToString()) )
    );
  } else if (value->IsBoolean()) {
    wrappedReturnValue = env->NewObject(V8Boolean_class,
      m_V8Boolean_init_bool,
      value->BooleanValue()
    );
  } else {
    wrappedReturnValue = NULL;
  }

  return wrappedReturnValue;
}

static Handle<Value>
registerCallback (const Arguments& args) {
  Isolate* isolate = args.GetIsolate();

  Locker l(isolate);
  Isolate::Scope isolateScope(isolate);

  HandleScope handle_scope(isolate);

  MappableMethodData* data = (MappableMethodData*) External::Cast(*args.Data())->Value();
  JNIEnv* env;
  data->jvm->AttachCurrentThread(&env, NULL);
  
  if (needsToCacheClassData) {
    cacheClassData(env);
  }

  jobject methodObject = data->methodObject;

  jobjectArray jargs = (jobjectArray) env->NewObjectArray(args.Length(), V8Value_class, NULL);
  for (int i=0; i<args.Length(); ++i) {
    jobject wrappedArg = newV8Value(env, args[i]);

    env->SetObjectArrayElement(jargs, i, wrappedArg);
    env->DeleteLocalRef(wrappedArg);
  }

  Handle<Value> returnVal;

  jobject jresult = env->CallObjectMethod(methodObject, m_V8MappableMethod_methodToRun, jargs);

  env->DeleteLocalRef(jargs);

  if (jresult == NULL) {
    returnVal = Null();
  } else {

    jint jresultType = env->CallIntMethod(jresult, m_V8Value_getTypeID);

    if (jresultType == V8VALUE_TYPE_NUMBER) {
      jdouble num = env->GetDoubleField(jresult, f_V8Number_val);
    
      returnVal = Number::New(num);
    
    } else if (jresultType == V8VALUE_TYPE_BOOLEAN) {
      jboolean b = env->GetBooleanField(jresult, f_V8Boolean_val);
    
      returnVal = Boolean::New(b);
    
    } else if (jresultType == V8VALUE_TYPE_STRING) {
      jstring jstr = (jstring) env->GetObjectField(jresult, f_V8String_val);
      const char* str = env->GetStringUTFChars(jstr, 0);
    
      returnVal = String::New(str);
    
      env->ReleaseStringUTFChars(jstr, str);
      env->DeleteLocalRef(jstr);
    } else {
      returnVal = Undefined();
    }
  }

  env->DeleteLocalRef(jresult);

  return returnVal; // TODO
}

} // namespace jv8

#endif // JV8_H_