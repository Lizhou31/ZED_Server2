import socket
import json
import time

def send_json_command(port, commands):
    # Create a socket (SOCK_STREAM means a TCP socket)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to server and send data
        sock.connect(("127.0.0.1", port))
        for command in commands:
            print(json.dumps(command))
            sock.sendall(json.dumps(command).encode('ASCII'))
            if command["Command"] == 5:
                # Receive data from the server and shut down
                received = sock.recv(1024).decode('utf-8')
                print(received)
            time.sleep(0.1) 
    # return received

# Example usage
port = 12002  # Replace with the port your server is listening on
commands = []
commandCreate = {"Command": 0, "Args": ["test","10","15"]} 
commandProbe = {"Command": 1, "Args": ["123"]}
commandStop = {"Command": 4}
commandInfo = {"Command": 5}

commands.append(commandCreate)
commands.append(commandInfo)
commands.append(commandProbe)
commands.append(commandInfo)
commands.append(commandProbe)
commands.append(commandInfo)
commands.append(commandProbe)
commands.append(commandInfo)
commands.append(commandInfo)
commands.append(commandProbe)
commands.append(commandInfo)
commands.append(commandProbe)
commands.append(commandInfo)
commands.append(commandStop)

response = send_json_command(port, commands)
