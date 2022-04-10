import ads1256       # import this lib                             

gain = 1			 # ADC's Gain parameter
sps = 25			 # ADC's SPS parameter

# Create the first list. It will receive ADC's absolute values
AllChannelValuesVolts = [0,0,0,0,0,0,0,0]       

# Create the second list. It will received absolute values converted to Volts
AllChannelValues = [0,0,0,0,0,0,0,0]		

# Initialize the ADC using the parameters
ads1256.start(str(gain),str(sps))  

# Fill the first list with all the ADC's absolute channel values
AllChannelValues = ads1256.read_all_channels()        
                
for i in range(0, 8):
	# Fill the second list  with the voltage values
	AllChannelValuesVolts[i] = (((AllChannelValues[i] * 100) /167.0)/int(gain))/1000000.0   


for i in range(0, 8):     
    # Print all the absolute values
    print( AllChannelValues[i] )             


# Print a new line
print ("\n");							   


for i in range(0, 8):     
    # Print all the Volts values converted from the absolute values
    print( AllChannelValuesVolts[i] )        

# Stop the use of the ADC
ads1256.stop() 							   



     
