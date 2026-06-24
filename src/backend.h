#pragma once

#include "telemetry/telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Backend {
    const char* name;
    const char* url;

    float last_latency;
    float avg_latency;
    float score;

    TelemetryWindow latency_window;
} Backend;

Backend* get_best_backend();
void update_backend_stats(const char* name, float latency);

#ifdef __cplusplus
}
#endif