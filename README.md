# Main thread function executor

## Overview

This repository implements a C++ class that allows worker threads to pass functions for execution on the main thread.
It is intended for non-thread-safe third-party code where locking is insufficient, e.g., due to garbage collection.
For example, we can use **manticore** to allow workers to execute R code that might trigger GC.
This is useful for parallelizing functions that need to occasionally - but safely - call the R APIs.

## Quick start

The **manticore** function is based around the `Executor` class, which should be used like below:

```cpp
#include "manticore/manticore.hpp"
#include <thread>
#include <vector>

// For a given number of threads...
manticore::Executor mexec;
mexec.initialize(nthreads);

std::vector<std::thread> jobs;
jobs.reserve(nthreads);

for (int t = 0; t < nthreads; ++t) {
    jobs.emplace_back([&](int thread) -> void {
        mexec.run([&]() -> void { /* do something on the main thread */ });

        // now do something in the worker thread

        mexec.run([&]() -> void { /* do another thing on the main thread */ });

        // and so on, until...
        mexec.finish_thread();
    }, t);
}

mexec.listen();
for (auto& j : jobs) {
    j.join();
}
```

The above code initializes the `Executor` and launches worker threads that request main thread execution via `run()`.
Meanwhile, the main thread is listening for worker requests via `listen()`.
This blocks until all workers call `finish_thread()`, at which point the main thread is allowed to proceed.

Check out the [reference documentation](https://tatami-inc.github.io/manticore) for more details.

## Create a global `Executor`

Sometimes, the `Executor::run()` function needs to be called from deep inside another library, with no opportunity to pass the actual `Executor` object through the library's interface.
(Looking at you, [**tatami_r**](https://github.com/tatami-inc/tatami_r).)
In such cases, we should create a global `Executor` object that can be called from anywhere.
For standard source files, we can use `extern` linkage, while for header-only libraries, we can use `static` getters:

```cpp
auto& executor() {
    static manticore::Executor mexec;
    return mexec;
}
```

This allows us to do:

```cpp
void some_function() {
    // Do various things.

    auto& mexec = executor();
    mexec.run([&]() -> void { /* Something that must be done on the main thread. */ });

    // More things.
}
```

`some_function()` can now be called inside a parallel context:

```cpp

auto& mexec = executor();
mexec.initialize(2);

for (int t = 0; t < nthreads; ++t) {
    jobs.emplace_back([&](int thread) -> void {
        some_function();
        mexec.finish_thread();
    }, t);
}

mexec.listen();
for (auto& j : jobs) {
    j.join();
}
```

Note that `some_function()` can _also_ be called outside of a parallel context, i.e. without running `initialize()`, `finish_thread()` or `listen()`.
In such cases, it will run directly on the main thread, allowing developers to re-use the same function in serial applications.

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  manticore 
  GIT_REPOSITORY https://github.com/tatami-inc/manticore
  GIT_TAG master # or any version of interest 
)

FetchContent_MakeAvailable(manticore)
```

Then you can link to **manticore** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe manticore)

# For libaries
target_link_libraries(mylib INTERFACE manticore)
```

### CMake using `find_package()`

You can install the library by cloning a suitable version of this repository and running the following commands:

```sh
mkdir build && cd build
cmake ..
cmake --build . --target install
```

Then you can use `find_package()` as usual:

```cmake
find_package(tatami_manticore CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE tatami::manticore)
```

### Manual

If you're not using CMake, the simple approach is to just copy the files - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
