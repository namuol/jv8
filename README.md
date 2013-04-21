jv8
===

A simple way to run V8 JavaScript instances from Java.

```java
class NumberAdder implements V8MappableMethod {
  @Override
  public V8Value methodToRun(V8Value[] args) {
    long sum = 0;
    for (V8Value val : args) {
      sum += val.toNumber();
    }
    return new V8Value(sum);
  }
}

// ...

V8Runner v8 = new V8Runner();
v8.map(new NumberAdder(), "javaAdd");
long sum = v8.run("javaAdd(6, 3, 12, 17, 4);").toNumber();

// sum == 42
```

Currently, only the Android platform is supported, but I intend to support major desktop platforms as well (Linux/Windows/OS X).
