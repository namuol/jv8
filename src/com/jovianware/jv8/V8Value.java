package com.jovianware.jv8;

import java.util.Map;


public class V8Value {
  private long handle;
  
  private native void init(V8Runner runner);
  private native void init(V8Runner runner, V8Value[] array);
  private native void init(V8Runner runner, double num);
  private native void init(V8Runner runner, String str);
  private native void init(V8Runner runner, Map<String, V8Value> object);

  public native boolean isArray();
  public native boolean isBoolean();
  public native boolean isNumber();
  public native boolean isString();
  public native boolean isObject();
  //TODO public native boolean isFunction();
  //TODO public native boolean isDate();
  //TODO? public native boolean isExternal();
  //TODO? public native boolean isRegExp();

  public native V8Value[] toArray();
  public native boolean toBoolean();
  public native double toNumber();
  @Override
  public native String toString();
  public native Map<String,V8Value> toObject();
  //TODO public native V8Function toFunction();
  //TODO public native Date toDate();
  //TODO? public native Object toExternal();
  //TODO? public native RegEx isRegExp();

  private native void dispose();
  
  private V8Value() {}
  
  public V8Value(V8Runner runner) { init(runner); }
  public V8Value(V8Runner runner, V8Value[] array) { init(runner, array); }
  public V8Value(V8Runner runner, int num) { init(runner, num); }
  public V8Value(V8Runner runner, long num) { init(runner, num); }
  public V8Value(V8Runner runner, double num) { init(runner, num); }
  public V8Value(V8Runner runner, String str) { init(runner, str); }
  public V8Value(V8Runner runner, Map<String, V8Value> object) { init(runner, object); }
  public void finalize() { dispose(); }
  
}
