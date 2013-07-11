package com.jovianware.jv8;

public class V8Value {
  public static final int TYPE_UNDEFINED  = -1;
  public static final int TYPE_BOOLEAN    = 0;
  public static final int TYPE_NUMBER     = 1;
  public static final int TYPE_STRING     = 2;
  public static final int TYPE_OBJECT     = 3;
  public static final int TYPE_FUNCTION   = 4;

  protected V8Value(){

  }

  public int getTypeID() {
    return TYPE_UNDEFINED;
  }
  
  //public boolean isArray(); // TODO

  public boolean isBoolean() {
    return false;
  }

  public boolean isNumber() {
    return false;
  }

  public boolean isString() {
    return false;
  }

  public boolean isObject() {
    return false;
  }

  public boolean isFunction() {
    return false;
  }

  public boolean isUndefined() {
    return false;
  }
  
  public boolean toBoolean() {
    return false;
  }

  public double toNumber() {
    return Double.NaN;
  }

  @Override
  public String toString(){
    return "[object Object]";
  }
  
}
