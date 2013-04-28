# JV8

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
v8.map("javaAdd", new NumberAdder());
long sum = v8.runJS("javaAdd(6, 3, 12, 17, 4);").toNumber();

// sum == 42
```

Currently, only the Android platform is supported, but I intend to support major desktop platforms as well (Linux/Windows/OS X).

This **pre-alpha** software still in highly experimental stages.


## Building

### Requirements
You need to be able to build V8 for Android.
- svn
- git
- python
- More stuff I'm probably not aware of.

### Instructions
First, you must initialize and pull down the v8 git submodule:
```
# From the root of the project folder:
git submodule init
git submodule update # This will take a while.
```

Now use the (hackish) build script to create everything you need:
```
./build.sh -j4 # Use the `-h` flag to see options
```

Have a snack while you wait. This will result in 3 key files:
```
dist/
  jv8.jar
libs/armeabi/
  libgnustl_shared.so
  libjv8.so
```

There will also be GDB stuff in here; the build script does not generate production
binaries, currently, and I Do Not Recommend using this in production.
