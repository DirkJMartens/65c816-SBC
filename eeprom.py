import serial       #You need the pyserial library
import time 
import struct       #For packing data into bytes

print("      ________     ________    _________    _________    _________    _____________ ")
print("    /  ______/|  /  ______/|  /  ___   /|  /  ___   /|  /   __   /|  /  _    _    /|")
print("   /  /_____ |/ /  /_____ |/ /  /__/  / / /  /__/  / / /  /  /  / / /  / /  / /  / /")
print("  /  ______/|  /  ______/|  /   _____/ / /     ___/ / /  /  /  / / /  / /  / /  / / ")
print(" /  /______|/ /  /______|/ /  / _____|/ /  /\  \__|/ /  /__/  / / /  / /  / /  / /  ")
print("/________/|  /________/|  /__/ /       /__/ /\__/|  /________/ / /__/ /__/ /__/ /   ")
print("|________|/  |________|/  |__|/        |__|/ |__|/  |________|/  |__|/|__|/|__|/    ")
print("\n")
#print("  Allan Gallop 2020")
#print("  www.M0VTE.co.uk")
#print("  https://github.com/AllanGallop/eprom\n")
#print("  Based on work by Robson Couto")
#print("  github.com/robsoncouto/eprom\n")


#Default value, 32KB chip:
romsize=32*1024     # 28C256 is 32K EEPROM 
sectorsize=128      # I am sending data in 128 byte chunks
numsectors=int(romsize/sectorsize)      # 32768/128 = 256
#print("Enter COM port address (eg COM1, /dev/ttyACM0)")
#comport=str(input(":"))
comport = "COM21"
ser = serial.Serial(comport, 250000, timeout=0)     # Serial Port stream 

while True:

    print("  What would you like to do?       ")
    print("                                   ")
    print("       1 - Dump eeprom to file    ")
    print("       2 - Burn eeprom from file  ")
    print("       3 - Fill eeprom with 0x00  ")
    print("       4 - Blank check            ")
#    print("       5 - Select chip size       ")
    print("       6 - Verify against file    ")
    print("                                   ")
    print("       7 - Quit                 \n")
    #get option from user:
    option=int(input("Please insert a number:"))

    ###############
    # Read EEPROM #
    ###############
    if(option==1): 
#        name=input("What the name of the file?")
        name="test.hex" 
        f = open(name, 'w')     # create empty file 
        f.close()               # close it 
        ser.flushInput()        # clear serial stream 
        ser.write(b"\x55")      # send 0x55 to Arduino as start byte 
        ser.write(bytes("r","ASCII"))   # send "r" to tell Arduino to read EEPROM 
        numBytes=0 
        f = open(name, 'ab')    # open file, using append and binary mode 
        #TODO - Checksum scheme as when burning
        while (numBytes<romsize):   # read as many bytes as EEPROM has 
            while ser.inWaiting()==0:   # 
                print("Reading from eeprom. Current percentage:{:.2%}".format(numBytes/romsize),end='\r')
