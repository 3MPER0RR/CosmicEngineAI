#include "backend.h"
#include "probe.hpp"

#include <string.h>

static Backend backends[] = {
    {
        "local",
        "http://localhost:8000",
        0, 0, 0,
        {}
    },
    {
        "remote",
        "http://example.com",
        0, 0, 0,
        {}
    }
};

static int count = 2;

void update_backend_stats(const char* name, float latency) {
    for (int i = 0; i < count; i++) {

        Backend* b = &backends[i];

        if (strcmp(b->name, name) == 0) {

            b->last_latency = latency;

            b->avg_latency =
                b->avg_latency * 0.8f + latency * 0.2f;

            telemetry_push(&b->latency_window, latency);
        }
    }
}

Backend* get_best_backend() {

    Backend* best = &backends[0];

    for (int i = 0; i < count; i++) {

        Backend* b = &backends[i];

        float avg = telemetry_avg(&b->latency_window);
        float jit = telemetry_jitter(&b->latency_window);

        if (avg <= 0.0f)
            avg = b->avg_latency;

        float rtt = probe_rtt(b->url);

        if (rtt < 0 || rtt > 5000.0f)
            rtt = 1000.0f;

        float score =
            (1000.0f / (avg + 1.0f)) *
            (1.0f / (1.0f + jit)) *
            (1.0f / (1.0f + rtt / 100.0f));

        b->score = score;

        if (score > best->score)
            best = b;
    }

    return best;
}