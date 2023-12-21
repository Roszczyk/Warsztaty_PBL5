# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
# butPin = 17 # Broadcom pin GPIO17 (Pin 11 on J8 hea>
butPin = 26 # Broadcom pin GPIO3 (Pin 5 on J8 header)

# Pin Setup:
GPIO.setmode(GPIO.BCM) # Broadcom pin-numbering scheme
GPIO.setup(butPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

butState = GPIO.input(butPin)

timePrev=time.time()
timeNow=0
timeSpent=0

print("Here we go! Press CTRL+C to exit")
try:
    while 1:
        newButState = GPIO.input(butPin)
        if(butState != newButState):
            if newButState: # button is released
               print("Button released!")
            else: # button is pressed:
               print("Button pressed!")
            butState = newButState
            timeNow=time.time()
            timeSpent=timeNow-timePrev
            timePrev=timeNow
            print(f"Time spent: {timeSpent}")
except KeyboardInterrupt: # If CTRL+C is pressed, exi>
    GPIO.cleanup() # cleanup all GPIO
