#include "Rcpp.h"
#include "manticore/manticore.hpp"
#include <thread>
#include <vector>

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::List parallel_matrix(int nthreads, int nrow, int ncol) {
    auto statenv = Rcpp::Environment::base_env();
    Rcpp::Function matrix = statenv["matrix"];
    Rcpp::IntegerVector NR = Rcpp::IntegerVector::create(nrow);
    Rcpp::IntegerVector NC = Rcpp::IntegerVector::create(ncol);

    manticore::Executor mexec;
    mexec.initialize(nthreads);

    Rcpp::List output(nthreads);

    std::vector<std::thread> jobs;
    jobs.reserve(nthreads);
    for (int t = 0; t < nthreads; ++t) {
        jobs.emplace_back([&](int thread) -> void {
            mexec.run([&]() -> void {
                auto data = Rcpp::NumericVector::create(thread + 1);
                output[thread] = matrix(data, NR, NC);
            });
            mexec.finish_thread();
        }, t);
    }

    mexec.listen();
    for (auto& j : jobs) {
        j.join();
    }

    return output;
}
