#!/usr/bin/env python3
from flask import Flask, render_template, request
import socket

app = Flask(__name__)

clients = []

@app.route('/')
def index():
    return render_template('/var/www/html/index.html')

@app.route('/control', methods=['POST'])
def control():
    direction = request.form['direction']
    send_command(direction)
    return 'OK', 200

def send_command(command):
    for client in clients:
           try:
               client.sendall(command.encode())
           except Exception as e:
                print("Error sending command to client: {e}")

def start_tcp_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('10.16.230.251', 7001))
    server_socket.listen(5)
    print("TCP server listening on port 7001")

    while True:
        client_socket, addr = server_socket.accept()
        print(f"Accepted connection from {addr}")
        clients.append(client_socket)

if __name__ == '__main__':
    from threading import Thread
    Thread(target=start_tcp_server).start()
    app.run(host='0.0.0.0', port=7000)