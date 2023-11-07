import random
import string
from datetime import datetime
from databases import Database
from fastapi import HTTPException
from fastapi.routing import APIRouter
from fastapi.responses import JSONResponse 
from models.models import RequestModel,RequestLogModel
from database.database import execute_fetch_all,execute_query,DATABASE_URL
router = APIRouter(prefix='/esp',tags=['esp'])

database = Database(DATABASE_URL)

@router.on_event("startup")
async def startup_db_client():
    await database.connect()

# Desconectar do banco de dados
@router.on_event("shutdown")
async def shutdown_db_client():
    await database.disconnect()

def generate_random_string(length=8):
    characters = string.ascii_letters + string.digits
    return ''.join(random.choice(characters) for _ in range(length))

@router.post("/check_key")
async def check_key(request: RequestModel):
    key = request.key
    door_id = request.door_id
    query = f"""
    SELECT
	    u."key"
    FROM
	    "security".bonds b
    JOIN "security".users u ON
	    u.id_user = b.id_user
    WHERE
	    b.id_door = '{door_id}'
    """
    result = await execute_fetch_all(database,query)
    result =[r['key'] for r in result]
    
    if len(result) == 0:
        raise HTTPException(status_code=404, detail="Port id not registered!")
    
    if key in result:
        return JSONResponse(content={"message": "Valid Key"}, status_code=200)
    else:
        raise HTTPException(status_code=403, detail="This key doesn't open this door!")
   
@router.post('/log_entries')
async def insert_log(request:RequestLogModel):
    key = request.key
    id_door = request.door_id
    hour = request.hour
    id= generate_random_string()
    query = f"""INSERT INTO "security".logs
        (id_log,"key", "hour", id_door)
        VALUES(:id,:key,:hour,:id_door);"""
    
    values = {"id":id,"key":key, "hour":hour,"id_door": id_door}
    try:
        await execute_query(database,query,values)
        return JSONResponse(content={"message": "Log inserted successfully!"}, status_code=200)
    except Exception as e:
        print(e)
        raise HTTPException(status_code=404, detail=f"Error when entering data! {e}")
