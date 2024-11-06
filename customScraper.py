from bs4 import BeautifulSoup
import requests
import logging


try:
    page_to_scrape = requests.get("https://www.nasdaq.com/market-activity/stocks/aapl/option-chain", timeout=1)
except:
    logging.error("timeout raised, recovering")