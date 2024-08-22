from matplotlib import pyplot as plt
import csv
from numpy import genfromtxt

plt.rcParams.update({'font.size': 22})

def plotStockPath(stockVals, timeVals):
    assert len(stockVals) == len(timeVals)
    fig, ax = plt.figure(figsize=(10,8))
    ax.plot(timeVals,stockVals,'b-')
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.savefig("plots/stockPath.png")
    plt.plot()

def plotStockCSV(filenames):
    stockData = [genfromtxt(filename, delimiter=',') for filename in filenames]
    fig, ax = plt.subplots(figsize=(13, 8))
    for data in stockData:
        ax.plot(data[:,0],data[:,1])
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.title("Paths of a Heston Model")
    plt.savefig("Plots/stockPath.png")
    plt.plot()

if __name__ == "__main__":
    plotStockCSV(["Data/stockPath1.csv","Data/stockPath2.csv","Data/stockPath3.csv"])



