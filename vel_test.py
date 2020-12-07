import serial
import numpy as np
from time import sleep
import csv
import matplotlib
import matplotlib.pyplot as plt

# Global run code statement
keepRunning = True

# Enter function to connect to serial line
def serialConnect(portName, baudRate):
    try:
        ser = serial.Serial(portName, baudRate)
        print("opened port " + ser.name + '\n')
        # give Arduino time to reset
        sleep(2)
        # flush input buffer, discarding all contents
        ser.reset_input_buffer()
        return ser
    except serial.SerialException:
        raise IOError("problem connecting to " + portName)

# Enter function to receive data
def receiveData(ser):
    endReceive = False
    endMessage = "ENDOFSIG"
    global keepRunning
    count, left, right = [],[], []
    while endReceive == False:  # While loop to keep function call on while reading
        bytesRead = ser.read_until()
        stringData = bytesRead.decode().strip()
        print(stringData)
        # GREAT use of using arduino to echo values for processing
        # count, left, right all read here in this loop
        # Note did not use count in calculation but good example of importing what you need from arduino
        if stringData == 'COUNT':
            countString = ser.read_until()
            countList = [int(val) for val in countString.decode().strip().split('\t')]
            count.append(countList)
            #print(count)- print for debuggin but expect a flurry of data
        elif stringData == 'LEFT':
            leftString = ser.read_until()
            leftList = [int(val) for val in leftString.decode().strip().split('\t')]
            left.append(leftList)
            #print(left)- print for debuggin but expect a flurry of data
        elif stringData in 'RIGHT':
            rightString = ser.read_until()
            rightList = [int(val) for val in rightString.decode().strip().split('\t')]
            right.append(rightList)
            #print(right) - print for debuggin but expect a flurry of data
        elif stringData == endMessage:
            endReceive = True
    return count, left, right

# Enter function to put received data into lists of data
def intoDataSets(left_flat,right_flat):
    foward_left_25, foward_right_25, back_left_25, back_right_25 = [],[], [],[]
    foward_left_50, foward_right_50, back_left_50, back_right_50 = [],[], [],[]
    foward_left_75, foward_right_75, back_left_75, back_right_75 = [],[], [],[]
    foward_left_25 = left_flat[1:10]
    foward_left_50 = left_flat[11:20]
    foward_left_75 = left_flat[21:30]
    foward_right_25 = right_flat[1:10]
    foward_right_50 = right_flat[11:20]                                 # Whole function just maps the inputted needed
    foward_right_75 = right_flat[21:30]                                 # into lists of data for calculation
    back_left_25 = left_flat[32:41]
    back_left_50 = left_flat[42:51]
    back_left_75 = left_flat[52:61]
    back_right_25 = right_flat[32:41]
    back_right_50 = right_flat[42:51]
    back_right_75 = right_flat[52:61]
    return foward_left_25, foward_right_25, back_left_25, back_right_25, foward_left_50, foward_right_50, \
           back_left_50, back_right_50, foward_left_75, foward_right_75, back_left_75, back_right_75

# Enter function to calculate means
def means(to_mean):
    mean = sum(to_mean)/10
    return mean

# Enter function to display motor data
def displayData(left_means,right_means):
    label_at_perc = ['Forward 25%', 'Forward 50%', 'Forward 75%', 'Backward 25%', 'Backward 50%', 'Backward 75%']
    x = np.arange(len(label_at_perc))
    bar_width = 0.40
    fig, ax = plt.subplots()
    bar_left = ax.bar(x - bar_width / 2, left_means, bar_width, label='left motor')
    bar_right = ax.bar(x + bar_width / 2, right_means, bar_width, label='right motor')
    ax.set_ylabel('Encoder Rotations Per')
    ax.set_title('Overall Encoder Counts')
    ax.set_xticks(x)
    ax.set_xticklabels(label_at_perc)
    ax.legend()
    def height_label(bars):                             # quick hight label for graph
        for bar_s in bars:
            height = bar_s.get_height()
            ax.annotate('{}'.format(height),
                        xy=(bar_s.get_x() + bar_s.get_width() / 2, height),
                        xytext=(0, 3),
                        textcoords="offset points",
                        ha='center', va='bottom')
    height_label(bar_left)
    height_label(bar_right)
    fig.tight_layout()
    plt.show()

# Enter MAIN
if __name__ == '__main__':

    portName = "COM7"                                                               # Connect port
    ser = serialConnect(portName, 115200)
    sleep(2)

    count, left, right = [],[], []                                                  # Arrays for inputting data
    count_flat, left_flat, right_flat = [], [], []

    foward_left_25, foward_right_25, back_left_25, back_right_25 = [],[], [],[]
    foward_left_50, foward_right_50, back_left_50, back_right_50 = [],[], [],[]
    foward_left_75, foward_right_75, back_left_75, back_right_75 = [],[], [],[]



    while keepRunning == True:                                                     # Enter while loop for calculations
        try:
            print("We are now entering the gathering data portion data")
        except:
            print("We are IN calculating the data ")
        count, left, right = receiveData(ser)
        for elem in count:
            count_flat.extend(elem)
        for elem in left:
            left_flat.extend(elem)                                                 # for loops to flatten nested lists
        for elem in right:
            right_flat.extend(elem)
        # Gather Data
        foward_left_25, foward_right_25, back_left_25, back_right_25, foward_left_50, foward_right_50, back_left_50, \
        back_right_50, foward_left_75, foward_right_75, back_left_75, back_right_75= intoDataSets(left_flat,right_flat)
        # Put data into means for graphing
        left_means = [means(foward_left_25), means(foward_left_50), means(foward_left_75), means(back_left_25), means(back_left_50), means(back_left_75)]
        right_means = [means(foward_right_25), means(foward_right_50), means(foward_right_75), means(back_right_25), means(back_right_50), means(back_right_75)]

        displayData(left_means,right_means)

'''
DEAD CODE FOR DEBUGGING 
        print(count_flat)
        print(left_flat)
        print(right_flat)

        print(foward_left_25)
        print(foward_right_25)
        print(back_left_25)
        print(back_right_25)

        print(foward_left_50)
        print(foward_right_50)
        print(back_left_50)
        print(back_right_50)

        print(foward_left_75)
        print(foward_right_75)
        print(back_left_75)
        print(back_right_75)

        print(left_means)
        print(right_means)

'''
