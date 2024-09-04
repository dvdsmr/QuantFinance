# Quantitative Finance with C++
This repo contains a code basis for quantitative methods in finance, coded C++ and visualized using Python. 

We can showcase the price surfaces in the Black-Scholes-Merton model for european call and put options.

European call             |  European put
:-------------------------:|:-------------------------:
![Call](Plots/priceSurface.png) |  ![Put](Plots/priceSurfacePut.png)


The repository contains tools to simulate modern stochastic models for stock movements. The example plot below shows three paths of a Heston model, used to to model stock prices with a stochastic volatility process.


![Heston model paths.](Plots/stockPath.png)

Canonical models for derivative pricing are included. The below plot shows the development of the price, Delta and Gamma of a european call option in the Black-Scholes-Merton model as time to maturity increases.

![European call option in BSM model.](Plots/optionPrice.gif)
