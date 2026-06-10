#pragma once

#define MAX_SAMPLES 16

typedef struct {
    float samples[MAX_SAMPLES];
    int idx;
    int count;

    float avg;
    float jitter;
} TelemetryWindow;

void telemetry_init(TelemetryWindow* t);
void telemetry_push(TelemetryWindow* t, float value);

float telemetry_avg(TelemetryWindow* t);
float telemetry_jitter(TelemetryWindow* t);