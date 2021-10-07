import serial
from threading import Thread
from camera_thread import CameraThread
from motorkaputt import MotorKaputt
import socket
import sys

def run_camera():
    Ct = CameraThread()
    CtThread = Thread(target = Ct.run)
    CtThread.start()
    
def run_motor_kaputt(cs):
    Mk = MotorKaputt(cs)
    MkThread = Thread(target = Mk.run)
    MkThread.start()
    
if __name__ == '__main__':
    run_camera()
    
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = "karserver.westeurope.cloudapp.azure.com"
    client_socket.connect((host, 5000))
    
    run_motor_kaputt(client_socket)
    ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
    ser.flush
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').rstrip()
            #print(data)
            if (data == 'q'): 
                client_socket.send(str.encode(data + '\n') )
                client_socket.close()
            #else:
                #client_socket.send(str.encode(data + '\n'))
