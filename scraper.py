"""
    File for scraping options data from the web.
    Included the standard example from https://github.com/aosingh/options_scraper
"""


from options_scraper.scraper import NASDAQOptionsScraper
from options_scraper.utils import batched

scraper = NASDAQOptionsScraper()
ticker_symbol = 'XOM'
kwargs = { "money": 'all',
           "expir": 'week',
           "excode": None,
           "callput": None
         }

records_generator = scraper(ticker_symbol, **kwargs)


# Either access each record individually as shown below
for item in records_generator:
    print(item)

# Or use the batched util to get a list of items
for items in batched(records_generator, batch_size=100):
    print(items)