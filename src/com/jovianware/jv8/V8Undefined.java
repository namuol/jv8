package com.jovianware.jv8;

public class V8Undefined implements V8Value {

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
    return false;
  }

  @Override
  public boolean isUndefined() {
    return true;
  }

  @Override
  public boolean toBoolean() {
    return false;
  }

  @Override
  public double toNumber() {
    return Double.NaN;
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_UNDEFINED;
  }

}
