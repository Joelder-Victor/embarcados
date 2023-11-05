from pydantic import BaseModel
from datetime import datetime
class RequestModel(BaseModel):
    key: str
    door_id:str
    # Adicione outros campos que você deseja incluir na requisição

    class Config:
        schema_extra = {
            "example": {
                "key": "example_key",
                "door_id":"door_id"
                # Adicione exemplos de outros campos, se houver
            }
        }

class RequestModelEmail(BaseModel):
    email: str
    # Adicione outros campos que você deseja incluir na requisição

    class Config:
        schema_extra = {
            "example": {
                "email": "email_example",
                # Adicione exemplos de outros campos, se houver
            }
        }



class RequestLogModel(BaseModel):
    key: str
    door_id:str
    hour:datetime
    # Adicione outros campos que você deseja incluir na requisição

    class Config:
        schema_extra = {
            "example": {
                "email": "email_example",
                # Adicione exemplos de outros campos, se houver
            }
        }
