#include "engine.h"
#include <stdio.h>
#include <string.h>

/* core */
#include "backend.h"
#include "http.h"
#include "orchestrator/orchestrator.h"

/* image backend */
#include "img_backend.h"

/* streaming */
#include "providers/groq/stream.h"

/* =========================
   HANDLERS
========================= */

typedef void (*cmd_handler)(const char* input);

typedef struct {
    const char* cmd;
    cmd_handler handler;
} command_entry;

/* forward */
static void handle_llm(const char* input);
static void handle_http(const char* input);
static void handle_img(const char* input);
static void handle_imgx(const char* input);
static void handle_default(const char* input);

/* =========================
   LLM STREAM (GROQ)
========================= */

static void handle_llm(const char* input)
{
    const char* prompt = input + 4;

    /* reset streaming state */
    telemetry_start_stream();

    /* stream execution (SSE pipeline) */
    groq_stream_prompt(prompt);
}

/* =========================
   HTTP TEST
========================= */

static void handle_http(const char* input)
{
    Backend* b = get_best_backend();

    char out[4096];

    http_post(
        b->url,
        "{\"msg\":\"test\"}",
        out,
        sizeof(out)
    );

    printf("[backend: %s]\n", b->name);
    printf("[latency: %.2f ms]\n", b->last_latency);
    printf("[response]\n%s\n", out);
}

/* =========================
   IMG
========================= */

static void handle_img(const char* input)
{
    const char* prompt = input + 4;

    char out[4096];

    if (img_generate(prompt, out, sizeof(out)) == 0) {
        printf("[image]\n%s\n", out);
    } else {
        printf("[image error]\n");
    }
}

/* =========================
   IMGX (LLM → IMG)
========================= */

static void handle_imgx(const char* input)
{
    const char* idea = input + 5;

    /* fallback sync (ok for now) */
    const char* refined = orchestrate_text_prompt(idea);

    if (!refined) {
        printf("[imgx error: llm failed]\n");
        return;
    }

    char out[4096];

    if (img_generate(refined, out, sizeof(out)) == 0) {
        printf("[imgx]\n%s\n", out);
    } else {
        printf("[imgx error: image failed]\n");
    }
}

/* =========================
   DEFAULT
========================= */

static void handle_default(const char* input)
{
    printf("echo: %s\n", input);
}

/* =========================
   REGISTRY
========================= */

static command_entry registry[] = {
    { "llm ",  handle_llm },
    { "http ", handle_http },
    { "img ",  handle_img },
    { "imgx ", handle_imgx }
};

static const int registry_size =
    sizeof(registry) / sizeof(registry[0]);

/* =========================
   CORE
========================= */

void engine_execute(const char* input)
{
    for (int i = 0; i < registry_size; i++) {

        int len = strlen(registry[i].cmd);

        if (strncmp(input, registry[i].cmd, len) == 0) {
            registry[i].handler(input);
            return;
        }
    }

    handle_default(input);
}