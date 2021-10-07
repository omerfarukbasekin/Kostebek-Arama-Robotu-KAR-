import RPi.GPIO as GPIO   
import curses
import sys
from time import sleep

in1 = 23
in2 = 24
in3 = 27
in4 = 17
en = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(in1,GPIO.OUT)
GPIO.setup(in2,GPIO.OUT)
GPIO.setup(in3,GPIO.OUT)
GPIO.setup(in4,GPIO.OUT)
GPIO.setup(en,GPIO.OUT)
GPIO.output(in1,GPIO.LOW)
GPIO.output(in2,GPIO.LOW)
GPIO.output(in3,GPIO.LOW)
GPIO.output(in4,GPIO.LOW)
p=GPIO.PWM(en,1000)

p.start(25)
print("\n")
print("The default speed & direction of motor is LOW & Forward.....")
print("r-run c-stop w-forward s-backward a-left d-right 1-low 2-medium 3-high e-exit")
print("\n")    

class MotorKaputt:
    client_socket = None
    temp1 = 1
    def __init__(self,cs):
        self._running = True
        self.client_socket = cs
        
    def terminate(self):
        self._running =  False
        
    def run(self):
        while(True):
            response = self.client_socket.recv(2048)
            if not response: break
            data = response.decode("utf-8")
            if data=="r":
                print("run")
                if(self.temp1==1):
                 GPIO.output(in1,GPIO.HIGH)
                 GPIO.output(in2,GPIO.LOW)
                 GPIO.output(in3,GPIO.HIGH)
                 GPIO.output(in4,GPIO.LOW)
                 print("forward")
                else:
                 GPIO.output(in1,GPIO.LOW)
                 GPIO.output(in2,GPIO.HIGH)
                 GPIO.output(in3,GPIO.LOW)
                 GPIO.output(in4,GPIO.HIGH)
                 print("backward")
            elif data=='c':
                print("stop")
                GPIO.output(in1,GPIO.LOW)
                GPIO.output(in2,GPIO.LOW)
                GPIO.output(in3,GPIO.LOW)
                GPIO.output(in4,GPIO.LOW)
            elif data=='W':
                print("forward")
                GPIO.output(in1,GPIO.HIGH)
                GPIO.output(in2,GPIO.LOW)
                GPIO.output(in3,GPIO.HIGH)
                GPIO.output(in4,GPIO.LOW)
                self.temp1=1
                

            elif data=='S':
                print("backward")
                GPIO.output(in1,GPIO.LOW)
                GPIO.output(in2,GPIO.HIGH)
                GPIO.output(in3,GPIO.LOW)
                GPIO.output(in4,GPIO.HIGH)
                self.temp1=0
                

            elif data=='1':
                print("low")
                p.ChangeDutyCycle(25)
                

            elif data=='2':
                print("medium")
                p.ChangeDutyCycle(50)
               

            elif data=='3':
                print("high")
                p.ChangeDutyCycle(100)
               

            elif data=='a':
                print("left")
                GPIO.output(in1,GPIO.LOW)
                GPIO.output(in2,GPIO.HIGH)
                GPIO.output(in3,GPIO.HIGH)
                GPIO.output(in4,GPIO.LOW)
                self.temp1=0
                

            elif data=='d':
                print("right")
                GPIO.output(in1,GPIO.HIGH)
                GPIO.output(in2,GPIO.LOW)
                GPIO.output(in3,GPIO.LOW)
                GPIO.output(in4,GPIO.HIGH)
                self.temp1=0
               
       
            elif data=='e':
                GPIO.cleanup()
                print("GPIO Clean up")
                sys.exit()
            else:
                    print("<<<  wrong data  >>>")
                    print("please enter the defined data to continue.....")
