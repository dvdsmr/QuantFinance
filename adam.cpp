#include "adam.h"
#include "options.h"
#include <cmath>
#include <iostream>


void testAdam()
{
    // we test Adam on calibrating vol to an options price
    double spot = 171.01;
    double strike = 180.0;
    double maturity = 1.0;
    double interest = 0.03;
    double dividendYield = 0.0;
    double trueVol{ 0.3 };

    double truePrice{ Options::Pricing::BSM::call(interest, trueVol, maturity, strike, spot, dividendYield) };

    auto func
    {
        [&](double vol) {
            double price{ Options::Pricing::BSM::call(interest, vol, maturity, strike, spot, dividendYield) };
            return (price - truePrice)*(price-truePrice); 
        }
    };
    auto deriv
    {
        [&](double vol) {
            double price{ Options::Pricing::BSM::call(interest, vol, maturity, strike, spot, dividendYield) };
            return 2 * (price - truePrice)* Options::Pricing::BSM::callVega(interest, vol, maturity, strike, spot, dividendYield);
        }
    };
    Adam adam{ 0.2 };
    double optVol{ adam.optimize(func, deriv) };
    std::cout << "Adam found vol of " << optVol << ". True vol is " << trueVol << ".\n";
}