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
    options[['bid', 'ask', 'strike']] = options[['bid', 'ask', 'strike']].apply(pd.to_numeric)
    options['midPrice'] = (options['bid'] + options['ask']) / 2 
    
    # Drop unnecessary columns (might be important later for validation and testing)
    options = options.drop(columns = ['expirationDate','openInterest','volume','contractSymbol',\
                                      'contractSize', 'currency', 'change', 'percentChange', 'lastTradeDate', 'lastPrice'])

    return options

def extractPriceSurface(chain,Type='Call'):
    # extract desired payoff type
    modifiedChain = chain[chain['Type'] == Type]
    # extract valid prices
    modifiedChain = chain[chain['midPrice'] > 0.0]

    # extract maturities which are bigger than 0.2 years, discarding messy volatility spikes in short maturites
    modifiedChain = chain[chain['toMaturity'] > 0.2]

    #maturities = modifiedChain['toMaturity'].unique()
    modifiedChain = modifiedChain[['toMaturity','strike','bid','ask','midPrice','impliedVolatility']]
    return modifiedChain


if __name__ == "__main__":

    symbol = "KO"
    chain = options_chain(symbol)
    # chain.to_csv('Data/yFinance/AAPL.csv')

    # stored data
    modChain = extractPriceSurface(chain)
    modChain.to_csv(f"Data/yFinance/{symbol}_callPriceSurface.csv")
    print(modChain)