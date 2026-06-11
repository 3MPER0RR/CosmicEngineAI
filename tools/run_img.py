import os
import requests
import io
from PIL import Image

API_URL = "https://router.huggingface.co/hf-inference/models/black-forest-labs/FLUX.1-schnell"

headers = {
    "Authorization": f"Bearer {os.environ['HF_TOKEN']}",
}

def query(prompt):
    r = requests.post(API_URL, headers=headers, json={"inputs": prompt})
    if r.status_code != 200:
        print(r.text)
        return None
    return r.content

img = query("cyberpunk AI control room")

if img:
    image = Image.open(io.BytesIO(img))
    image.save("out.png")
    print("saved out.png")
