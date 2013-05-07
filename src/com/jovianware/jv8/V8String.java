package com.jovianware.jv8;

public class V8String implements V8Value {
  private String val;
  
  public V8String() {
    val = "";
  }
  
  public V8String(String val) {
    this.val = val;
  }

  public String value() {
    return val;
  }
  
  @Override
  public boolean isBoolean() {
    return false;
  }

  @Override
  public boolean isNumber() {
    return false;
  }

  @Override
  public boolean isString() {
    return true;
  }

  @Override
  public boolean isUndefined() {
    return false;
  }

  @Override
  public boolean toBoolean() {
    return Boolean.parseBoolean(val);
  }

  @Override
  public double toNumber() {
    try {
      return Double.parseDouble(val);
    } catch (NumberFormatException e) {
      return Double.NaN;
    }
  }
  
  @Override
  public String toString() {
    return val;
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_STRING;
  }

}
