from fastapi.routing import APIRouter
from fastapi import HTTPException
from models.models import RequestModelEmail
from fastapi.responses import JSONResponse 
from database.database import execute_fetch_all,execute_query,DATABASE_URL
from databases import Database
router = APIRouter(prefix='/app',tags=['app'])

emails = set()
database = Database(DATABASE_URL)

@router.on_event("startup")
async def startup_db_client():
    await database.connect()

# Desconectar do banco de dados
@router.on_event("shutdown")
async def shutdown_db_client():
    await database.disconnect()

@router.post("/get_key")
async def get_key(request:RequestModelEmail):
    email = request.email
    query = f"""
        select
	        u.key
        from
	        "security".users u
        where
	        u.email = '{email}'
        """
    result = await execute_query(database,query)

    try:

        if result is None:
            return HTTPException(status_code=404, detail="Email not registered")
        else:
            return JSONResponse(content={"key": result}, status_code=200)
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
    
@router.post("/get_doors")
async def get_doors(request:RequestModelEmail):
    email = request.email
    
    query = f"""SELECT 
	    d.door_description
    FROM
	    "security".users u
    JOIN "security".bonds b on
	    b.id_user = u.id_user
    JOIN "security".doors d on
	    d.id_door = b.id_door
    WHERE
	    u.email = '{email}' """
    
    result = await execute_fetch_all(database,query)
    return result
    
@router.post("/check_email")
async def check_email(request:RequestModelEmail):
    email = request.email

    if email in emails:
        return JSONResponse(content={"message": "Valid Email."}, status_code=200) 
    query = f"SELECT email FROM security.users WHERE email = '{email}';"
    result = await execute_query(database,query)
    
    try:

        if result is None:
            return HTTPException(status_code=404, detail="Invalid Email")
        elif str(result).rstrip() == email:
            emails.add(str(result).rstrip())
            return JSONResponse(content={"message": "Valid Email"}, status_code=200)
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
