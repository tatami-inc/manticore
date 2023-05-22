# Testing manticore execution

Here, we test **manticore** on environments that rely on main thread execution for correctness.

R is our motivating example where any evaluation of R code from C++ (via **Rcpp**) must be done on the main thread.
It is hard to determine exactly why the main thread is required here,
but we observe segmentation faults if we attempt to run R code in a serial section of a worker thread, so there you have it.
The [`R`](R/) subdirectory contains an R package with C++ functions that interact with the R API from worker threads.
This can be installed and tested with:

```r
R CMD INSTALL R/
cd R/tests && R -f testthat.R
```

More tests to come as needed.
Python is probably a good candidate if the GIL must be respected for arbitrary Python code.
