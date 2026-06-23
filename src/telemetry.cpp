#include "telemetry.hpp"
#include <math.h>
#include <string.h>

void telemetry_init(TelemetryWindow* t) {
    memset(t, 0, sizeof(TelemetryWindow));
}

void telemetry_push(TelemetryWindow* t, float value) {
    t->samples[t->idx] = value;
    t->idx = (t->idx + 1) % MAX_SAMPLES;

    if (t->count < MAX_SAMPLES)
        t->count++;
}

float telemetry_avg(TelemetryWindow* t) {
    float sum = 0.0f;

    for (int i = 0; i < t->count; i++)
        sum += t->samples[i];

    return t->count ? sum / t->count : 0.0f;
}

float telemetry_jitter(TelemetryWindow* t) {
    float avg = telemetry_avg(t);
    float var = 0.0f;

    for (int i = 0; i < t->count; i++) {
        float d = t->samples[i] - avg;
        var += d * d;
    }

    return t->count ? var / t->count : 0.0f;
}