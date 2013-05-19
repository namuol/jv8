package com.jovianware.jv8;


public class V8Runner {
  private V8Value Undefined_;
  public V8Value Undefined() {
    return Undefined_;
  }
  
  static {
    System.loadLibrary("jv8");
  }
  
  public static native void setDebuggingRunner(V8Runner v8);
  
  public V8Value val(String str) {
    return new V8String(str);
  }
  
  public V8Value val(double num) {
    return new V8Number(num);
  }
  
  public V8Value val(boolean bool) {
    return new V8Boolean(bool);
  }
  
  private native long create();
  private native void dispose();
  
  public native V8Value runJS(String name, String src) throws V8Exception;
  
  public V8Value tryRunJS(String name, String src) {
    try {
      return runJS(name, src);
    } catch (V8Exception e) {
      return null;
    }
  }
  
  @Override
  public void finalize() {
    dispose();
  }
  
  public native void map(String name, V8MappableMethod m);
  
  private long handle;
  public V8Runner() {
    handle = create();
    Undefined_ = new V8Undefined();
  }
}
