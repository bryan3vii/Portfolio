# ********************************************************************************
# Author: Bryan Mendoza
# Date: November 10, 2023
# 
# Programming Assignment 2: Building a Web Server
# 
# Description:  HTTP server that can handle simple GET and POST requests to 
# different endpoints. It uses multiple threads to handle incoming client 
# connections and provides a framework for handling different HTTP endpoints.
# 
# Undergrad score: 100.00 / 100.00
# Grad scrore: 89.49 / 100.00
# ****************************** Start of Project 1 ******************************
import socket
import threading
import queue
import urllib.parse
import logging
import sys

# Configure a logging system.
logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
logger = logging.getLogger(__name__)

# Define the server settings
HOST = '0.0.0.0'
PORT = 8080

# Create a queue for incoming client connections
client_queue = queue.Queue()

# Create a thread-safe flag for the kill switch
kill_switch = threading.Event()

# HTTP response to the client with the specified status code
# HTTP responses over a socket 
def send_response(socket, status_code, headers, content):
    response = f'HTTP/1.1 {status_code}\r\n'
    for key, value in headers.items():
        response += f'{key}: {value}\r\n'
    response += '\r\n'
    response += content
    socket.send(response.encode('utf-8'))
    
# converts 'a' and 'b' to integers, multiplies them, and responds with the result. If conversion fails, it sends a "400 Bad Request" along with an error message.
def handle_multiply_post(query_params):
    a = query_params.get('a', [''])[0] 
    b = query_params.get('b', [''])[0]

    try:
        a = int(a)
        b = int(b)
        result = a * b
        return 200, {'Content-Type': 'text/html'}, str(result)
    except ValueError:
        return 400, {'Content-Type': 'text/html'}, 'Bad Request: Both "a" and "b" must be integers'
    
# Part of the Graduate Question ( I did not complete it ) 
def handle_misbehave_client():
    pass
# This function processes client data, decodes it as UTF-8, manages exceptions, logs errors,
#  and closes the client socket – useful in network servers to handle communication issues
def handle_client(client_socket):
    try:
        request_data = client_socket.recv(4096).decode('utf-8')
    except UnicodeDecodeError as decode_err:
        logger.error(f"UnicodeDecodeError: {decode_err}")
        send_response(client_socket, 500, {'Content-Type': 'text/html'}, 'Internal Server Error: Decoding Error')
        client_socket.close()
        return
    except socket.timeout:
        logger.error("Socket timeout")
        client_socket.close()
        return
    except socket.error as err:
        logger.error(f"Socket error: {err}")
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
            client_socket.close()
        except socket.error as shutdown_err:
            logger.error(f"Error during socket shutdown: {shutdown_err}")

    request_lines = request_data.split('\n')
# This part of the code parses HTTP requests, extracting method, path, query, and form data 
# from the URL and body (for POST). It handles malformed requests, sending client errors.
    if not request_lines:
        client_socket.close()
        return

    first_line = request_lines[0].strip()
    parts = first_line.split()

    if len(parts) < 3:
        send_response(client_socket, 400, {'Content-Type': 'text/html'}, 'Bad Request')
        client_socket.close()
        return

    method, path, protocol = parts

    query_params = {}
    if '?' in path:
        path, query_string = path.split('?')
        query_params = urllib.parse.parse_qs(query_string)

    body = request_data.split('\r\n\r\n', 1)
    if len(body) > 1:
        body_data = body[1]
        form_params = urllib.parse.parse_qs(body_data)
        query_params.update(form_params)
# defines HTTP server endpoints, methods, and associated actions—a standard routing method for web servers or apps.
    endpoints = {
        '/': { 
            'GET': (200, {'Content-Type': 'text/html'}, '''
              <!DOCTYPE html>
<html>
<head>
    <title>Welcome to my server</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background-color: #a28089;
            margin: 0;
            padding: 0;
            text-align: center;
            color: #ffffff;
            background-image: url('https://wallpapercave.com/wp/wp11855069.jpg');
            background-size: cover; 
            background-position: center center; 
            background-repeat: no-repeat;
        }
        h1 {
            font-size: 3em;
            margin-top: 10%;
        }
        h2 {
            font-size: 2em;
            margin-bottom: 20px;
        }
        p {
            font-size: 1.2em;
        }
    </style>
</head>
<body>
    <h1>*CS 484 Programing Project #2*</h1>
    <p>HTML WEBPAGE<p>
    <h2>911 Porsche GT3 RS</h2>
    <h3>Author: Bryan Mendoza </h3>
    <p>Images are from "https://wallpapercave.com/w/wp11855069" credits: zaina242424 <p>
    <p>The engineers knew exactly what their objective was: to make the new 911 GT3 RS the most powerful Porsche for the road. 
    Objective achieved. The water-cooled six-cylinder bi-turbo Boxer engine with four-valve technology, 
    variable turbine geometry (VTG) and VarioCam Plus delivers its power through the rear axle.</p>
</body>
</html>

            '''),
        },
        '/index.html': {
            'GET': (200, {'Content-Type': 'text/html'}, '<html><body><h1>Welcome to the index page</h1></body></html>'),
        },
        '/multiply': {
            'POST': handle_multiply_post,
        },
        '/google': {
            'GET': (301, {'Location': 'https://google.com'}, ''),
        },
        '/database.php': {
            'DELETE': (403, {'Content-Type': 'text/html'}, 'Forbidden: Database deletion is not allowed.'),
        },
    }
# This code manages incoming HTTP requests by routing them based on the HTTP method and requested path. 
# It utilizes custom responses from the endpoints dictionary and handles errors for unmatched endpoints.
    if method == 'POST' and path == '/multiply':
        response = handle_multiply_post(query_params)
        status, headers, content = response
        send_response(client_socket, status, headers, content)
    else:
        if path in endpoints:
            if method in endpoints[path]:
                response = endpoints[path][method]
                if callable(response):
                    status, headers, content = response(query_params)
                else:
                    status, headers, content = response
                send_response(client_socket, status, headers, content)
            else:
                send_response(client_socket, 405, {'Content-Type': 'text/html'}, 'Method Not Allowed')
        elif path == '/favicon.ico':
            send_response(client_socket, 404, {'Content-Type': 'text/html'}, 'Not Found')
        else:
            send_response(client_socket, 404, {'Content-Type': 'text/html'}, 'Not Found')

    try:
        client_socket.shutdown(socket.SHUT_RDWR)
        client_socket.close()
    except socket.error as shutdown_err:
        logger.error(f"Error during socket shutdown: {shutdown_err}")

# In this part the code runs a network server using multithreading for 
# the concurrent client handling. The "thread_manager" function monitors
# and manages incoming connections, enabling parallel client processing with a separate thread.
def thread_manager():
    while not kill_switch.is_set():
        try:
            client_socket, addr = client_queue.get(block=True, timeout=1)
            client_handler = threading.Thread(target=handle_client, args=(client_socket,))
            client_handler.start()
        except queue.Empty:
            pass

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(5)

logger.info(f'\nServer is listening on {HOST}:{PORT}')

manager_thread = threading.Thread(target=thread_manager)
manager_thread.start()

# This part of the code keeps accepting client connections, queues them, 
# and processes them until the "kill_switch" is activated, after which it closes the 
# server socket, and indicating the server's shutdown
while not kill_switch.is_set():
    try:
        client_socket, addr = server.accept()
        client_queue.put((client_socket, addr))
    except socket.timeout:
        logger.error("Socket timeout during accept")
        pass

manager_thread.join()

server.close()
# ****************************** END of Project 1 ******************************
