# -*- coding: utf-8 -*-
import numpy as np
import os
import sys
import json

from merge import *

import re
from matplotlib import pyplot as plt

dpi=300
DEFAULT_SETTINGS = json.loads(
    '{"y-label":"y-label","x-label":"Time in 10 ms", "sensor":"Sensor", "pumpe":"Pumpe", "voltage":"Voltage", "amper":"Amper"}')


def get_file():
    arr = []
    i = 0
    for dir in os.listdir():
        if re.search("^out", dir):
            arr.append([None])
            for file in os.listdir(dir + "/"):
                if re.search("^test", file):
                    arr[i].append(dir + "/" + file)
                elif file == "Description.json":
                    arr[i][0] = dir + "/" + file
            i += 1
    return arr


def get_Settings(file):
    if file is None:
        return DEFAULT_SETTINGS
    with open(file) as f:
        data = json.loads(f.read())

    return {"x-label": DEFAULT_SETTINGS['x-label'], "y-label": data['Measuring'], "sensor": data['Sensor'],
            "pumpe": data['Pump'], "voltage": data['Voltage'], "amper": data['Amper']}

def img_path(file):
    return file.split("/")[0] + "/img/" + file.split("/")[1].split(".txt")[0] + ".png"


def plot(file, Description):
    with open(file, "r") as myfile:
        data = myfile.readlines()
    title = data.pop(0).split("\n")[0].encode('unicode-escape')
    title = str(title).replace("\\\\xc3\\\\xbc","ü")
    title = str(title).replace("\\\\xfc","ü")
    title = re.sub('\'$','',title)
    title = re.sub('^b\'','',title)
    data.pop(len(data) - 1)
    try:
        data = np.loadtxt(data)
    except ValueError as error:
        print("failed to load " + file, file=sys.stdout)
        return file
    fig, ax = plt.subplots()
    plt.rc('axes', titlesize=16)
    fig.suptitle(u"{title}".format(title=title))
    plt.rc('axes', titlesize=8)
    ax.set_title(Description['pumpe'] + " Pumpe", loc='left')
    ax.set_title(Description['voltage'] + ", " + Description['amper'], loc='right')
    ax.set_xlabel(Description['x-label'])
    ax.set_ylabel(Description['y-label'])
    ax.plot(data)
    path = file.split("/")[0] + "/img"
    os.makedirs(path, exist_ok=True)
    fig.savefig(img_path(file), dpi=dpi)
    print("Successfully created " + img_path(file))
    plt.close(fig)
    return None

    # figg.clear(True)


if __name__ == "__main__":
    args = sys.argv[1:]
    for arg in args:
        if re.search("^-dpi=", arg):
            dpi=int(arg.split("-dpi=")[1])
    # TODO set base folder
    list_files = get_file()
    failed_files = []
    for x in list_files:
        img_files=[]
        for file in x:
            if file != x[0]:
                img_files.append(img_path(file))
                res = plot(file, get_Settings(x[0]))
                if res != None:
                    failed_files.append(res)
        pngname=str(x[0].split('/')[0])
        mergepng(img_files,re.sub('^out','',pngname)+"_full.png")

    print("\n")
    print("Plotting falied for " + str(len(failed_files)) + " files: " + str(failed_files))
