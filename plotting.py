from matplotlib import pyplot as plt

def plotStockPath(stockVals, timeVals):
    assert len(stockVals) == len(timeVals)
    fig, ax = plt.figure(figsize=(10,8))
    ax.plot(timeVals,stockVals,'b-')
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.savefig("plots/stockPath.png")
    plt.plot()



