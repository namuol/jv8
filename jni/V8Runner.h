#ifndef V8RUNNER_H_
#define V8RUNNER_H_
#include <vector>
using namespace std;

#include <jni.h>

#include <v8.h>

#include "jv8.h"
namespace jv8 {

class V8Runner {
  v8::Isolate* isolate;
  v8::Handle<v8::Context> context;
  vector<MappableMethodData*> methodDatas;

public:
  V8Runner();

  v8::Handle<v8::Value> runJS(const char* str);
  void mapMethod (JNIEnv* env,  jobject v8MappableMethod, const char* name);
  void destroy(JNIEnv* env);
  v8::Isolate* getIsolate();
  v8::Handle<v8::Context>& getContext();
};

} // namespace jv8

#endif // V8RUNNER_H_