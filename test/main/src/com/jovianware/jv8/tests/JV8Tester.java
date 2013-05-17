package com.jovianware.jv8.tests;

import java.util.Collection;
import java.util.Iterator;

import com.jovianware.jv8.V8Exception;
import com.jovianware.jv8.V8MappableMethod;
import com.jovianware.jv8.V8Runner;
import com.jovianware.jv8.V8Value;

public class JV8Tester {
  private V8Runner v8;
  private static final int TEST_INSTANCE_COUNT = 5;

  // It's 2013, and Java still doesn't have a join() method in any of its standard utilities.
  static String join(Collection<?> s, String delimiter) {
    StringBuilder builder = new StringBuilder();
    Iterator<?> iter = s.iterator();
    while (iter.hasNext()) {
      builder.append(iter.next());
      if (!iter.hasNext()) {
        break;
      }
      builder.append(delimiter);
    }
    return builder.toString();
  }
  
  class TestMappableMethod implements V8MappableMethod {
    @Override
    public V8Value methodToRun(V8Value[] args) {
      System.out.println("Hello from Java!");
      System.out.println("Arguments:");
      for (V8Value val : args) {
        System.out.println(val.toString());
      }
      return v8.val("TESTING RETURN VAL");
    }
  }
  
  public void runTest() {
    v8 = new V8Runner();
    try {
      v8.runJS("(program)",
        "42;\n" +
        "42+1;\n" +
        "somethingUndefined.runMe()"
      );
    } catch (V8Exception e) {
      System.err.println(e.getMessage());
    }
    
    v8.map("sayHello", new TestMappableMethod());
    V8Value result = v8.tryRunJS("(program)", "sayHello('Testing', 1, 2, 3);");
    System.out.println(result.toString());
    
    
    for (int i = 0; i < TEST_INSTANCE_COUNT; ++i) {
      // Test multiple instances:
      V8Runner testInstance = new V8Runner();

      try {
        // This wont work because `sayHello` was only
        // defined for the other V8Runner context.
        testInstance.runJS("(program)", "sayHello('This wont work.');");
      } catch (V8Exception e) {
        System.err.println(e.getMessage());
      }
      
      // GC can clean up `testInstance` now.
    }
    
  }
}
