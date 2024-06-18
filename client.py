import socket
import time

def tcp_client(host='127.0.0.1', port=12345):
    # 创建一个 TCP/IP 套接字
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # 连接到服务器
    server_address = (host, port)
    print(f"Connecting to {host}:{port}")
    client_socket.connect(server_address)
    
    try:
        # 发送数据
        message = 'This is the message. It will be echoed back.'
        print(f"Sending: {message}")
        client_socket.sendall(message.encode('utf-8'))
        
        # 等待回复
        amount_received = 0
        amount_expected = len(message)
        
        while amount_received < amount_expected:
            data = client_socket.recv(1024)
            amount_received += len(data)
            print(f"Received: {data.decode('utf-8')}")
    
    finally:
        # 清理连接
        time.sleep(5)
        print("Closing connection")
        client_socket.close()

if __name__ == "__main__":
    tcp_client()
