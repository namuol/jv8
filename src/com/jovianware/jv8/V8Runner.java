package com.jovianware.jv8;


public class V8Runner {
  private V8Value Undefined_;
  public V8Value Undefined() {
    return Undefined_;
  }
  
  static {
    System.loadLibrary("gnustl_shared");
    System.loadLibrary("jv8");
  }
  
  public V8Value val(String str) {
    return new V8Value(this, str);
  }
  
  public V8Value val(double num) {
    return new V8Value(this, num);
  }
  
  private native long create();
  public native void dispose();
  
  public native V8Value runJS(String src) throws V8Exception;
  
  public V8Value tryRunJS(String src) {
    try {
      return runJS(src);
    } catch (V8Exception e) {
      return null;
    }
  }
  
  public native void map(String name, V8MappableMethod m);
  
  private long handle;
  public V8Runner() {
    handle = create();
    Undefined_ = new V8Value(this);
  }
}
