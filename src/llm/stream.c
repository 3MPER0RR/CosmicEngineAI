#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>

#include "stream.h"

typedef struct {
    struct timespec start;
    struct timespec first_token;
    int first_seen;

    long tokens_estimated;
} telemetry_t;

static telemetry_t t = {0};

static long diff_ms(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) * 1000 +
           (b.tv_nsec - a.tv_nsec) / 1000000;
}

/*
 * crude token estimator (good enough for telemetry)
 */
static void estimate_tokens(const char* s) {
    long tokens = 0;

    while (*s) {
        if (*s == ' ')
            tokens++;
        s++;
    }

    t.tokens_estimated += (tokens / 2);
}

/*
 * extract delta.content but KEEP RAW CHUNKS
 */
static void handle_json(const char* line) {

    const char* p = strstr(line, "\"content\":\"");
    if (!p) return;

    p += strlen("\"content\":\"");

    if (!t.first_seen) {
        clock_gettime(CLOCK_MONOTONIC, &t.first_token);
        t.first_seen = 1;

        printf("\n[TTFB: %ld ms]\n\n",
            diff_ms(t.start, t.first_token));
    }

    printf("%.*s", 2000, p); // raw stream view

    estimate_tokens(p);
}

/*
 * STREAM CALLBACK
 */
static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {

    size_t len = size * nmemb;
    char* data = (char*)ptr;

    fwrite(data, 1, len, stdout); // RAW VIEW

    char* line = strtok(data, "\n");

    while (line) {

        if (strstr(line, "\"content\"")) {
            handle_json(line);
        }

        if (strstr(line, "usage")) {
            printf("\n\n[TOKENS ~ %ld]\n", t.tokens_estimated);
        }

        line = strtok(NULL, "\n");
    }

    return len;
}

int groq_stream_prompt(const char* prompt) {

    const char* api = getenv("GROQ_API_KEY");

    if (!api) return -1;

    memset(&t, 0, sizeof(t));
    clock_gettime(CLOCK_MONOTONIC, &t.start);

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

    printf("\n[STREAM START]\n");

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    printf("\n\n[STREAM END]\n");
    printf("[TOKENS ESTIMATED: %ld]\n", t.tokens_estimated);

    return 0;
}