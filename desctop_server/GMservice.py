import enum
import serial


class _states(enum.Enum):
    pause = 0
    work = 1

class GMservice:
    def __init__(self, port):
        self.port = serial.Serial(port, baudrate=115200)
        self.temp = 0
        self.N = 0
        self.L = 0
        self.M = 0
        self.last_data = [0] * 4096
        self.acc_data = [0] * 4096
        self.state = _states.pause

    def read_cadr(self) -> str:
        """
        функция читает данные из порта пока не увидит '!!!'
        """
        is_cadr_end = 0
        _len = 0
        res = b''
        while is_cadr_end < 3:
            d = self.port.read()
            _len += 1
            if d == b'!':
                is_cadr_end += 1
            res += d
        return res

    def process_cadr(self, cadr: str):
        data = cadr.split()
        if len(data) < 7:
            return
        if len(data) > 4100:
            return
        if data[0] != b'SPCSPC' and data[-1] != '!!!':
            return
        self.temp = float(data[1])
        self.N = int(data[2])
        self.M = int(data[3])
        self.L = int(data[4])
        print(f"{self.N = } {self.M = } {self.L = }")
        for i, n in enumerate(data[6:-1]):
            if not n.isdigit():
                continue
            self.last_data[i] = int(n)
            self.acc_data[i] += int(n)

    def pause(self):
        self.port.write(b'MODMODWAIT\0')
        self.state = _states.pause

    def work(self):
        self.port.write(b'MODMODTEST\0')
        self.state = _states.work

    def set_N(self, val=-1):
        if val > -1 and self.state == _states.work:
            self.port.write(f'PEMPEM {val} {val} \0'.encode())

    def set_M(self, val=-1):
        if val > -1 and self.state == _states.work:
            self.port.write(f'SDVSDV {val} {val} \0'.encode())

    def set_L(self, val=-1):
        if val > -1 and self.state == _states.work:
            self.port.write(f'TVHTVH {val} {val} \0'.encode())
