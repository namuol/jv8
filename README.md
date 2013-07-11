# JV8

A simple way to run V8 JavaScript instances from Java.

## Example

```java
V8Runner v8 = new V8Runner();

v8.map("javaSum", new V8MappableMethod {
  @Override
  public V8Value methodToRun(V8Value[] args) {
    double sum = 0;
    for (V8Value val : args) {
      sum += val.toNumber();
    }
    return new v8.val(sum);
  }
});

double sum = v8.runJS("(program)", "javaSum(6, 3, 12, 17, 4);").toNumber();

// sum == 42.0
```

## Supported types

 - `Boolean`
 - `Number`
 - `String`
 - `Function`
 
## Function passing and calling

Javascript can pass a Function to the Java context, and Java can call this function back.

```java

v8.map("doSomethingWithFunction", new V8MappableMethod {
  @Override
  public V8Value methodToRun(V8Value[] args) {
  
    // Get the function from JS parameters
    V8Function function = (V8Function)args[0];
    
    // Create a table of arguments to pass
    V8Value newArgs[] = { new V8Number(8) };
    
    // Execute the function with one argument
    V8Value result = v8.callFunction(function, newArgs);
    
    // Get the result
    if (result.isNumber()) {
      double number = result.toNumber(); // 64
    }    
  }
});

v8.runJS("(program)", "doSomethingWithFunction(function(n){ return n * n; })");
```

## Platform support


Currently, only the Android platform is supported, but I intend to support major desktop platforms as well (Linux/Windows/OS X).

This **pre-alpha** software still in highly experimental stages. **Do not use this in production software.**

## Binaries
A nightly(ish) Android build can be downloaded [here](http://static.jovianware.com/jv8/jv8_android_arm.tar.gz).

The tarball should be extracted into the root of a standard android project, and contains the following:

```
libs/
  jv8.jar
  armeabi/
    gdbserver # FOR DEBUG
    gdb.setup # FOR DEBUG
    libgnustl_shared.so
    libjv8.so
```

Note: These are debug-only builds for now; i.e. not-optimized and containing debug symbols.

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

Have a snack while you wait. This will result in 3 essential files:

```
dist/
jv8.tar.gz/
  libs/
    jv8.jar               # Java code
    armeabi/
      libgnustl_shared.so # Android C++ STL support 
      libjv8.so           # JV8 JNI code
```

There will also be GDB stuff in here; the build script does not generate production
binaries, currently, and I Do Not Recommend using this in production.
