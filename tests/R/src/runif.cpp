#include "Rcpp.h"
#include "manticore/manticore.hpp"
#include <thread>

//' @export
//[[Rcpp::export(rng=false)]]
Rcpp::List parallel_runif(int nthreads, int iterations, int number) {
    auto statenv = Rcpp::Environment::namespace_env("stats");
    Rcpp::Function runif = statenv["runif"];
    auto nsim = Rcpp::IntegerVector::create(number);

    manticore::Executor mexec;
    std::vector<Rcpp::List> raw_output;

    if (nthreads == 0) {
        raw_output.resize(1);
        raw_output[0] = Rcpp::List(iterations);

        for (int i = 0; i < iterations; ++i) {
            mexec.run([&]() -> void {
                raw_output[0][i] = runif(nsim);
            });
        }

    } else {
        mexec.initialize(nthreads);

        raw_output.resize(nthreads);
        for (int t = 0; t < nthreads; ++t) {
            raw_output[t] = Rcpp::List(iterations);
        }

        std::vector<std::thread> jobs;
        jobs.reserve(nthreads);

        for (int t = 0; t < nthreads; ++t) {
            jobs.emplace_back([&](int thread) -> void {
                for (int i = 0; i < iterations; ++i) {
                    mexec.run([&]() -> void {
                        raw_output[thread][i] = runif(nsim);
                    });
                }
                mexec.finish_thread();
            }, t);
        }

        mexec.listen();
        for (auto& j : jobs) {
            j.join();
        }
    }

    return Rcpp::List(raw_output.begin(), raw_output.end());
}
