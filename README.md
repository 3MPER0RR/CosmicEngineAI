## Cosmic Engine

AI runtime written in C/C++ with optional Python tooling for image generation.
Overview
Cosmic Engine is a modular AI orchestration runtime designed to unify multiple inference backends under a single C/C++ core.
It currently supports:
LLM inference via Groq API
HTTP backend abstraction layer
Stub interface for local model execution (Ollama integration planned)
The architecture is designed around a minimal core engine with pluggable providers.

## Requirements

System

clang or gcc

make

libcurl

Python (optional, image pipeline only)

Python ≥ 3.9

requests

pillow

## Configuration

Groq API Key (required)

huggingface token ( required )

export HF_TOKEN="hf_Hjxxxxxx" huggingface token

GROQ_API_KEY=xxx ./llmrt


## usage 

./llmrt llm prompt 

image generator new terminal 

use menu.sh setup python venv

source venv venv/bin/activate

python3 run_img.py prompt 
