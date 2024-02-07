import asyncio
import bleak
import array 
import time
import PySimpleGUI as sg

def make_window():
    layout = [[sg.VPush()],
              [sg.Text("Status:", font = ("Helvetica", 18)), sg.Text("Connecting...", key = 'S',font = ("Helvetica", 18)),sg.Push(),sg.Button("Refresh", font = ("Helvetica", 20)),sg.Button("Close", font = ("Helvetica", 20))],
              [sg.Text("Last Locker Opened:",font = ("Helvetica", 18)),sg.Text("-",text_color= 'red',key = 'LO',font = ("Helvetica", 18)),sg.Push(), sg.Text(" ",font = ("Helvetica", 40)),sg.Button("Drop Off", font = ("Helvetica", 20))],
              [sg.Push(), sg.Text("Select locker to open",font = ("Helvetica", 35)), sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L1-',font = ("Helvetica", 20), size = (4,2)), sg.Button('----', font = ("Helvetica", 20), size = (4,2)), sg.Button("0%", key = '-L2-',font = ("Helvetica", 20), size = (4,2)),sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L3-',font = ("Helvetica", 20), size = (4,2)), sg.Button("0000", key = '-L4-',font = ("Helvetica", 20), size = (4,2)),sg.Button("0000", key = '-L5-',font = ("Helvetica", 20), size = (4,2)), sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L6-',font = ("Helvetica", 20), size = (4,2)), sg.Button("0000", key = '-L7-',font = ("Helvetica", 20), size = (4,2)),sg.Button("0000", key = '-L8-',font = ("Helvetica", 20), size = (4,2)), sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L9-',font = ("Helvetica", 20), size = (4,2)), sg.Button("0000", key = '-L10-',font = ("Helvetica", 20), size = (4,2)),sg.Button("0000", key = '-L11-',font = ("Helvetica", 20), size = (4,2)), sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L12-',font = ("Helvetica", 20), size = (4,2)), sg.Button("0000", key = '-L13-',font = ("Helvetica", 20), size = (4,2)),sg.Button("0000", key = '-L14-',font = ("Helvetica", 20), size = (4,2)), sg.Push()],
              [sg.Push(), sg.Button("0000", key = '-L15-',font = ("Helvetica", 20), size = (4,2)), sg.Button("0000", key = '-L16-',font = ("Helvetica", 20), size = (4,2)), sg.Push()],
              [sg.VPush()]]
    
    return sg.Window('MakerSpace Locker', layout, resizable=True, size=(500,800), location=(0,0))
def dropoff_window():
    layout = [
                [sg.Push(),sg.Text('Student ID ',font = ("Helvetica", 15), size=(10,1)),sg.Input(key='-ID-',expand_x=True,font = ("Helvetica", 15)),sg.Push()],
                [sg.Push(), sg.Text('Size: ',font = ("Helvetica", 15)), sg.Button("Small",font = ("Helvetica", 15)), sg.Button("Medium",font = ("Helvetica", 15)),sg.Button("Large",font = ("Helvetica", 15)), sg.Push()],
                [sg.Push(), sg.Text(" ",font = ("Helvetica", 40)),sg.Push()],[sg.Cancel(font = ("Helvetica", 10))]
            ]
    return sg.Window('Dropoff Form', layout, size=(715,207))
def lockScreen():
    code = ""
    window = make_window()
    running = True
    while running:
        event, value= window.read()
        #print(event)
        if event==("CLOSE"):
            running = False
        else:
        #window['-TOP-'].update("INCORRECT CODE")
            if len(event) == 4:
                foo = (event[2])
            else:
                foo = (event[2]+event[3])
            #window.refresh()
            print(foo)
    window.close()
  

def print_id(array):
    print(" Current Users: \n________________")
    print(f"|{to_four(str(array[0]))}|----| {str(array[1])}%|")
    print(f"|{to_four(str(array[3]))}|{to_four(str(array[3]))}|{to_four(str(array[4]))}|")
    print(f"|{to_four(str(array[6]))}|{to_four(str(array[6]))}|{to_four(str(array[7]))}|")
    print(f"|{to_four(str(array[9]))}|{to_four(str(array[9]))}|{to_four(str(array[10]))}|")
    print(f"|{to_four(str(array[12]))}|{to_four(str(array[12]))}|{to_four(str(array[13]))}|")
    print(f"| {to_four(str(array[14]))} | {to_four(str(array[15]))}  |")
    print("⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻")

def to_four(input_str):
    while len(input_str) < 4:
        input_str = "0" + input_str
    return input_str



