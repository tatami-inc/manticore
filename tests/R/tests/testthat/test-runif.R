# library(testthat); library(manticore.tests); source("setup.R"); source("test-matrix.R")

test_that("parallel runif calls work as expected", {
    {
        set.seed(99)
        n <- num.threads(2)
        out <- parallel_runif(n, 10, 20)
        expect_identical(length(out), n)
        expect_identical(unique(lengths(out)), 10L)
        expect_identical(unique(lengths(unlist(out, recursive=FALSE))), 20L)

        set.seed(99)
        combined <- runif(n * 10 * 20)
        expect_identical(sort(unlist(out)), sort(combined))
    }

    {
        set.seed(999)
        n <- num.threads(3)
        out <- parallel_runif(n, 5, 50)
        expect_identical(length(out), n)
        expect_identical(unique(lengths(out)), 5L)
        expect_identical(unique(lengths(unlist(out, recursive=FALSE))), 50L)

        set.seed(999)
        combined <- runif(n * 5 * 50)
        expect_identical(sort(unlist(out)), sort(combined))
    }

    {
        set.seed(9999)
        n <- num.threads(8)
        out <- parallel_runif(n, 20, 100)
        expect_identical(length(out), n)
        expect_identical(unique(lengths(out)), 20L)
        expect_identical(unique(lengths(unlist(out, recursive=FALSE))), 100L)

        set.seed(9999)
        combined <- runif(n * 20 * 100)
        expect_identical(sort(unlist(out)), sort(combined))
    }
})
