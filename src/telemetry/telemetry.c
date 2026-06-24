#include "telemetry.h"
#include <string.h>
#include <time.h>

/* =========================
 * BASIC TELEMETRY
 * ========================= */

long telemetry_diff_ms(struct timespec a, struct timespec b)
{
    return (b.tv_sec - a.tv_sec) * 1000 +
           (b.tv_nsec - a.tv_nsec) / 1000000;
}

void telemetry_start(telemetry_t* t)
{
    memset(t, 0, sizeof(*t));
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

void telemetry_first_token(telemetry_t* t)
{
    if (t->first_token)
        return;

    clock_gettime(CLOCK_MONOTONIC, &t->first);
    t->first_token = 1;
}

/* =========================
 * BACKEND WINDOW ANALYTICS
 * ========================= */

void telemetry_push(TelemetryWindow* w, float v)
{
    if (!w) return;

    w->values[w->index % 64] = v;
    w->index++;

    if (w->size < 64)
        w->size++;
}

float telemetry_avg(TelemetryWindow* w)
{
    if (!w || w->size == 0)
        return 0.0f;

    float sum = 0.0f;

    for (int i = 0; i < w->size; i++)
        sum += w->values[i];

    return sum / (float)w->size;
}

float telemetry_jitter(TelemetryWindow* w)
{
    if (!w || w->size < 2)
        return 0.0f;

    float mean = telemetry_avg(w);
    float var = 0.0f;

    for (int i = 0; i < w->size; i++) {
        float d = w->values[i] - mean;
        var += d * d;
    }

    return var / (float)w->size;
}