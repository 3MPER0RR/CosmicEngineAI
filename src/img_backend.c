#include "img_backend.h"
#include "config.h"
#include "http.h"

#include <stdio.h>
#include <string.h>

/*
 * IMAGE BACKEND - Gemini API (AI Studio)
 * Output: raw JSON response (fase 1)
 */

int img_generate(const char* prompt, char* out, int out_size)
{
    if (!prompt || !out || out_size <= 0)
        return -1;

    /* payload JSON */
    char payload[1024];

    snprintf(payload, sizeof(payload),
        "{"
          "\"contents\": [{"
            "\"parts\": [{"
              "\"text\": \"Generate an image from this prompt: %s\""
            "}]"
          "}]"
        "}",
        prompt
    );

    /* endpoint Gemini */
    char url[512];

    snprintf(url, sizeof(url),
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-image:generateContent?key=%s",
        GEMINI_API_KEY
    );

    /* HTTP request (void function, writes into out buffer) */
    http_post(url, payload, out, out_size);

    return 0;
}