//
//  main.cpp
//  finalprj_Pair-Trading
//
//  Created by Deyang Van on 4/27/22.
//
#include <iostream>
#include "Database.hpp"
#include "MarketData.hpp"
#include <string>
#include <map>
#include <vector>
#include "Util.hpp"
#include <chrono>
#include <unistd.h>
#include <thread>


using namespace std;


int main() {
    
    string path_in = "PairTrading.txt";
    vector<StockPairPrices> all_Pairs;
    string database_name = "pair_trading.db";
    cout << "Opening Database..." << endl;
    sqlite3* db = NULL;
    
    if (OpenDatabase(database_name.c_str(), db) != 0)  return -1;
    
    string t_start = "2012-01-01"; // start from 2012-01-01
    string t_end = "2022-04-30";
    char selection;
    do
    {
        std::cout << endl;
        std::cout << "Menu" << endl;
        std::cout << "========" << endl;
        std::cout << "A - Create and Populate Pair Table" << endl;
        std::cout << "B - Retrieve and Populate Historical Data for Each Stock" << endl;
        std::cout << "C - Create PairPrices Table" << endl;
        std::cout << "D - Calculate Violatility" << endl;
        std::cout << "E - Back Test" << endl;
        std::cout << "F - Calculate Profit and Loss for Each Pair" << endl;
        std::cout << "G - Manual Testing" << endl;
        std::cout << "H - Drop All the Tables" << endl << endl;
        std::cout << "X - Exit" << endl << endl;
        
        std::cout <<"Enter selection: ";
        std::cin >> selection;
        switch (selection) {
            case 'A':
            case 'a':
            {
                // Create and Populate Pair Table, not populated with vol yet
                PopulatePairs(all_Pairs, path_in); // read the PairTrading.txt and populate stockPair Prices
                for (int i =0; i<(int)all_Pairs.size(); i++){
                    cout<< all_Pairs[i].GetStockPair().first<< " "<<all_Pairs[i].GetStockPair().second<<endl;
                }
                if (Create_PairTable(all_Pairs, db) == -1){
                    return -1;
                }
                cout <<endl << "A Completed!" << endl;
                break;
            }
            case 'B':
            case 'b':
            {
                // Retrieve and Populate Historical Data for Each Stock
                auto start = chrono::steady_clock::now();
                // start
                map<string, Stock> firstStocks_A;
                map<string, Stock> firstStocks_B;
                map<string, Stock> secondStocks_A;
                map<string, Stock> secondStocks_B;
                
                int lenth = (int)all_Pairs.size();
                for (int i =0; i<lenth ; i++)
                {
                    pair<string, string> stockpair;
                    stockpair=all_Pairs[i].GetStockPair();
                    string symbol1 = stockpair.first.c_str();
                    string symbol2 = stockpair.second.c_str();
                    vector<TradeData> atrade_;
                    Stock stock1(symbol1, atrade_);
                    Stock stock2(symbol2, atrade_);
                    
                    if (i <= (int)lenth/2) {
                        firstStocks_A[symbol1] = stock1;
                        secondStocks_A[symbol2] = stock2;
                    } else {
                        firstStocks_B[symbol1] = stock1;
                        secondStocks_B[symbol2] = stock2;
                    }
                    
                }
                if (Create_PairOneTwoPricesTable(db) != 0) return -1;
                
                thread t1(FetchData, ref(firstStocks_A), t_start, t_end);
                thread t2(FetchData, ref(secondStocks_A), t_start, t_end);
                thread t3(FetchData, ref(firstStocks_B), t_start, t_end);
                thread t4(FetchData, ref(secondStocks_B), t_start, t_end);
                t1.join();
                t2.join();
                t3.join();
                t4.join();
                
                firstStocks_A.insert(firstStocks_B.begin(), firstStocks_B.end());
                secondStocks_A.insert(secondStocks_B.begin(), secondStocks_B.end());
                thread tpop1(PopulatePairPrices_tables, ref(db), ref(firstStocks_A), true);
                thread tpop2(PopulatePairPrices_tables, ref(db), ref(secondStocks_A), false);
                tpop1.join();
                tpop2.join();

                
                
                auto end = chrono::steady_clock::now();
                cout    << "Elapsed time in seconds: "
                        << chrono::duration_cast<chrono::seconds>(end - start).count()
                        << " sec" << endl;
                cout <<endl << "B Completed!" << endl;
                break;
            }
                
            case 'C':
            case 'c':
            {

                if(Create_PairPricesTable(db)!=0) return -1;
                if(Insert_intoPairPrices(db)!=0) return -1;
                cout << endl << "C Completed!" << endl;
                break;
            }
            case 'D':
            case 'd':
            {
                // Calculate Violatility
                string back_test_start_date_ = "2021-12-31";
                if(Calculate_Vol(db, back_test_start_date_)!=0) return -1;
                cout << endl << "D Completed!" << endl;
                break;
            }
            case 'E':
            case 'e':
            {
                // Back Test, 1/1/2022 - now
                float k_mutiple;
                cout << "Please enter a volatility mutiplier: ";
                cin >> k_mutiple;
                if (Calc_PnL(db, k_mutiple) != 0) return -1;
                cout << endl << "E Completed!" << endl;
                break;
            }
            case 'F':
            case 'f':
            {
                // Calculate Profit and Loss for Each Pair
                if (Calc_PnL_cumulative(db) != 0) return -1;
                string sql_showtable  = "SELECT * FROM StockPairs ; ";
                ShowTable( db, sql_showtable.c_str());
                cout << endl << "F Completed!" << endl;
                break;
            }
            case 'G':
            case 'g':
            {
                string symbol1, symbol2, test_date;
                float open1d2, close1d2, open2d2, close2d2;
                float k;
                cout << endl;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter Symbol1: ";
                cin >> symbol1;
                cout << "Enter Open Price for " + symbol1 +": " ;
                cin >> open1d2;
                cout << "Enter Close Price for " + symbol1 +": " ;
                cin >> close1d2;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter Symbol2: ";
                cin >> symbol2;
                cout << "Enter Open Price for " + symbol2 +": " ;
                cin >> open2d2;
                cout << "Enter Close Price for " + symbol2 +": ";
                cin >> close2d2;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter k: ";
                cin >> k;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << "Enter previous trade date for testing purpose in format of xxxx-xx-xx (should only be a TRADE DATE): " ;
                cin >> test_date;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout << " " << endl;
                cout << " " << endl;
                
                string sql_get_vol = string("SELECT volatility FROM StockPairs ")
                +"WHERE StockPairs.symbol1 = '"
                +symbol1
                +"' AND StockPairs.symbol2 = '"
                +symbol2+"';";
                
                char **results_vol = NULL;
                int rows_vol, columns_vol;
                char *error_vol = nullptr;
                sqlite3_get_table(db, sql_get_vol.c_str(), &results_vol, &rows_vol, &columns_vol, &error_vol);
                if (error_vol) {
                    cout << error_vol;
                }
                float sigma = stof(results_vol[1]);
                
                
                cout << "TESTING SUMMARY" << endl;
                cout << "~~~~~~~~~~~~~~~~~" << endl;
                cout<< "- Vol for ( " + symbol1 + " , " + symbol2 + " ) is: " <<sigma<<endl;
                
                string sql_get_price = string("SELECT open1, close1, open2, close2 FROM PairPrices ")
                +"WHERE PairPrices.symbol1 = '"+symbol1+"' AND PairPrices.symbol2 = '"+symbol2+"' AND PairPrices.date = \'"
                + test_date + "\';";
                
                char **results_price = NULL;
                int rows_price, columns_price;
                char *error_price = nullptr;
                sqlite3_get_table(db, sql_get_price.c_str(), &results_price, &rows_price, &columns_price, &error_price);
                if (error_price) {
                    cout << error_price;
                }
                
                float close1d1 = stof(results_price[5]);
                float close2d1 = stof(results_price[7]);
                
                float pnl = Calc_pnl_manual(sigma,k,open1d2,close1d1,close1d2,open2d2,close2d1,close2d2);
                
                
                //float price = stof(results_price[0]);
                cout<<"- PnL for pair "+symbol1+" and "+symbol2+" is: "<<pnl<<endl << endl;
                cout << endl << "G Completed!" << endl;
                break;
            }
            case 'H':
            case 'h':
            {
                string sql_dropPOP = " DROP TABLE IF EXISTS PairOnePrices ";;
                string sql_dropPTP = "DROP TABLE IF EXISTS PairTwoPrices;";
                string sql_dropPP = "DROP TABLE IF EXISTS PairPrices;";
                string sql_dropSP = "DROP TABLE IF EXISTS StockPairs;";
                string sql_foreign_off = " PRAGMA foreign_keys=off;";
                string sql_foreign_dropon = " PRAGMA foreign_keys=on; ";
                
                if (ExecuteSQL(db, sql_foreign_off.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "Foreign key check is off ..." << endl;
                }
                if (DropTable(db, sql_dropPOP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairOnePrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropPTP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairTwoPrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropPP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "PairPrices table is dropped" << endl;
                }
                if (DropTable(db, sql_dropSP.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "StockPairs table is dropped" << endl;
                }
                if (ExecuteSQL(db, sql_foreign_dropon.c_str()) != 0) {
                    return -1;
                } else {
                    cout << "Foreign key check is on !" << endl;
                }

                cout << endl << "H Completed!" << endl;
                break;
            }
            case 'X':
            case 'x':
            {
                break;
            }
            default:
            {
                cerr << "Invalid selection. Choose again." << endl;
                break;
            }
        }
        cout << endl;
        
    } while (selection != 'X' and selection != 'x');
    
    
    CloseDatabase(db);
    
    return 0;
}

/*
 Opening Database...
 Opened pair_trading.db

 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: a
 AAPL HPQ
 BAC JPM
 BHP FCX
 CAT DE
 CHK DVN
 COP CVX
 CS DB
 CSX NSC
 CVX XOM
 DAL UAL
 DSX GNK
 EGLE GNK
 HD LOW
 IEF TLT
 INTC SMH
 KGC NEM
 T VZ
 UGA USO
 Created 1 table

 A Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: b
 Finished Creating PairOnePrices & PairTwoPrices
  
 GNK: Successfully parsing JSON
 CVX: Successfully parsing JSON
 DSX: Successfully parsing JSON
 AAPL: Successfully parsing JSON
 LOW: Successfully parsing JSON
 DB: Successfully parsing JSON
 BAC: Successfully parsing JSON
 EGLE: Successfully parsing JSON
 NEM: Successfully parsing JSON
 DE: Successfully parsing JSON
 HD: Successfully parsing JSON
 BHP: Successfully parsing JSON
 SMH: Successfully parsing JSON
 DVN: Successfully parsing JSON
 IEF: Successfully parsing JSON
 CAT: Successfully parsing JSON
 TLT: Successfully parsing JSON
 FCX: Successfully parsing JSON
 INTC: Successfully parsing JSON
 CHK: Successfully parsing JSON
 HPQ: Successfully parsing JSON
 USO: Successfully parsing JSON
 KGC: Successfully parsing JSON
 COP: Successfully parsing JSON
 JPM: Successfully parsing JSON
 VZ: Successfully parsing JSON
 T: Successfully parsing JSON
 CS: Successfully parsing JSON
 NSC: Successfully parsing JSON
 UGA: Successfully parsing JSON
 CSX: Successfully parsing JSON
 UAL: Successfully parsing JSON
 CVX: Successfully parsing JSON
 XOM: Successfully parsing JSON
 DAL: Successfully parsing JSON
 CVX : Successfully inserted
 AAPL : Successfully inserted
 DB : Successfully inserted
 BAC : Successfully inserted
 DE : Successfully inserted
 BHP : Successfully inserted
 DVN : Successfully inserted
 CAT : Successfully inserted
 CHK : Successfully inserted
 FCX : Successfully inserted
 GNK : Successfully inserted
 COP : Successfully inserted
 HPQ : Successfully inserted
 CS : Successfully inserted
 CSX : Successfully inserted
 JPM : Successfully inserted
 LOW : Successfully inserted
 CVX : Successfully inserted
 NEM : Successfully inserted
 DAL : Successfully inserted
 DSX : Successfully inserted
 NSC : Successfully inserted
 EGLE : Successfully inserted
 SMH : Successfully inserted
 HD : Successfully inserted
 TLT : Successfully inserted
 UAL : Successfully inserted
 IEF : Successfully inserted
 USO : Successfully inserted
 INTC : Successfully inserted
 KGC : Successfully inserted
 VZ : Successfully inserted
 XOM : Successfully inserted
 T : Successfully inserted
 UGA : Successfully inserted
 Elapsed time in seconds: 43 sec

 B Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: c
 Creating PairPrices table ...
 PairPrices table created.
 Populating PairPrices table ...
 PairPrices table populated .

 C Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: d
 Populating Volatility in StockPrices table ...

 D Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: e
 Please enter a volatility mutiplier: 1

 E Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: f
 id           symbol1      symbol2      volatility   profit_loss
 ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~ ~~~~~~~~~~~~
 1            AAPL         HPQ          1.58545      -183695
 2            BAC          JPM          0.000479807  53368.7
 3            BHP          FCX          0.875849     121783
 4            CAT          DE           0.0225316    108791
 5            CHK          DVN          0.0580281    -27723.3
 6            COP          CVX          0.0069038    135475
 7            CS           DB           0.0912074    -1465.77
 8            CSX          NSC          0.000188582  -17862
 9            CVX          XOM          0.0825573    232101
 10           DAL          UAL          0.0195749    -26791.7
 11           DSX          GNK          0.0125231    -9377.16
 12           EGLE         GNK          200.884      1638.78
 13           HD           LOW          0.0294213    -26059.8
 14           IEF          TLT          0.00487944   19672.5
 15           INTC         SMH          0.0159747    103929
 16           KGC          NEM          0.00188555   -8087.68
 17           T            VZ           0.00433099   26443.2
 18           UGA          USO          0.0320085    -20615.2

 F Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: g

 ~~~~~~~~~~~~~~~~~
 Enter Symbol1: AAPL
 Enter Open Price for AAPL: 156.3
 Enter Close Price for AAPL: 162.3
 ~~~~~~~~~~~~~~~~~
 Enter Symbol2: HPQ
 Enter Open Price for HPQ: 36.5
 Enter Close Price for HPQ: 35.6
 ~~~~~~~~~~~~~~~~~
 Enter k: 1
 ~~~~~~~~~~~~~~~~~
 Enter previous trade date for testing purpose in format of xxxx-xx-xx (should only be a TRADE DATE): 2022-04-29
 ~~~~~~~~~~~~~~~~~
  
  
 TESTING SUMMARY
 ~~~~~~~~~~~~~~~~~
 - Vol for ( AAPL , HPQ ) is: 1.58545
 - Action: Long
 - PnL for pair AAPL and HPQ is: 98539


 G Completed!


 Menu
 ========
 A - Create and Populate Pair Table
 B - Retrieve and Populate Historical Data for Each Stock
 C - Create PairPrices Table
 D - Calculate Violatility
 E - Back Test
 F - Calculate Profit and Loss for Each Pair
 G - Manual Testing
 H - Drop All the Tables

 X - Exit

 Enter selection: x

 Program ended with exit code: 0
 */
