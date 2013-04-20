package com.jovianware.jv8;

public interface V8MappableMethod {
  V8Value methodToRun(V8Value[] args);
}
