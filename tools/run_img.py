import os
import sys
import requests

API_URL = "https://router.huggingface.co/hf-inference/models/black-forest-labs/FLUX.1-schnell"

headers = {
    "Authorization": f"Bearer {os.environ['HF_TOKEN']}",
}

prompt = sys.argv[1]

payload = {
    "inputs": prompt
}

r = requests.post(
    API_URL,
    headers=headers,
    json=payload
)

with open("out.png", "wb") as f:
    f.write(r.content)

print("saved -> out.png")
