#include "orchestrator/orchestrator.hpp"

#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char response[8192];

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* user) {
    strncat((char*)user, (char*)ptr, size * nmemb);
    return size * nmemb;
}

const char* orchestrate_text_prompt(const char* input) {

    response[0] = 0;

    const char* key = getenv("GROQ_API_KEY");
    if (!key) {
        return "ERROR: GROQ_API_KEY not set";
    }

    CURL* curl = curl_easy_init();
    if (!curl) return "ERROR: curl init failed";

    char payload[4096];

    snprintf(payload, sizeof(payload),
        "{"
        "\"model\":\"llama-3.3-70b-versatile\","
        "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]"
        "}",
        input
    );

    struct curl_slist* headers = NULL;

    headers = curl_slist_append(headers, "Content-Type: application/json");

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header),
        "Authorization: Bearer %s",
        key
    );

    headers = curl_slist_append(headers, auth_header);

    curl_easy_setopt(curl, CURLOPT_URL,
        "https://api.groq.com/openai/v1/chat/completions"
    );

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return "ERROR: request failed";
    }

    return response;
}