from matplotlib import pyplot as plt
from matplotlib import cm
from matplotlib import animation
import csv
from numpy import genfromtxt
import numpy as np

# for kde
import scipy.stats
import seaborn as sns



plt.rcParams.update({'font.size': 22})


def plotLossCSV(filenames):
    Data = [genfromtxt(filename, delimiter=',') for filename in filenames]
    fig, ax = plt.subplots(figsize=(14, 8))
    for data in Data:
        ax.plot(data[:,0],data[:,1])
    ax.set_xlabel("Vol")
    ax.set_ylabel("Squared error")
    plt.title("BSM loss shape")
    plt.grid(True)
    plt.savefig("Plots/BSMlossShape.png")     
    plt.plot()


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
    fig, ax = plt.subplots(figsize=(14, 8))
    for data in stockData:
        ax.plot(data[:,0],data[:,1])
    ax.set_xlabel("Time")
    ax.set_ylabel("Stock price")
    plt.title("Paths of a Merton Jump Model")
    plt.grid(True)
    plt.savefig("Plots/MJstockPath.png")     
    plt.plot()

def plotHistogramCSV(filenames):
    distData = [genfromtxt(filename, delimiter=',') for filename in filenames]
    n_bins = 50

    labels = ["BSM", "Bachelier", "Heston", "VG", "Merton Jump"]

    # We can set the number of bins with the *bins* keyword argument.
    fig = plt.figure(figsize=(14,8))
    for i in range(len(filenames)):
        dist = distData[i][:,1]
        #density = scipy.stats.gaussian_kde(dist)
        #_, x, _ = plt.hist(dist, bins=n_bins, histtype=u'step', density=True)
        #plt.plot(x, density(x), label=labels[i])
        sns.kdeplot(dist,label=labels[i])
    plt.xlabel("Stock price")
    plt.title("Probability densities for different models")
    plt.yticks([])
    plt.legend()
    plt.savefig("Plots/mcSamplesHistogramMultiModel.png")    
    plt.show()

def plotSurface(filename,plotName):

    plt.rcParams.update({'font.size': 12})

    labels = genfromtxt(filename, delimiter=',', max_rows=1, dtype=None)
    surface = genfromtxt(filename, delimiter=',', skip_header=1)

    fig = plt.figure()
    ax = fig.add_subplot(projection = '3d')

    title = labels[0]
    ylabel = labels[1]
    xlabel = labels[2]
    zlabel = labels[3]

    # make data
    x = surface[0,1:]
    y = surface[1:,0]
    X, Y = np.meshgrid(x, y)
    Z = surface[1:,1:]
    
    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                           linewidth=0, antialiased=False)

    # set labels read from the labeled table
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_zlabel(zlabel)
    plt.title(title)

    # Add a color bar which maps values to colors.
    # fig.colorbar(surf, shrink=0.5, aspect=5)

    # Contours for X and Y axis
    ax.contour(X, Y, Z, cmap = cm.coolwarm, offset = np.max(X), zdir = 'x')
    ax.contour(X, Y, Z, cmap = cm.coolwarm, offset = np.max(Y), zdir = 'y')

    # Filled contour with contour lines
    ax.contour(X, Y, Z, colors = 'black', offset = -1)
    ax.contourf(X, Y, Z, cmap = cm.coolwarm, offset = -1, alpha = 0.75)

    plt.savefig(f"plots/{plotName}.png")
    plt.show()

def plotOptionCSV(filenames):
    data = [genfromtxt(filename, delimiter=',') for filename in filenames]
    fig, ax = plt.subplots(figsize=(14, 8))

    def animate(i):
        ax.clear()
        plt.title(f"European call in BSM model with T={data[0][1+i,0]:.2f}")
        ax.set_xlabel('Spot price')
        ax.set_ylabel('Option price')
        ax.set_ylim(-1.,np.max(data[0][1:,1:]))
        ax.set_xlim(np.min(data[0][0,1:]),np.max(data[0][0,1:]))
        ax.grid(True)
        ax.plot(data[0][0,1:],data[0][1+i,1:],'b-',label="Price")
        ax.plot(data[1][0,1:],data[1][1+i,1:],'r-',label="Delta")
        ax.plot(data[2][0,1:],data[2][1+i,1:],'g-',label="Gamma")
        ax.legend(loc="upper left")

    ani = animation.FuncAnimation(fig, animate, repeat=True,
                                    frames=data[0].shape[0]-1, interval=50)

    writer = animation.PillowWriter(fps=10,
                                    metadata=dict(artist='Me'),
                                    bitrate=1800)
    ani.save('plots/optionPrice.gif', writer=writer)

if __name__ == "__main__":
    # plotLossCSV(["Data/BSMlossCurve"])
    # plotHistogramCSV(["Data/mcSamplesBSM.csv","Data/mcSamplesBachelier.csv","Data/mcSamplesHeston.csv","Data/mcSamplesVarianceGamma.csv",\
    #                  "Data/mcSamplesMertonJump.csv"])
    plotStockCSV(["Data/MJstockPath1.csv","Data/MJstockPath2.csv","Data/MJstockPath3.csv"])
    # plotStockCSV(["Data/VGstockPath1.csv","Data/VGstockPath2.csv","Data/VGstockPath3.csv"])
    # plotOptionCSV(["Data/callPrices.csv","Data/callDeltas.csv","Data/callGammas.csv"])
    # plotSurface("Data/ArtificialPriceSurface.csv","priceSurfacePlot")
    # plotSurface("Data/ArtificalVolSurface.csv","volSurfacePlot")
    # plotSurface("Data/HestonModelPriceSurface.csv","hestonModelPriceSurfacePlot")
    # plotSurface("Data/HestonModelErrorSurface.csv","hestonModelErrorSurfacePlot")
    # plotSurface("Data/BSMModelPriceSurface.csv","bsmModelPriceSurfacePlot")
    # plotSurface("Data/BSMModelErrorSurface.csv","bsmModelErrorSurfacePlot")
    # plotSurface("Data/VGModelPriceSurface.csv","hestonModelPriceSurfacePlot")
    # plotSurface("Data/VGModelErrorSurface.csv","hestonModelErrorSurfacePlot")
