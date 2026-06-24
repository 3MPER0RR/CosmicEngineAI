#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bridge.hpp"
#include "http.h"
#include "backend.h"

#include "orchestrator/orchestrator.h"
#include "providers/groq/stream.h"

int main() {

    cpp_init();

    char input[512];

    printf("llmrt> ");

    while (fgets(input, sizeof(input), stdin)) {

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0)
            break;

        /*
         * HTTP backend test
         */

        if (strncmp(input, "http ", 5) == 0) {

            Backend* b = get_best_backend();

            char out[4096];

            http_post(
                b->url,
                "{\"msg\":\"test\"}",
                out,
                4096
            );

            printf("[backend: %s]\n", b->name);
            printf("[latency: %.2f ms]\n", b->last_latency);

            printf("[response]\n%s\n", out);

            printf("llmrt> ");

            continue;
        }

        /*
         * RAW STREAM TEST
         */

        if (strncmp(input, "stream ", 7) == 0) {

            const char* prompt = input + 7;

            groq_stream_prompt(prompt);

            printf("llmrt> ");

            continue;
        }

        /*
         * NORMAL LLM REQUEST
         */

        if (strncmp(input, "llm ", 4) == 0) {

            const char* prompt = input + 4;

            const char* out =
                orchestrate_text_prompt(prompt);

            printf(
                "[groq]\n%s\n",
                out ? out : "error"
            );

            printf("llmrt> ");

            continue;
        }

        /*
         * IMAGE BACKEND
         */

        if (strncmp(input, "img ", 4) == 0) {

            const char* prompt = input + 4;

            printf(
                "[image backend] %s\n",
                prompt
            );

            system("python3 tools/python/run_img.py");

            printf("llmrt> ");

            continue;
        }

        /*
         * OLLAMA BACKEND
         */

        if (strncmp(input, "ollama ", 7) == 0) {

            const char* prompt = input + 7;

            printf(
                "[ollama backend] %s\n",
                prompt
            );

            printf("llmrt> ");

            continue;
        }

        printf("echo: %s\n", input);

        printf("llmrt> ");
    }

    return 0;
}