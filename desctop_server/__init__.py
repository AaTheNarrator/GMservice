import time
import serial
from threading import Thread

from fastapi import FastAPI
from fastapi import APIRouter
from fastapi.staticfiles import StaticFiles

from . import GMservice


gm = GMservice.GMservice('/dev/ttyUSB0')
gm.work()

app = FastAPI()

api_v1 = FastAPI()
app.mount("/api/v1", api_v1)
app.mount("/", StaticFiles(directory="../ui", html=True))

time_label = time.time()
@api_v1.get("/cadr")
def get_last_cadr():
    return gm.last_data

@api_v1.get("/acc")
def get_acc_data():
    return gm.acc_data

@api_v1.get("/N")
def getset_N():
    pass

@api_v1.get("/L")
def getset_L():
    pass

@api_v1.get("/M")
def getset_M():
    pass


def update():
    while 1:
        c = gm.read_cadr()
        gm.process_cadr(c)

t1 = Thread(target=update)
t1.start()
