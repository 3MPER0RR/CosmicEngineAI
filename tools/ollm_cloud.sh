#!/usr/bin/env bash

curl https://ollama.com/api/chat \
-H "Authorization: Bearer $OLLAMA_API_KEY" \
-d '{
"model": "gpt-oss:120b-cloud",
"messages": [{
"role": "user",
"content": "Why is the sky blue?"
}],
"stream": false
}'
