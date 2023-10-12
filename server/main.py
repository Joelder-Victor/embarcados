from pydantic import BaseModel
from fastapi.responses import JSONResponse 
from fastapi import FastAPI, HTTPException, Request
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




class RequestModel(BaseModel):
    key: str
    # Adicione outros campos que você deseja incluir na requisição

    class Config:
        schema_extra = {
            "example": {
                "key": "example_key",
                # Adicione exemplos de outros campos, se houver
            }
        }

@app.post("/check_key")
def check_key(request: RequestModel):
    
    key = request.key
    print('keys: ',keys)
    print('key: ',key)
    if key in keys:
        return JSONResponse(content={"message": "Chave válida"}, status_code=200)
    else:
        raise HTTPException(status_code=404, detail="Chave inválida" + key)
