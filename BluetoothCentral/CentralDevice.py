import asyncio
import bleak

async def connect_to_ble_device(device_address):
    client = bleak.BleakClient(device_address)

    try:
        await client.connect()

        # Discover services and characteristics
        services = await client.get_services()
        for service in services:
            print("Service UUID:", service.uuid)
            
            for characteristic in service.characteristics:
                print("Characteristic UUID:", characteristic.uuid)

        # Prompt user for ON/OFF command
        while True:
            command = input("Enter 'ON' or 'OFF' to control the LED (or 'q' to quit): ").strip().upper()
            if command == "ON" or command == "OFF":
                # Write the command to the characteristic
                await client.write_gatt_char("19B10001-E8F2-537E-4F6C-D104768A1214", command.encode())
                print("Sent command:", command)
            elif command == "Q":
                break
            else:
                print("Invalid command. Enter 'ON' or 'OFF' (or 'q' to quit).")

    except bleak.BleakError as e:
        print("Error:", e)
    finally:
        await client.disconnect()

if __name__ == "__main__":
    device_address = "DC:54:75:CF:45:D1"  # Replace with the address of your device
    asyncio.run(connect_to_ble_device(device_address))
