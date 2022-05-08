# Pair Trading 
- freq = Intraday
- PPT 👇

https://docs.google.com/presentation/d/1_Ji4500JqWLY2TU-w8mcDJqJMqjnOxf-4WcDaaE1KKc/edit#slide=id.g127b8bbc5de_1_26

### May 4 Update - Deyang
- add multithreading techiniques for parallel computing


### May 3 Update3 - Deyang
- beautify files and funcitons

### May 3 Update2 - Deyang
- add showtable function
- add isNum funciton for showing table.

### May 3 Update - Deyang
For Presentation:

Please create a PPT slide presentation with about 10-15 slides to cover the following:

- **Zhuoran**

> (1) Title page: Project name, course name and all the team member names
> 
> (2) A brief executive summary.
> 
> (3) A task allocation page indicates contribution from each team member.

- **Deyang**

> (4)  The diagrams showing your design, such as how you used containers to manage your data, how you implemented all the calculation, the flow charts of your programs, the relationship of your class(es), containers, and independent functions (if you use). UML diagrams are preferred, including E-R model.

- **All team memebrs, each one write their own detailed explaination for functinality**

> (5) The SQL statements you create for your team projects, how you use them in your project.
> 
> (6) Explanations of the functionality and implementation of your class(es), containers and others. But do not just copy and paste your source codes onto slides.

- **Zirui**

> (7) Your conclusion

- **TBD**

> (8) Any enhancement and enrichment you have added to this project.

- **Everyone**

> (9) A reference slide.


### May 1 Update
- Finished menu part A,B,C,D,G by Zuoran, Elaine, Zirui

### Apr 29 Update
- PopulatePairs, Create PairTable :  : Zhuoran Ma
- PopulateStocks, create Pair1Stocks & Pair2Stocks Table: Elaine
- Create & populate PairPrices Table, Calc_Vol: Zirui
- Calc_PnL: Deyang

### Pairs Trading Project
- Design, populate, and query a sqlite3 database for pairs trading.
- Provide SQL queries for pair trading statistics.
- Write an Object-Oriented program in C++ to run the back-test for a given portfolio over a given time period and store the results in the database.
- Design the algorithm to implement real time "grey box" trading using the database and objects you used for the back tester.

#### For Demo

- The market data for 1/2/2012 - 12/31/2021 should be already persistent in your database before demo. In other words, you should not need to retrieve data from EOD Historical in order complete the project demo. The market data after 1/1/2022 is optional, could be either persistent in advance or pull in real time during demo time.

- You could ask to test functionalities of your menu, such as calculation for volatility, back testing, and manual testing.

#### Condition and Assumption:
- (1) “Going short”– the first stock of the pair is short and the other is long. 
- (2) “Going long” - the first stock of the pair is long and the other is short.
- (3) Always trade 10,000 shares for the first stock (S1) and determine the shares for the other (S2) accordingly: N1P1 + N2P2 = 0
- (4) N1 and N2 are the numbers of shares of S1 and S2, and P1 and P2 are the prices of S1 and S2

#### Trading Algorithm:
- (1) Compute the standard deviation, σp, of the ratio of the two adjusted closing stock prices in each pair from 1/2/2012 to 12/31/2021. 
Store this standard deviation in the database.

- (2) The variable, k, has default value 1, but could be changed by user in the pair trading program.

- (3) Get Close1d1, Close2d1, Open1d2, Open2d2, Close1d2, and Close2d2, where Close1d1 and Close2d1 are the closing prices on day d – 1 for stocks 1 and 2, respectively, Open1d2, Open2d2 are the opening prices for day d.

- (4) Open Trade:
    If abs(Close1d1/Close2d1 – Open1d2/Open2d2) >= kσ, 
        short the pair;
    Else go long the pair.
    N1 = 10,000 shares, traded at the price Open1d2, 
    N2 = N1 * (Open1d2/Open2d2), traded at the price Open2d2.

- (5) Close Trade:
    The open trades will be closed at the closing prices and P/L for the pair trade will be calculated as:
        (±N1 * [Close1d2 – Open1d2]) + 
        (±N2 * [Close2d2 – Open2d2])

- (6) Use the stock daily prices from 1/3/2022 to now for back testing

- (7) Your program should support manually entered open and close price for a pair to simulate real-time trading

#### Database Implementation:
    (a) Create 5 tables, Pairs, Pair1Stocks, Pair2Stocks, PairPrices, Trades (Trade table could also be part of PairPrices table):
        Pairs: Pair symbols, volatility, profit_loss (from back_testing),  primary keys: pair symbols
        Pair1Stocks and Pair2Stocks: the daily market data retrieved for each stock in the pair from 1/2/2012 to 12/31/2021, primary keys: symbol and date
        PairPrices: pair symbols, date, open and close price for each stock, primary keys: pair symbols and date
        Trades: pair symbols, date and profit_loss of each day, primary keys: pair symbols and date
    (b) Establish E-R model to enforce the relationship of primary keys and foreign keys for each table.