async def connect_to_ble_device(device_address):
    user_data = [0] * 16
    #Twindow = make_window()
    
    
    window = make_window()
    window.read(timeout = 100)
    #window.read(timeout = 1000)
    print("Attempting to connect...")
    client = bleak.BleakClient(device_address)
    locker = 0
    try:
        await client.connect()
        running = True
        print("Connected!")
        Twindow.close()
        while running:
            services = await client.get_services()
            for service in services:
                if service.uuid == "19b10001-e8f2-537e-4f6c-d104768a1214":
                    for characteristic in service.characteristics:
                        #print("Characteristic UUID:", characteristic.uuid)
                        try:
                            value = await client.read_gatt_char(characteristic.uuid)
                            decoded_value = int.from_bytes(value, byteorder='little')
                            user_data[int(characteristic.uuid[-2] + characteristic.uuid[-1]) - 1] = decoded_value
                        except bleak.BleakError as e:
                            print("Error reading characteristic:", e)
            
            window.read(timeout = 10)
            window['S'].update("Connected")
            for x in range(len(user_data)):
                #print('-L' + str(x+1) + '-')
                #print(user_data[x])
                if x != 1:
                    window['-L' + str(x+1) + '-'].update(to_four(str(user_data[x])))
                else:
                    window['-L' + str(x+1) + '-'].update(" "+str(user_data[x])+"%")
                window.refresh()
                
            
        
            event, value= window.read()
            #print(event)
            if event==("Close"):
                running = False
                window.close()
            elif event==("Drop Off"):
                windowD = dropoff_window()
                event, values = windowD.read()
                windowD.close()
                lockerType = "0"
                reuse = False
                match event:
                    case "Small":
                        lockerType = "1"
                    case "Medium":
                        lockerType = "2"
                    case "Large":
                        lockerType = "3"
                if(lockerType != "0"):
                    try:
                        location = user_data.index(int(values['-ID-']))
                        reuse = True
                        print(int(location)+1)
                        locker = int(location)+1
                        if ((locker <= 16 and lockerType == "1") or (locker >= 5 and lockerType == "2") or (locker >= 15 and lockerType == "3")):
                            try:
                                print("Reused Locker: "+ str(locker))
                                window['LO'].update(str(locker))
                                window.refresh()
                                locker_bytes = locker.to_bytes(1, 'little') 
                                #print(locker_bytes)
                                #print(service.characteristics[0].uuid)
                                await client.write_gatt_char(service.characteristics[0].uuid, locker_bytes,True)
                            except bleak.BleakError as e:
                                print("Error writing to characteristic:", e)
                    except:
                        print("No Locker In Use")
                    
                    if (not reuse):
                        message = values['-ID-'] + lockerType
                        
                        intMessage = int(message)
                        print(intMessage)
                        try:
                            locker_bytes = intMessage.to_bytes(4, 'little') 
                            #print(locker_bytes)
                            #print(service.characteristics[1].uuid)
                            await client.write_gatt_char(service.characteristics[1].uuid, locker_bytes,True)
                            value = await client.read_gatt_char(service.characteristics[1].uuid)
                            decoded_value = int.from_bytes(value, byteorder='little')
                            print("Locker Assignment Successful: Use Locker "+ str(decoded_value))
                            window['LO'].update(str(decoded_value))
                            window.refresh()
                            await client.write_gatt_char(service.characteristics[1].uuid, bytes([0]),True)
                        except bleak.BleakError as e:
                            print("Locker Assignment Failed\nError writing to characteristic:", e)



            elif event == ("Refresh"):
                print("refreshing...")
            elif event != ("----"):
            #window['-TOP-'].update("INCORRECT CODE")
                if len(event) == 4:
                    foo = (event[2])
                else:
                    foo = (event[2]+event[3])
                #window.refresh()
                print(foo)
                locker = int(foo)
                print(locker)
                window['LO'].update(str(locker))
                window.refresh()
                if locker != 0:
                    try:
                        locker_bytes = locker.to_bytes(1, 'little') 
                        #print(locker_bytes)
                        #print(service.characteristics[0].uuid)
                        await client.write_gatt_char(service.characteristics[0].uuid, locker_bytes,True)
                    except bleak.BleakError as e:
                        print("Error writing to characteristic:", e)
        

    except bleak.BleakError as e:
        print("Error:", e)
        window['S'].update("Failed")
        event, values = window.read()
        window.close()
    finally:
        print_id(user_data)
        window.close()
        await client.disconnect()

if __name__ == "__main__":
    #lockScreen()
    device_address = "DC:54:75:CF:45:D1"  # Replace with the address of your device
    asyncio.run(connect_to_ble_device(device_address))
    
    if(0):
        window = dropoff_window()
        event, values = window.read()
        print (event)
        if event != "Cancel":
            print(int(values['-ID-']))
        window.close()
