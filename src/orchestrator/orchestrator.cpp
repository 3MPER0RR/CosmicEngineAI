#include "orchestrator/orchestrator.hpp"

#include <curl/curl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char response[8192];
static char content_buffer[4096];

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* user)
{
    size_t total = size * nmemb;

    strncat(
        (char*)user,
        (const char*)ptr,
        total
    );

    return total;
}

static const char* extract_content(const char* json)
{
    const char* key = "\"content\":\"";

    const char* start = strstr(json, key);

    if (!start)
        return json;

    start += strlen(key);

    size_t j = 0;

    while (*start && j < sizeof(content_buffer) - 1) {

        /* fine stringa JSON */
        if (*start == '"' && *(start - 1) != '\\')
            break;

        /* unescape minimale */
        if (*start == '\\') {

            start++;

            if (!*start)
                break;

            switch (*start) {

                case 'n':
                    content_buffer[j++] = '\n';
                    break;

                case 't':
                    content_buffer[j++] = '\t';
                    break;

                case '"':
                    content_buffer[j++] = '"';
                    break;

                case '\\':
                    content_buffer[j++] = '\\';
                    break;

                default:
                    content_buffer[j++] = *start;
                    break;
            }

            start++;
            continue;
        }

        content_buffer[j++] = *start++;
    }

    content_buffer[j] = '\0';

    return content_buffer;
}

const char* orchestrate_text_prompt(const char* input)
{
    response[0] = '\0';

    const char* key = getenv("GROQ_API_KEY");

    if (!key)
        return "ERROR: GROQ_API_KEY not set";

    CURL* curl = curl_easy_init();

    if (!curl)
        return "ERROR: curl init failed";

    char payload[4096];

    snprintf(
        payload,
        sizeof(payload),
        "{"
        "\"model\":\"llama-3.3-70b-versatile\","
        "\"messages\":["
        "{"
        "\"role\":\"user\","
        "\"content\":\"%s\""
        "}"
        "]"
        "}",
        input
    );

    struct curl_slist* headers = NULL;

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    char auth_header[512];

    snprintf(
        auth_header,
        sizeof(auth_header),
        "Authorization: Bearer %s",
        key
    );

    headers = curl_slist_append(
        headers,
        auth_header
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        "https://api.groq.com/openai/v1/chat/completions"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        payload
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        write_cb
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        response
    );

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
        return "ERROR: request failed";

    return extract_content(response);
}
