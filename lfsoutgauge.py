import socket
import struct
import sys
import serial

import time
minInterval = 0.04 #40ms
lastSend = 0
udpIp = "127.0.0.1"
udpPort = 4444

bufferSize = 256

baud_rate = 38400

serialPorts = ["/dev/ttyUSB0", "/dev/ttyUSB1"] #IMPORTANT, MAY NEED TO CHANGE THIS

outgaugeFormat = 'I3sxH2B7f2I3f15sx15sxi'
outgaugeSize = struct.calcsize(outgaugeFormat)

def main():
    lastSend = 0

    udpSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        udpSocket.bind((udpIp, udpPort))
    except OSError as e:
        print(f"cant bind to {udpIp}:{udpPort} - code {e}")
        sys.exit(1)

    ser = None
    for port in serialPorts:
        try:
            ser = serial.Serial(port, baud_rate, timeout=1)
            print(f"using {port}")
            break
        except serial.SerialException as e:
            print(f"  {port} failed - {e}")

    if ser is None:
        print("cant find serial")
        sys.exit(1)

    time.sleep(2)   # Uno resets when the port opens

    try:
        while True:
            data, _ = udpSocket.recvfrom(bufferSize)
            if len(data) != outgaugeSize:
                continue

            now = time.monotonic()
            if now - lastSend < minInterval:
                continue
            lastSend = now

            pack = struct.unpack(outgaugeFormat, data)
            
            
            speedMs = pack[5]
            rpm = pack[6]
            engTemp = pack[8]
            throttleFraction = pack[14]

            speedKph = speedMs * 3.6
            throttlePercent = throttleFraction*100
            

			

            ser.write(f"{speedKph:.1f},{int(rpm)},{engTemp:.1f},{throttlePercent:.1f}\n".encode()) #sends to arduino through serial as kph,rpm,temp,throttle
            #print(f"{speedKph:.1f},{int(rpm)},{engTemp:.1f},{throttlePercent:.1f}\n")
            
    except KeyboardInterrupt:
        print("user stopped")

    finally: 
        udpSocket.close()
        ser.close()
        print("hi")

if __name__ == "__main__":
    main()
