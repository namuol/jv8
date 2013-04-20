package com.jovianware.jv8;


public class V8Value {
  private long handle;
  
  private native void init(V8Runner runner);
  private native void init(V8Runner runner, V8Value[] array);
  private native void init(V8Runner runner, double num);
  private native void init(V8Runner runner, String str);

  public native boolean isArray();
  public native boolean isBoolean();
  public native boolean isNumber();
  public native boolean isString();
  //TODO public native boolean isObject();
  //TODO public native boolean isFunction();
  //TODO public native boolean isDate();
  //TODO? public native boolean isExternal();
  //TODO? public native boolean isRegExp();

  public native V8Value[] toArray();
  public native boolean toBoolean();
  public native double toNumber();
  @Override
  public native String toString();
  //TODO public native Map<String,V8Value> toObject();
  //TODO public native V8Function toFunction();
  //TODO public native Date toDate();
  //TODO? public native Object toExternal();
  //TODO? public native RegEx isRegExp();

  private native void dispose();
  
  private V8Value() {}
  
  public V8Value(V8Runner runner) { init(runner); }
  public V8Value(V8Runner runner, V8Value[] array) { init(runner, array); }
  public V8Value(V8Runner runner, double num) { init(runner, num); }
  public V8Value(V8Runner runner, String str) { init(runner, str); }
  public void finalize() { dispose(); }
}
