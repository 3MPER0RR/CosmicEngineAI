#include <stdio.h>
#include <string.h>

#include "bridge.hpp"
#include "engine.h"

int main() {

    cpp_init();

    char input[256];

    printf("llmrt> ");

    while (fgets(input, sizeof(input), stdin)) {

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0)
            break;

        engine_execute(input);

        printf("llmrt> ");
    }

    return 0;
}