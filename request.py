import requests

url = 'http://localhost:8000/check_key'
data = {'key': '6b9d7389bae3'}

response = requests.post(url, json=data)

print(response.json())
