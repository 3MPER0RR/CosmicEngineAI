#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Backend {
    const char* name;
    const char* url;
    float latency_ms;
    float score;
} Backend;

Backend* get_best_backend();
void update_backend_stats(const char* name, float latency);

#ifdef __cplusplus
}
#endif