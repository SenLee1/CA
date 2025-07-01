#include <immintrin.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void impl(int N, int step, double *p) {
    /* Your code here */
    int thread = omp_get_max_threads();
    double *p_next = (double *)aligned_alloc(64, N * N * sizeof(double));
    double *p_free = p_next;
    double *tem = (double *)aligned_alloc(64, N * N * sizeof(double));
    memcpy(p_next, p, N * N * sizeof(double));
    memcpy(tem, p, N * N * sizeof(double));
    double *orign = p;
    p = tem;
    int Block = 8;
    __m256d factor = _mm256_set1_pd(0.25);

    // printf("%d", omp_get_max_threads());
    omp_set_num_threads(thread);
    for (int k = 0; k < step; k++) {
#pragma omp parallel for schedule(dynamic)
        for (int ii = 1; ii < N - 1; ii += Block) {
            for (int jj = 1; jj < N - 1; jj += Block) {
                int i_end = ii + Block < N - 1 ? ii + Block : N - 1;
                int j_end = jj + Block < N - 1 ? jj + Block : N - 1;
                for (int i = ii; i < i_end; i++) {
                    for (int j = jj; j < j_end; j += 8) {
                        if (j + 3 + 4 < j_end) {
                            __m256d top = _mm256_loadu_pd(&p[(i - 1) * N + j]);
                            __m256d bottom = _mm256_loadu_pd(&p[(i + 1) * N + j]);
                            __m256d sum = _mm256_add_pd(top, bottom);
                            __m256d left = _mm256_loadu_pd(&p[i * N + j - 1]);
                            sum = _mm256_add_pd(sum, left);
                            __m256d right = _mm256_loadu_pd(&p[i * N + j + 1]);
                            sum = _mm256_add_pd(sum, right);
                            __m256d avg = _mm256_mul_pd(sum, factor);
                            _mm256_storeu_pd(&p_next[i * N + j], avg);

                            top = _mm256_loadu_pd(&p[(i - 1) * N + j + 4]);
                            bottom = _mm256_loadu_pd(&p[(i + 1) * N + j + 4]);
                            sum = _mm256_add_pd(top, bottom);
                            left = _mm256_loadu_pd(&p[i * N + j - 1 + 4]);
                            sum = _mm256_add_pd(sum, left);
                            right = _mm256_loadu_pd(&p[i * N + j + 1 + 4]);

                            sum = _mm256_add_pd(sum, right);
                            avg = _mm256_mul_pd(sum, factor);

                            _mm256_storeu_pd(&p_next[i * N + j + 4], avg);
                        } else {
                            for (int jj = j; jj < j_end; jj++) {
                                p_next[i * N + jj] =
                                    (p[(i - 1) * N + jj] + p[(i + 1) * N + jj] +
                                     p[i * N + jj + 1] + p[i * N + jj - 1]) *
                                    0.25;
                            }
                        }
                    }
                }
            }
        }
        double *temp = p;
        p = p_next;
        p_next = temp;
    }
    memcpy(orign, p, N * N * sizeof(double));
    free(p_free);
    free(tem);
    p = orign;
}
