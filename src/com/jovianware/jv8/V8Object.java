package com.jovianware.jv8;

/**
 * Created by ldiqual on 7/9/13.
 */
public class V8Object extends V8Value {
  private Object val;

  protected V8Object() {
  }

  public Object value() {
    return val;
  }

  @Override
  public boolean isObject() {
    return true;
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_OBJECT;
  }
}
