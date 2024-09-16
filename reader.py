import serial
import serial.tools.list_ports
import time

def find_usb_serial_device():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "USB Serial Device" in port.description:
            return port.device
    return None

def connect_and_monitor():
    while True:
        device = find_usb_serial_device()
        if device:
            try:
                with serial.Serial(device, 115200, timeout=1) as ser:
                    print(f"Connected to {device}")
                    while True:
                        try:
                            if ser.in_waiting:
                                line = ser.readline().decode('utf-8').strip()
                                print(f"Received: {line}")
                        except serial.SerialException:
                            print("Connection lost. Attempting to reconnect...")
                            break
            except serial.SerialException as e:
                print(f"Failed to connect: {e}")
        else:
            pass
        
        time.sleep(0.01)  # Wait for 10ms before trying to reconnect

if __name__ == "__main__":
    print("Starting USB Serial Device Monitor...")
    connect_and_monitor()