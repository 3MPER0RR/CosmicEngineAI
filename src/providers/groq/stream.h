#ifndef GROQ_STREAM_H
#define GROQ_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

void telemetry_start_stream(void);
int groq_stream_prompt(const char* prompt);
void stream_handle_delta(const char* delta, const char* model);
void stream_finalize(const char* model);

#ifdef __cplusplus
}
#endif

#endif


