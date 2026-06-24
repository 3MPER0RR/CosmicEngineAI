#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>

#include "stream.h"

typedef struct {
    struct timespec start;
    struct timespec first;

    int first_token;

    long tokens_estimate;

    char model[128];
    long created;
} ctx_t;

static ctx_t ctx = {0};

static long ms_diff(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) * 1000 +
           (b.tv_nsec - a.tv_nsec) / 1000000;
}

/*
 * crude token estimation (you will replace later)
 */
static void estimate_tokens(const char* s) {
    while (*s) {
        if (*s == ' ')
            ctx.tokens_estimate++;
        s++;
    }
    ctx.tokens_estimate /= 2;
}

/*
 * extract only content
 */
static void extract_content(const char* json) {

    const char* p = strstr(json, "\"content\":\"");
    if (!p) return;

    p += strlen("\"content\":\"");

    if (!ctx.first_token) {
        clock_gettime(CLOCK_MONOTONIC, &ctx.first);
        ctx.first_token = 1;

        printf("\n[TTFB: %ld ms]\n",
            ms_diff(ctx.start, ctx.first));
    }

    /* TEXT LAYER */
    printf("%s", "");

    while (*p && *p != '"') {
        putchar(*p);
        p++;
    }

    estimate_tokens(json);
}

/*
 * RAW + structured separation
 */
static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {

    size_t len = size * nmemb;

    char* data = (char*)ptr;

    /* =========================
     * LAYER 0: RAW STREAM
     * ========================= */
    fwrite(data, 1, len, stdout);

    /* =========================
     * LAYER 1: parse lines
     * ========================= */
    char buffer[8192];

    if (len >= sizeof(buffer))
        len = sizeof(buffer) - 1;

    memcpy(buffer, data, len);
    buffer[len] = 0;

    char* line = strtok(buffer, "\n");

    while (line) {

        if (strstr(line, "\"model\"")) {
            const char* m = strstr(line, "\"model\":\"");
            if (m) {
                m += 10;
                sscanf(m, "%127[^\"]", ctx.model);
            }
        }

        if (strstr(line, "\"created\"")) {
            const char* c = strstr(line, "\"created\":");
            if (c) {
                ctx.created = atol(c + 10);
            }
        }

        if (strstr(line, "\"content\"")) {
            extract_content(line);
        }

        line = strtok(NULL, "\n");
    }

    return len;
}

int groq_stream_prompt(const char* prompt) {

    const char* api = getenv("GROQ_API_KEY");

    if (!api) return -1;

    memset(&ctx, 0, sizeof(ctx));
    clock_gettime(CLOCK_MONOTONIC, &ctx.start);

    CURL* curl = curl_easy_init();

    char payload[4096];

    snprintf(payload, sizeof(payload),
        "{"
        "\"model\":\"llama-3.3-70b-versatile\","
        "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],"
        "\"stream\":true"
        "}", prompt);

    struct curl_slist* headers = NULL;

    char auth[512];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", api);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth);

    curl_easy_setopt(curl, CURLOPT_URL,
        "https://api.groq.com/openai/v1/chat/completions");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    printf("\n[STREAM RAW + OBSERVABILITY]\n");

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    /* =========================
     * LAYER 2: FINAL METRICS
     * ========================= */
    printf("\n\n[METRICS]\n");

    printf("model: %s\n", ctx.model);
    printf("created: %ld\n", ctx.created);
    printf("tokens_est: %ld\n", ctx.tokens_estimate);

    return 0;
}