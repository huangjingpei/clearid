# A (1001) 呼叫B (1002)，第一步需要把call消息发给服务器C(123.110.110.110)。
A->C
```C
{
    "action":"invite",
    "from": "1001",
    "to":"1002
	"cseq":0
    "offer": {
    }
}
```

# A收到服务器返回的消息，建立socket连接，服务器在20000端口监听,等待服务器连接。A和C建立连接
# trying的消息为一个中间状态消息，代表服务器已经收到了消息，正在处理。
C->A
```C
{
    "action":"trying",
    "from": "1001",
    "to":"1002",
	"cseq":0,
    "network": {
        "addr" :"123.110.110.110",
        "port":20000
    },
    "answer": {
    }
}
```


# 服务器需要把信息转发给B, B收到从服务器发送的network信息后，根据信息在54000端口建立socket监听。然后把消息发送给B
C->B
```C
{
    "action":"invite",
    "from": "1001",
    "to":"1002",
	"cseq":0,
    "network": {
        "addr" :"123.110.110.110",
        "port":54000
    },
    "offer": {
    }
}
```




# B收到从服务器发送的call信息后，根据信息连接建立socket连接,连接服务器的54000端口，然后把established消息发送给A
```C
{
    "action":"established",
    "from": "1002",
    "to":"1001",
	"cseq":0,
    "answer": {
    }
}
```

# 服务器发送给A的挂断消息
{
    "action":"hangup"
    "from": "1002",
    "to":"1001",
	"cseq":0,
    
}

# A发送给服务器的挂断消息
{
    "action":"hangup"
    "from": "1001",
    "to":"1002",
	"cseq":0,
    
}

# A收到服务器发送的拒接消息
{
    "action":"reject"
    "from": "1002",
    "to":"1001",
	"cseq":0,
}


# A收到服务器发送的振铃消息
{
    "action":"ringing"
    "from": "1002",
    "to":"1001",
	"cseq":0,
}

