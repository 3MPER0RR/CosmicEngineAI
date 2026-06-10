#include "probe.hpp"
#include <curl/curl.h>
#include <time.h>

static size_t discard(void* ptr, size_t size, size_t nmemb, void* userp) {
    (void)ptr; (void)userp;
    return size * nmemb;
}

float probe_rtt(const char* url) {

    CURL* curl = curl_easy_init();
    if (!curl) return -1.0f;

    clock_t start = clock();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // HEAD-like
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard);

    curl_easy_perform(curl);

    clock_t end = clock();

    curl_easy_cleanup(curl);

    return (float)(end - start) * 1000.0f / CLOCKS_PER_SEC;
}