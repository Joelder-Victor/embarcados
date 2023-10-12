from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import JSONResponse

import hashlib
import secrets

app = FastAPI()

# Armazenamento temporário para as chaves geradas
keys = set()
keys.add("CHAVESECRETA1")

def generate_key():
    """Gera uma chave de 12 caracteres."""
    return hashlib.sha256(secrets.token_bytes(16)).hexdigest()[:12]

@app.get("/get_key")
def get_key():
    key = generate_key()
    keys.add(key)
    return {"key": key}

@app.post("/check_key")
async def check_key(request: Request):
    data = await request.json()
    key = data.get('key')

    if key in keys:
        return JSONResponse(content={"message": "Chave válida"}, status_code=200)
    else:
        raise HTTPException(status_code=404, detail="Chave inválida")
