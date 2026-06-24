#include "backend.h"
#include "telemetry/telemetry.h"
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

/* =========================
 * UPDATE STATS
 * ========================= */

void update_backend_stats(const char* name, float latency)
{
    for (int i = 0; i < count; i++) {

        Backend* b = &backends[i];

        if (strcmp(b->name, name) == 0) {

            b->last_latency = latency;

            b->avg_latency =
                b->avg_latency * 0.8f + latency * 0.2f;

            /* ensure window initialized */
            telemetry_push(&b->latency_window, latency);
        }
    }
}

/* =========================
 * BEST BACKEND SELECTION
 * ========================= */

Backend* get_best_backend()
{
    Backend* best = &backends[0];

    for (int i = 0; i < count; i++) {

        Backend* b = &backends[i];

        /* safe fallback if window is empty */
        float avg = telemetry_avg(&b->latency_window);
        float jit = telemetry_jitter(&b->latency_window);

        if (avg <= 0.0f)
            avg = (b->avg_latency > 0.0f) ? b->avg_latency : 1000.0f;

        if (jit < 0.0f)
            jit = 0.0f;

        float rtt = probe_rtt(b->url);

        /* probe fallback safety */
        if (rtt <= 0.0f || rtt > 5000.0f)
            rtt = 1000.0f;

        /* =========================
         * SCORING FUNCTION
         * =========================
         * higher score = better backend
         */

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