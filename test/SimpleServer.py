# tcp_server
# coding=utf-8
# !/usr/bin/env python

import os
import socket
import time
import threading
import signal
serverIP = "0.0.0.0"
serverPort = 8989
clientSocketList = []  # 放每个客户端的socket

def init_server():
    sockServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockServer.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_addr = (serverIP, serverPort)
    sockServer.bind(server_addr)
    sockServer.listen(10)
    print(server_addr)
    print("The server has started, waiting for the client to connect ......")
    return sockServer


def accept_client(sockServer):
    while True:
        time.sleep(0.1)
        clientSock, addr = sockServer.accept()
        strTime = time.strftime('%Y-%m-%d %H:%M:%S')
        strPrint = 'connected from: {}'.format(addr)
        strPrint = strTime + strPrint
        print(strPrint)
        clientSock.setblocking(0)
        clientSocketList.append([clientSock, addr])


def server_Send(direction):
    while True:
        time.sleep(0.1)
        try:
            sendByte = bytes(direction)
            for clientInfo in clientSocketList:
                currClient, addr = clientInfo
                currClient.sendall(sendByte)
                print("to {}, send <{}> ".format(addr, sendByte))
        except Exception as e:
            clientSocketList.remove(clientInfo)
            continue


def server_Recv():
    while True:
        time.sleep(0.1)
        for clientInfo in clientSocketList:
            # print(client.getsockname())
            # print(client.getpeername())
            currClient, addr = clientInfo
            try:
                dataRecv = currClient.recv(1024)
            except Exception as e:
                continue

            if not dataRecv:
                clientSocketList.remove(clientInfo)
                print("currClient{} has closeed.\n".format(addr))
                continue
            try:
                strTime = time.strftime('%Y-%m-%d %H:%M:%S')
                strRecv = "{} from {} recv len={}, data={}".format(strTime, addr, len(dataRecv), dataRecv)
                print(strRecv)
            except Exception as e:
                print(e)
                pass

def exit_handler():
    os._exit(0)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit_handler)
    signal.signal(signal.SIGTERM, exit_handler)
    sockServer = init_server()
    threadCheckClient = threading.Thread(target=accept_client, args=(sockServer, ))  # 子线程
    # threadCheckClient.setDaemon(True)
    threadCheckClient.start()

    threadSend = threading.Thread(target=server_Recv)  # 子线程
    # threadSend.setDaemon(True)
    threadSend.start()

