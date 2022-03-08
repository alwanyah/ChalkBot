import numpy as np
import os

import re
from matplotlib import pyplot as plt

def get_file():
    arr=[]
    i=0
    for dir in os.listdir():
        if re.search("^out",dir):
            arr.append([None])
            for file in os.listdir(dir+"/"):
                if re.search("^test",file):
                    #print("\t"+file)
                    arr[i].append(dir+"/"+file)
                elif file == "Description.json":
                    arr[i][0]=file
            i+=1
    return arr

def get_Settings(file):
    
    return file.split("/")[0]+"/Description.json"


def plot(file,Description):
    #TODO check for null
    with open (file, "r") as myfile:
        data=myfile.readlines()
    fig, ax = plt.subplots() 
    ax.set_xlabel('x-axis') 
    ax.set_ylabel('y-axis') 
    #print(data[0])
    data.pop(0)
    data = np.loadtxt(data)
    print(data)
    ax.plot(data)
    #TODO check for img directory
    print(file.split("/")[0]+"/img/"+file.split("/")[1].split(".txt")[0]+".png")
    fig.savefig(file.split("/")[0]+"/img/"+file.split("/")[1].split(".txt")[0]+".png",dpi=300)
    #print(os.listdir())
    

    #figg.clear(True)



if __name__ == "__main__":
    list_files=get_file()
    print(list_files)
    for x in list_files:
        print(x)
        #plot(x,get_Settings(x))
# delay 10 ms
# x = busVoltage or current
# Description in File