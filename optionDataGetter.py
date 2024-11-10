import pandas as pd
import numpy as np
import yfinance as yf
import datetime

def options_chain(symbol):

    ticker = yf.Ticker(symbol)

    # Expiration dates
    dates = ticker.options

    # Get options for each expiration
    options = pd.DataFrame()
    for date in dates:
        opt = ticker.option_chain(date)
        opt = pd.concat([opt.calls,opt.puts], ignore_index=True)
        opt['expirationDate'] = date
        options = pd.concat([options,opt], ignore_index=True)

    # Expiration and time to maturity in years
    options['expirationDate'] = pd.to_datetime(options['expirationDate']) + datetime.timedelta(days = 1)
    options['toMaturity'] = (options['expirationDate'] - datetime.datetime.today()).dt.days / 365
    
    # Option type
    options['Type'] = options['contractSymbol'].str[4:].apply(
    lambda x: "Call" if "C" in x else "Put" if "P" in x else None
    )
    
    # We want to use the mid price between bid and ask as a "fair" price for our model calibrations
    options['midPrice'] = (options['bid'] + options['ask']) / 2 
    
    # Drop unnecessary columns (might be important later for validation and testing)
    options = options.drop(columns = ['expirationDate','openInterest','volume','contractSymbol','bid','ask',\
                                      'contractSize', 'currency', 'change', 'percentChange', 'lastTradeDate', 'lastPrice'])

    return options


if __name__ == "__main__":

    chain = options_chain("AAPL")
    print(chain)