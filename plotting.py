from matplotlib import pyplot as plt
import csv
from numpy import genfromtxt


def plotStockPath(stockVals, timeVals):
    assert len(stockVals) == len(timeVals)
    fig, ax = plt.figure(figsize=(10,8))
    ax.plot(timeVals,stockVals,'b-')
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.savefig("plots/stockPath.png")
    plt.plot()

def plotStockCSV(filename):
    stockData = genfromtxt(filename, delimiter=',')
    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(stockData[:,0],stockData[:,1],'b-')
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.savefig("Plots/stockPath.png")
    plt.plot()

if __name__ == "__main__":
    plotStockCSV("Data/stockPath.csv")



