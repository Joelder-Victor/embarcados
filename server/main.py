import secrets
import hashlib
from databases import Database
from fastapi.responses import JSONResponse 
from fastapi import FastAPI
from routes import esp_routes
from routes import app_routes


app = FastAPI()

app.include_router(esp_routes.router)
app.include_router(app_routes.router)

@app.get("/")
async def root():
    return JSONResponse(content={"message": "Server On!"}, status_code=200)