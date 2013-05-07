package com.jovianware.jv8;

public class V8Boolean implements V8Value {
  
  private boolean val;

  public V8Boolean(boolean val) {
    this.val = val;
  }

  public boolean value() {
    return val;
  }
  
  @Override
  public boolean isBoolean() {
    return true;
  }

  @Override
  public boolean isNumber() {
    return false;
  }

  @Override
  public boolean isString() {
    return false;
  }

  @Override
  public boolean isUndefined() {
    return false;
  }

  @Override
  public boolean toBoolean() {
    return val;
  }

  @Override
  public double toNumber() {
    return val ? 1 : 0;
  }
  
  @Override
  public String toString() {
    return Boolean.toString(val);
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_BOOLEAN;
  }

}
