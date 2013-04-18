package com.jovianware;

public interface V8MappableMethod {
  V8Runner getRunner();
  V8Value methodToRun(V8Value[] args);
}
