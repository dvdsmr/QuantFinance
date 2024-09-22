# Quantitative Finance with C++
This repo contains a code basis for quantitative methods in finance, coded in C++ and visualized using Python. 

As an example application, the below plot showcases the price surfaces in the Black-Scholes-Merton model for european call and put options.

European call             |  European put
:-------------------------:|:-------------------------:
![Call](Plots/priceSurface.png) |  ![Put](Plots/priceSurfacePut.png)


The repository contains tools to simulate stochastic processes modelling stock prices. The example plot below shows three paths of a Heston model, used to to model stock prices with a stochastic volatility process.


![Heston model paths.](Plots/stockPath.png | width = 50)

Canonical models for derivative pricing are included. The below plot shows the development of the price, Delta and Gamma of a european call option in the Black-Scholes-Merton model as time to maturity increases.

![European call option in BSM model.](Plots/optionPrice.gif)
