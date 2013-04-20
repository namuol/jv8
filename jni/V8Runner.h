#ifndef V8RUNNER_H_
#define V8RUNNER_H_
#include <vector>
using namespace std;

#include <jni.h>

#include <v8.h>
using namespace v8;

#include "jv8.h"
namespace jv8 {

class V8Value;
class V8Exception;

class V8Runner {
  Isolate* isolate;
  Handle<Context> context;
  vector<MappableMethodData*> methodDatas;

public:
  V8Runner();

  V8Value* runJS(const char* str);
  void mapMethod (JNIEnv* env,  jobject v8MappableMethod, const char* name);
  void destroy(JNIEnv* env);
  Isolate* getIsolate();
  Handle<Context>& getContext();
  friend class V8Value;
  friend class V8Exception;
};

} // namespace jv8

#endif // V8RUNNER_H_