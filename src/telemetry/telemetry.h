#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================
 * BASIC TELEMETRY
 * ========================= */

typedef struct {

    struct timespec start;
    struct timespec first;

    int first_token;
    long tokens_estimate;
    char model[128];

} telemetry_t;

/* =========================
 * BACKEND WINDOW ANALYTICS
 * ========================= */

typedef struct {

    float values[64];
    int index;
    int size;

} TelemetryWindow;

/* BASIC */
void telemetry_start(telemetry_t* t);
void telemetry_first_token(telemetry_t* t);
long telemetry_diff_ms(struct timespec a, struct timespec b);

/* ANALYTICS (used by backend.cpp) */
void telemetry_push(TelemetryWindow* w, float v);
float telemetry_avg(TelemetryWindow* w);
float telemetry_jitter(TelemetryWindow* w);

#ifdef __cplusplus
}
#endif

#endif