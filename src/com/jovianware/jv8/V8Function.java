package com.jovianware.jv8;

/**
 * An instance of V8Function has a v8::Function equivalent on the native side.
 * The link between those structures is stored in a map inside v8.cpp.
 */
public class V8Function extends V8Object {

  private long handle;

  /**
   * For now we don't support JS function instantiation with a String.
   */
  public V8Function() {
    handle = 0;
  }

  /**
   * Constructs a function with a native JS Function pointer.
   * @param handle A pointer to Persistent<Function>
   */
  private V8Function( long handle ){
    this.handle = handle;
  }

  /**
   * Calls the JS function with args as arguments.
   *
   * @param args
   *
   * @return null
   */
  public V8Value call(V8Value[] args) {
    return null;
  }

  /**
   * Returns whether the V8Function is bound to a v8::Function yet
   * It can be false if we instantiate V8Function with a String
   * and that jv8 hasn't created the v8::Function yet.
   */
  public boolean isLinked(){
    return handle != 0;
  }

  @Override
  public boolean isFunction(){
    return true;
  }

  @Override
  public int getTypeID() {
    return V8Value.TYPE_FUNCTION;
  }
}
