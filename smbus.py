import serial
import serial.tools.list_ports
import time

class STM32USBInterface:
    def __init__(self, baudrate=115200, debug = False):
        self.serial = None
        self.baudrate = baudrate
        self.connect()
        self.debug = debug
        self.end_flag = b'\r\n'

    def connect(self):
        ports = list(serial.tools.list_ports.comports())
        for port in ports:
            if "USB Serial Device" in port.description:
                self.serial = serial.Serial(port.device, self.baudrate, timeout=1)
                print(f"Connected to {port.device}")
                return

        
        if self.serial is None:
            raise Exception("No USB Serial Device found")

    def _send_command(self, command):
        command_ef = command+self.end_flag
        self._debug(f'sending command {command_ef}...')
        if self.serial is None:
            raise Exception("No USB Serial Device found")
        
        self.serial.write(command_ef)
        
        response = bytearray()
        
        response = self._get_response(10)
        
        # return code byte, data byte(s)
        return response[0], response[1:]
    
    def _get_response(self, timeout):
        """Get response with timeout in milliseconds"""
        response = bytearray()

        self._debug(f'awaiting response with timeout {timeout}ms...')
        start_time = time.time()
        while 1:
            if (time.time() - start_time) > timeout/1000:  # 500ms timeout
                raise TimeoutError(f"Device did not respond within {timeout}ms")

            if self.serial.in_waiting:
                chunk = self.serial.read(self.serial.in_waiting)
                self._debug(f'got chunk: {chunk}')
                response.extend(chunk)

            if self.end_flag in response:
                break

        return response[:-2]
        
    def _debug(self, str):
        if self.debug is True:
            print(f' [DBG] {str}')

    def write_byte(self, byte):
        command = f"-wb {byte:02x}"
        return self._send_command(command)

    def write_word(self, word):
        command = f"-ww {word:04x}"
        return self._send_command(command)

    def read_byte(self, register):
        command = b'-rb '+register.to_bytes(1, 'little')
        return self._send_command(command)

    def read_word(self, register):
        command = b'-rw '+register.to_bytes(1, byteorder='little')
        return self._send_command(command)

    def block_read(self, register):
        command = b'-br '+register.to_bytes(1, byteorder='little')
        return self._send_command(command)

    def set_address(self, address):
        command = b'-sa '+address.to_bytes(1, byteorder='little')
        return self._send_command(command)

    def set_speed(self, speed):
        command = b'-ss '+speed.to_bytes(4, byteorder='little')
        return self._send_command(command)

    def close(self):
        if self.serial:
            self.serial.close()
            self.serial = None

if __name__ == "__main__":
    try:
        # Example usage
        stm32 = STM32USBInterface(debug = True)

        # Set address
        success, response = stm32.set_address(0x0B)
        print(f"Success? {success==0}. Set address: {int.from_bytes(response, byteorder='little', signed=False):02X}")

        # Set speed
        success, response = stm32.set_speed(100000)
        print(f"Success? {success==0}. Set speed: {int.from_bytes(response, byteorder='little', signed=False)}")

        # # Write byte
        # success, response = stm32.write_byte(0x5B)
        # print(f"Success? {success==0}. Write byte: {response}")

        # # Write word
        # success, response = stm32.write_word(0x472C)
        # print(f"Success? {success==0}. Write word: {response}")

        # Read byte
        success, response = stm32.read_byte(0x15)
        print(f"Success? {success==0}. Read byte: {int.from_bytes(response, byteorder='little', signed=False)}")

        # Read word
        success, response = stm32.read_word(0x15)
        print(f"Success? {success==0}. Read word: {int.from_bytes(response, byteorder='little', signed=False)}")

        # Block read
        success, response = stm32.block_read(0x20)
        print(f"Success? {success==0}. Block length: {response[0]}. Block data: {response[1:]}")
        
    except KeyboardInterrupt:
        stm32.close()
        quit()

    stm32.close()
    quit()