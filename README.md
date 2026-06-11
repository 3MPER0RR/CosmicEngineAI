## Cosmic Engine

Cosmic Engine

Multi-provider AI runtime in C/C++ with experimental Python image pipeline.

Overview

Cosmic Engine is a lightweight AI orchestration runtime written in C/C++.
It currently supports:

LLM inference via Groq API
HTTP backend abstraction layer
Experimental image generation via Hugging Face (Python + FLUX)
Stubbed local backend for future Ollama integration

The system is designed as a modular engine with separated providers and a minimal core runtime.


export HF_TOKEN="hf_Hjxxxxxx"  huggingface token

GROQ_API_KEY=xxx ./llmrt  groq apikey
