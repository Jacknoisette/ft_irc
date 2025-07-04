import socket
import random
import string
import time

SERVER = "127.0.0.1"
PORT = 6667
CHANNEL = "#hello"
PASS = "password"
NICK = "floodbot"
USER = "floodbot floodbot floodbot :Flood Bot"

def random_long_message(length=1500):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def random_long_ascii(length=1500):
    chars = ''.join(chr(i) for i in range(0, 32))
    return ''.join(random.choices(chars, k=length))


def main():
    s = socket.socket()
    s.connect((SERVER, PORT))
    s.sendall(f"PASS {PASS}\r\n".encode())
    s.sendall(f"NICK {NICK}\r\nUSER {USER}\r\n".encode())
    # s.sendall(f"USER {USER}\r\n".encode()e)
    time.sleep(1)
    s.sendall(f"JOIN {CHANNEL}\r\n".encode())
    time.sleep(1)

    while True:
        rand = random.randint(0, 5)
        print("\033[5m\033[33m", rand ,"\033[0m")
        ircmsg = ""
        if rand == 1 :
            msg = random_long_message(random.randint(0, 1500))
            ircmsg = f"PRIVMSG {CHANNEL} :{msg}\r\n"
        elif rand == 2:
            msg = random_long_message(random.randint(0, 50))
            ircmsg = f"JOIN #{msg}\r\n"
        elif rand == 3:
            ircmsg = f"PRIVMSG {CHANNEL} :"
        elif rand == 4:
            ircmsg = f""
        elif rand == 5:
            msg = random_long_ascii(random.randint(0, 1500))
            ircmsg = f"{msg}"
        s.sendall(ircmsg.encode())
        print(f"Sent: {ircmsg[:80]}... ({len(ircmsg)} chars)")
        time.sleep(0.1)

if __name__ == "__main__":
    main()