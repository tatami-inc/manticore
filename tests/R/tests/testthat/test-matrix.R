# library(testthat); library(manticore.tests); source("setup.R"); source("test-matrix.R")

test_that("parallel matrix formation works as expected", {
    n <- num.threads(2)
    out <- parallel_matrix(n, 100, 200)
    expect_identical(length(out), n)
    for (i in seq_along(out)) {
        expect_identical(out[[i]], matrix(i*1, 100, 200))
    }

    n <- num.threads(3)
    out <- parallel_matrix(n, 50, 20)
    expect_identical(length(out), n)
    for (i in seq_along(out)) {
        expect_identical(out[[i]], matrix(i*1, 50, 20))
    }

    n <- num.threads(8)
    out <- parallel_matrix(n, 10, 90)
    expect_identical(length(out), n)
    for (i in seq_along(out)) {
        expect_identical(out[[i]], matrix(i*1, 10, 90))
    }
})

test_that("serial matrix formation works as expected", {
    out <- parallel_matrix(1L, 100, 200)
    expect_identical(length(out), 1L)
    for (i in seq_along(out)) {
        expect_identical(out[[i]], matrix(i*1, 100, 200))
    }

    expect_identical(out, parallel_matrix(0L, 100, 200))
})

