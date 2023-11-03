import os
from dotenv import load_dotenv
from databases import Database
from fastapi.routing import APIRouter
from fastapi import FastAPI, HTTPException, Depends, Request

load_dotenv()

# Configuração do banco de dados

DATABASE_URL = os.getenv('DATABASE_URL')



# Função para executar uma consulta
async def execute_query(database:Database,query: str,values:dict = {}):
    try:
        if len(values) ==0:
            result = await database.execute(query)
        else:
            result = await database.execute(query,values)
        return result
    except Exception as e:
        print(e)
        raise HTTPException(status_code=500, detail=str(e))
    
async def execute_fetch_all(database:Database,query:str):
    try:
        result = await database.fetch_all(query)
     
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

