#include "stream.h"
#include "../../telemetry/telemetry.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static telemetry_t g_tel;

void telemetry_start_stream(void)
{
    memset(&g_tel, 0, sizeof(g_tel));
    telemetry_start(&g_tel);
}

void stream_handle_delta(const char* delta, const char* model)
{
    (void)delta;
    (void)model;
}

void stream_finalize(const char* model)
{
    (void)model;
}

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    size_t len = size * nmemb;
    (void)userdata;

    if (len > 0) {
        fwrite(ptr, 1, len, stdout);
        fflush(stdout);
    }

    return len;
}

int groq_stream_prompt(const char* prompt)
{
    const char* api = getenv("GROQ_API_KEY");
    if (!api) {
        fprintf(stderr, "missing GROQ_API_KEY\n");
        return -1;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        return -1;
    }

    char payload[4096];
    snprintf(
        payload,
        sizeof(payload),
        "{"
        "\"model\":\"openai/gpt-oss-120b\","
        "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],"
        "\"stream\":true"
        "}",
        prompt
    );

    struct curl_slist* headers = NULL;

    char auth[512];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", api);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth);

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "\n[curl error] %s\n", curl_easy_strerror(res));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK) ? 0 : -1;
}
