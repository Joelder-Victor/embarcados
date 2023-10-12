import requests

url = 'https://laser-1.onrender.com/check_key'
data = {'key': 'CHAVESECRETA1'}

response = requests.post(url, json=data)

print(response.json())
