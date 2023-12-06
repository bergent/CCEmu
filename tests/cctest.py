import requests
import rstr
import sys
import json
import subprocess

from time import sleep

def read_json(path: str) -> dict:
    config_file = open(path)
    data = json.load(config_file)

    server_params = {"host" : str(data["server"]["host"]),
                     "port" : str(data["server"]["port"])}

    return server_params


def generate_number() -> str:
    number = rstr.xeger(r'\+?[7-8]\d{10}')
    return number


def request_loop(url : str, num_requests : int, delay_ms : int) -> int:
    for i in range(num_requests):
        sleep(delay_ms / 1000)
        request_url = url + generate_number()

        try: 
            response = requests.get(request_url)
            print(f"Sent request: {request_url}")
            print(f"Got response: {response.text}\n\n")
        except:
            print("ERROR: can't connect to the server")
            return 1
        
    return 0

def main(argv: list) -> int:
    num_requests = argv[1]
    delay_ms = argv[2]
    
    if (not num_requests.isnumeric()):
        print("ERROR: num_requests should be integer value!")
        return 1
    
    if (not delay_ms.isnumeric()):
        print("ERROR: delay_ms should be integer value!")
        return 1

    config = read_json("../config.json") 
    if (len(config) != 2):
        print("ERROR: failed to read JSON config file")
    url = "http://" + config["host"] + ':' + config["port"] + '/'

    if (request_loop(url, int(num_requests), int(delay_ms)) == 1):
        return 1

    return 0


if __name__ == "__main__":
    if (len(sys.argv) != 3):
        print("Usage: python3 cctest.py num_requests delay_ms")
        exit(1)
    else:
        exit_code = main(sys.argv)

        if (exit_code == 1):
            print("Test finished with an error!")
        else:
            print("Test was completed successfully!")