#include "http.h"
#include <curl/curl.h>
#include <string.h>

struct buffer {
    char* data;
    int len;
};

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* user) {
    struct buffer* b = (struct buffer*)user;
    int total = size * nmemb;

    if (b->len + total < 4096) {
        memcpy(b->data + b->len, ptr, total);
        b->len += total;
        b->data[b->len] = 0;
    }

    return total;
}

void http_post(const char* url, const char* payload, char* out, int out_size) {
    CURL* curl = curl_easy_init();
    if (!curl) return;

    struct buffer buf = { out, 0 };
    buf.data[0] = 0;

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

    curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}