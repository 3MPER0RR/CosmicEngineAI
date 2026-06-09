#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bridge.hpp"
#include "http.h"
#include "backend.h"

int main() {
    cpp_init();

    char input[256];

    printf("llmrt> ");

    while (fgets(input, sizeof(input), stdin)) {

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        }

        // TEST HTTP + backend routing
        if (strncmp(input, "http ", 5) == 0) {

            Backend* b = get_best_backend();

            char out[4096];

            clock_t start = clock();

            http_post(
                b->url,
                "{\"msg\":\"test\"}",
                out,
                4096
            );

            clock_t end = clock();

            float latency_ms =
                (float)(end - start) / CLOCKS_PER_SEC * 1000.0f;

            update_backend_stats(b->name, latency_ms);

            printf("[backend: %s]\n", b->name);
            printf("[latency: %.2f ms]\n", latency_ms);
            printf("[response]\n%s\n", out);

            printf("llmrt> ");
            continue;
        }

        printf("echo: %s\n", input);
        printf("llmrt> ");
    }

    return 0;
}