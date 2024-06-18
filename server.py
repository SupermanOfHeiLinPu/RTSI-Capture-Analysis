import socket
import time

def tcp_server(host='127.0.0.1', port=12345):
    # 创建一个 TCP/IP 套接字
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # 绑定套接字到地址
    server_address = (host, port)
    server_socket.bind(server_address)
    
    # 监听传入的连接
    server_socket.listen(1)
    print(f"Server is listening on {host}:{port}")
    
    while True:
        # 等待连接
        print("Waiting for a connection...")
        connection, client_address = server_socket.accept()
        
        try:
            print(f"Connection from {client_address}")
            
            # 接收数据
            while True:
                data = connection.recv(1024)
                if data:
                    print(f"Received: {data.decode('utf-8')}")
                    # 回送数据
                    connection.sendall(data)
                else:
                    print("No more data from", client_address)
                    break
            
        finally:
            time.sleep(5)
            # 清理连接
            connection.close()

if __name__ == "__main__":
    tcp_server()
