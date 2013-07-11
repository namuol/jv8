#ifndef JV8_H_
#define JV8_H_
#include <jni.h>

#include <v8.h>
using namespace v8;

namespace jv8 {

static jfieldID f_V8Runner_handle,
                f_V8Function_handle,
                f_V8Boolean_val,
                f_V8Number_val,
                f_V8String_val,
                sf_V8Value_TYPE_BOOLEAN,
                sf_V8Value_TYPE_NUMBER,
                sf_V8Value_TYPE_STRING,
                sf_V8Value_TYPE_FUNCTION;

static jmethodID m_V8String_init_str,
                 m_V8Number_init_num,
                 m_V8Boolean_init_bool,
                 m_V8Function_init,
                 m_V8MappableMethod_methodToRun,
                 m_V8Value_getTypeID;

static jclass V8Runner_class,
              V8Value_class,
              V8String_class,
              V8Number_class,
              V8Boolean_class,
              V8Function_class,
              V8Exception_class,
              V8MappableMethod_class;

static bool needsToCacheClassData = true;

static int V8VALUE_TYPE_BOOLEAN,
           V8VALUE_TYPE_NUMBER,
           V8VALUE_TYPE_STRING,
           V8VALUE_TYPE_FUNCTION;

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
  REQUIRE_STATIC_FIELD(V8Value_class, "TYPE_FUNCTION", "I", sf_V8Value_TYPE_FUNCTION)

  V8VALUE_TYPE_BOOLEAN = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_BOOLEAN);
  V8VALUE_TYPE_NUMBER = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_NUMBER);
  V8VALUE_TYPE_STRING = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_STRING);
  V8VALUE_TYPE_FUNCTION = env->GetStaticIntField(V8Value_class, sf_V8Value_TYPE_FUNCTION);

  // V8String
  REQUIRE_CLASS("com/jovianware/jv8/V8String", V8String_class)
  REQUIRE_METHOD(V8String_class, "<init>", "(Ljava/lang/String;)V", m_V8String_init_str)
  REQUIRE_FIELD(V8String_class, "val", "Ljava/lang/String;", f_V8String_val)

  // V8Number
  REQUIRE_CLASS("com/jovianware/jv8/V8Number", V8Number_class)
  REQUIRE_METHOD(V8Number_class, "<init>", "(D)V", m_V8Number_init_num)
  REQUIRE_FIELD(V8Number_class, "val", "D", f_V8Number_val)

  // V8Boolean
  REQUIRE_CLASS("com/jovianware/jv8/V8Boolean", V8Boolean_class)
  REQUIRE_METHOD(V8Boolean_class, "<init>", "(Z)V", m_V8Boolean_init_bool)
  REQUIRE_FIELD(V8Boolean_class, "val", "Z", f_V8Boolean_val)

  // V8Function
  REQUIRE_CLASS("com/jovianware/jv8/V8Function", V8Function_class)
  REQUIRE_METHOD(V8Function_class, "<init>", "(J)V", m_V8Function_init)
  REQUIRE_FIELD(V8Function_class, "handle", "J", f_V8Function_handle)

  REQUIRE_CLASS("com/jovianware/jv8/V8MappableMethod", V8MappableMethod_class)
  REQUIRE_METHOD(V8MappableMethod_class, "methodToRun", "([Lcom/jovianware/jv8/V8Value;)Lcom/jovianware/jv8/V8Value;", m_V8MappableMethod_methodToRun)

  needsToCacheClassData = false;
}

static inline Handle<Value>
v8ValueFromJObject (
  JNIEnv* env,
  jobject jobj
) {

  if (needsToCacheClassData) {
    cacheClassData(env);
  }

  if (jobj == NULL) {
    return Null();
  }

  jint jobjType = env->CallIntMethod(jobj, m_V8Value_getTypeID);
  Handle<Value> returnVal;

  // Number
  if (jobjType == V8VALUE_TYPE_NUMBER) {
    jdouble num = env->GetDoubleField(jobj, f_V8Number_val);
  
    returnVal = Number::New(num);
  
  }

  // Boolean
  else if (jobjType == V8VALUE_TYPE_BOOLEAN) {
    jboolean b = env->GetBooleanField(jobj, f_V8Boolean_val);
  
    returnVal = Boolean::New(b);
  }

  // String
  else if (jobjType == V8VALUE_TYPE_STRING) {
    jstring jstr = (jstring) env->GetObjectField(jobj, f_V8String_val);
    const char* str = env->GetStringUTFChars(jstr, 0);
  
    returnVal = String::New(str);
  
    env->ReleaseStringUTFChars(jstr, str);
    env->DeleteLocalRef(jstr);
  }

  // Function
  else if (jobjType == V8VALUE_TYPE_FUNCTION) {
    Persistent<Value>* functionPersistent = (Persistent<Value>*) env->GetLongField(jobj, f_V8Function_handle);
    Persistent<Function> function = Persistent<Function>::Cast( *functionPersistent );
    returnVal = Handle<Function>::Cast(function);
  }

  // Default to Undefined
  else {
    returnVal = Undefined();
  }

  return returnVal;
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

  // Number
  if (value->IsNumber()) {
    wrappedReturnValue = env->NewObject(V8Number_class,
      m_V8Number_init_num,
      value->NumberValue()
    );
  }

  // String
  else if (value->IsString()) {
    jstring jstr = env->NewStringUTF( *String::Utf8Value(value->ToString()) );
    
    wrappedReturnValue = env->NewObject(V8String_class,
      m_V8String_init_str,
      jstr
    );

    env->DeleteLocalRef(jstr);
  }

  // Boolean
  else if (value->IsBoolean()) {
    wrappedReturnValue = env->NewObject(V8Boolean_class,
      m_V8Boolean_init_bool,
      value->BooleanValue()
    );

  }

  // Function
  else if( value->IsFunction()) {

    Persistent<Function>* functionPersistent = new Persistent<Function>(value);
    jvalue handlePointer;
    handlePointer.j = (long)functionPersistent;
    wrappedReturnValue = env->NewObject(V8Function_class,
      m_V8Function_init,
      handlePointer
    );    
  }

  // Default to null
  else {
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
  data->jvm->AttachCurrentThread((void**)&env, NULL);
  
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

  returnVal = v8ValueFromJObject(env, jresult);

  env->DeleteLocalRef(jresult);

  return returnVal; // TODO
}

} // namespace jv8

#endif // JV8_H_