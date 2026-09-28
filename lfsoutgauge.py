import socket
import struct
import sys
import serial

udpIp = "127.0.0.1"
udpPort = 4444

bufferSize = 256

baud_rate = 38400

serialPort = "/dev/ttyUSB0" #IMPORTANT, MAY NEED TO CHANGE THIS

outgaugeFormat = 'I3sxH2B7f2I3f15sx15sxi'
outgaugeSize = struct.calcsize(outgaugeFormat)

def main():

    udpSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #setup socket

    try: 
        udpSocket.bind((udpIp, udpPort))
    except OSError as e:
        print(f"cant bind to {udpIp}:{udpPort} - code {e}") #if cant open udp port, print this

        sys.exit()
    try:
        ser = serial.Serial(serialPort, baud_rate, timeout = 1)

    except serial.SerialException as e:
        print(f"cant connect at port {serialPort} - code {e}") #if cant connect to arduino, print this

        sys.exit()

    print(f"input at {udpIp}:{udpPort}")
    print(f"output at {serialPort}")

    try: 
        while True:


            data, _ = udpSocket.recvfrom(bufferSize)
            if len(data) != outgaugeSize:
                continue
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