#                time.sleep(0.1) # delay for 0.1 sec 
            data = ser.read(1)  # read 1 byte from serial port 
            f.write(data)       # and write the byte to the file 
            numBytes=numBytes+1 # increase nr of bytes read 
        f.close()               # all bytes done, save/close file 
        print("\nDone\n")       # info message 
        
    ############################################
    # Burn EEPROM, see schematic at my website #
    ############################################
    if(option==2):
        name=input("What's the name of the file?")
        print(name)
        f = open(name, 'rb')
        for i in range(numsectors): 
            ser.write(b"\x55") 
            ser.write(bytes("w","ASCII" )) 
            time.sleep(0.001) 
            #send address of the block first
            ser.write(struct.pack(">B",i>>8))
            CHK=i>>8
            time.sleep(0.001)
            ser.write(struct.pack(">B",i&0xFF))
            CHK^=i&0xFF
            time.sleep(0.001)
            data=f.read(128);
            #print(data)
            #Gets checksum from xor-ing the package
            for j in range(len(data)):
                CHK=CHK^data[j]
            time.sleep(0.001)
            print("Writing data. Current percentage:{:.2%}".format(i/numsectors),end='\r')
            print("CHK:", ~CHK)
            response=~CHK
            #keeps trying while the replied checksum is not correct
            while response!=CHK:
                ser.write(data)
                ser.write(struct.pack(">B",CHK&0xFF))
                timeout=60
                while ser.inWaiting()==0:
                    time.sleep(0.01)
                    timeout=timeout-1
                    if timeout==0:
                        print("Could not get a response, please start again\n")
                        break
                CHKBCK = ser.read(1)
                if(len(CHKBCK) > 0):
                    response=ord(CHKBCK)
                    if response!=CHK:
                        print("Wrong checksum, sending chunk again\n")
        f.close()

    ##################
    # Fill with zero #
    ##################
    if(option==3):
        print("Writing zero's to EEPROM")
        for i in range(numsectors):
            ser.write(b"\x55")
            ser.write(bytes("w","ASCII" ))
            time.sleep(0.001)
            #send address of the block first
            ser.write(struct.pack(">B",i>>8))
            CHK=i>>8
            time.sleep(0.001)
            ser.write(struct.pack(">B",i&0xFF))
            CHK^=i&0xFF
            time.sleep(0.001)
            data=bytearray(128)
            for j in range(len(data)):
                CHK=CHK^data[j]
            time.sleep(0.001)
            print("Writing data. Current percentage:{:.2%}".format(i/numsectors),end='\n')
            print("CHK:", CHK)
            response=~CHK
            #keeps trying while the replied checksum is not correct
            while response!=CHK:
                ser.write(data)
                ser.write(struct.pack(">B",CHK&0xFF))
                timeout=60
                while ser.inWaiting()==0:
                    time.sleep(0.01)
                    timeout=timeout-1
                    if timeout==0:
                        print("could not get a response, please start again\n")
                        break
                CHKBCK = ser.read(1)
               
                if(len(CHKBCK) > 0):
                    response=ord(CHKBCK)
                    if response!=CHK:
                        print("wrong checksum, sending chunk again\n")

    ###############
    # Blank check #
    ###############
    if(option==4):
        #same as reading
        ser.flushInput()
        ser.write(b"\x55")
        ser.write(bytes("r","ASCII"))
        numBytes=0
        blank=1
        while (numBytes<romsize):
            while ser.inWaiting()==0:
                print("Reading from eeprom. Current percentage:{:.2%}".format(numBytes/romsize),end='\r')
                time.sleep(0.1)
            data = ser.read(1)
            numBytes=numBytes+1
            if ord(data)!=255:
                blank=0
                break
        #Ends check on first byte not erased
        if blank==1:
            print("\nThe chip is blank\n")
        else:
            print("\nThe chip seems to contain data\n")
        print("Done\n")

    #############################################################
    # Change size of EEPROM, for reading EEPROMs other than 1MB #
    #############################################################
    if(option==5):
        print("Current eeprom size:",romsize/(1024*1024),"MB\n")
        megs=float(input("Please insert the size of the eeprom in Megabytes"))
        romsize=megs*1024*1024
        numsectors=int(romsize/128) # I am sending data in 128 byte chunks
        if megs>1:
            print("Eeprom size changed to ",romsize/(1024*1024),"MB\n")
        else:
            print("Eeprom size changed to ",romsize/(1024),"KB\n")

    ###########################################################
    # This is for checking if the eeprom was programmed right #
    ###########################################################
    if(option==6):
        #Reads each byte and compares with a byte in the file
        print("This compares a eeprom with a file in the script folder\n")
        name=input("\nWhat's the name of the file?\n")
        f = open(name, 'rb')
        ser.flushInput()
        ser.write(b"\x55")
        ser.write(bytes("r","ASCII"))
        numBytes=0
        while (numBytes<romsize):
            while ser.inWaiting()==0:
                print("Reading from eeprom. Current percentage:{:.2%}".format(numBytes/romsize),end='\r')
                time.sleep(0.01)
            eeprom_byte = ser.read(1)
            file_byte = f.read(1)
            if(eeprom_byte!=file_byte):
                #the \033[031m and \033[0m sequence are ansi sequences that turn text red and cancel it respectively
                #I dont know how windows handle them
                print("\n\033[31mFound mismatch at",hex(f.tell()))
                print("- eeprom byte:",hex(ord(eeprom_byte)),"- file byte:",hex(ord(file_byte)),"\033[0m\n")
            numBytes=numBytes+1
        print("\nDone\n")

    ########
    # Exit #
    ########
    if(option==7):
        print("See ya!")
        break