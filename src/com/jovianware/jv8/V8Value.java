package com.jovianware.jv8;

public interface V8Value {
  public static final int TYPE_UNDEFINED = -1;
  public static final int TYPE_BOOLEAN = 0;
  public static final int TYPE_NUMBER = 1;
  public static final int TYPE_STRING = 2;
  
  public int getTypeID();
  
  //public boolean isArray(); // TODO
  public boolean isBoolean();
  public boolean isNumber();
  public boolean isString();
  //public boolean isObject(); // TODO
  public boolean isUndefined();
  
  public boolean toBoolean();
  public double toNumber();
  @Override
  public String toString();
  
}
