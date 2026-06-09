#include "backend.h"
#include <string.h>

static Backend backends[] = {
    {"local", "http://localhost:8000", 50.0f, 0.0f},
    {"remote", "http://example.com", 120.0f, 0.0f}
};

static int count = 2;

Backend* get_best_backend() {
    Backend* best = &backends[0];

    for (int i = 0; i < count; i++) {
        backends[i].score = 1000.0f / (backends[i].latency_ms + 1);

        if (backends[i].score > best->score)
            best = &backends[i];
    }

    return best;
}

void update_backend_stats(const char* name, float latency) {
    for (int i = 0; i < count; i++) {
        if (strcmp(backends[i].name, name) == 0) {
            backends[i].latency_ms =
                backends[i].latency_ms * 0.8f + latency * 0.2f;
        }
    }
}