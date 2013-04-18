package com.jovianware;


public class V8Value {
  private long handle;
  
  private native void init(V8Runner runner);
  private native void init(V8Runner runner, String str);
  private native void init(V8Runner runner, double num);
  
  private native boolean isUndefined();
  private native boolean isString();
  private native boolean isNumber();
  private native boolean isBoolean();
  
  public native String asString();
  public native double asNumber();
  public native boolean asBoolean();
  
  @Override
  public native String toString();
  
  private native void dispose();
  
  private V8Value() {}
  
  public V8Value(V8Runner runner) { init(runner); }
  public V8Value(V8Runner runner, String str) { init(runner, str); }
  public V8Value(V8Runner runner, double num) { init(runner, num); }
  public void finalize() { dispose(); }
}
