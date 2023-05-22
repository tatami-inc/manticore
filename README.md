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

## Building projects 

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

If you're not using CMake, the simple approach is to just copy the files - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
