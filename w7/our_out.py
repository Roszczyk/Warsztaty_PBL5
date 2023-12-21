# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
# ledPin = 23 # Broadcom pin GPIO23 (Pin 16 on J8 header)
ledPin = 26 # Broadcom pin GPIO4 (Pin 7 on J8 header)


# Pin Setup:
GPIO.setmode(GPIO.BCM) # Broadcom pin-numbering scheme
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output

# Initial state for LEDs:
gpioPinState = 0
GPIO.output(ledPin, GPIO.LOW)

print("Here we go! Press CTRL+C to exit")
try:
    while 1:
        time.sleep(0.512)
        if gpioPinState == 1:
            GPIO.output(ledPin, GPIO.LOW)
            gpioPinState = 0
        else: # button is pressed:
            GPIO.output(ledPin, GPIO.HIGH)
            gpioPinState = 1
except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly
    GPIO.cleanup() # cleanup all GPIO