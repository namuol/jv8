package com.jovianware.jv8;

public class V8Number extends V8Value {
  private double val;
  
  public V8Number() {
  }
  
  public V8Number(double val) {
    this.val = val;
  }
  
  public double value() {
    return val;
  }

  @Override
  public boolean isNumber() {
    return true;
  }

  @Override
  public boolean toBoolean() {
    return val != 0;
  }

  @Override
  public double toNumber() {
    return val;
  }
  
  @Override
  public String toString() {
    return Double.toString(val);
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_NUMBER;
  }

}
