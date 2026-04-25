#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;
// enum for account types
enum AccountType { CHECKING = 1, SAVINGS = 2, STUDENT = 3 };
// struct to store each transaction
struct Transaction {
    char type;     // 'D' = deposit, 'W' = withdraw, 'F' = fee
    double amount; 
    string memo;   
};
// struct to store account info
struct Account {
    string holder;          // account holder name
    double balance;         // current balance
    AccountType type;       // account type
    bool pinSet = false;    // check if pin exists
    unsigned long pinHash;  // hashed pin value
    Transaction* transactions = nullptr; // dynamic array
    int txnCount = 0;       // number of transactions
};

void printHeader();
bool isValidName(const string& name);
string readValidName();
unsigned long hashPin(const string& pin);
bool isValidPin(const string& p);
bool setOrChangePin(Account& acc);
AccountType chooseAccountType();
string accountTypeToString(AccountType t);
void recordTransaction(Account& acc, char type, double amount, const string& memo);
bool requirePin(Account& acc);
void withdraw(Account& acc);
string makeFileName(const string& holder);
void saveToFile(const Account& acc);
bool loadFromFile(Account& acc);

// new functions I added (partner 2)
void deposit(Account& acc);
void showDetails(Account& acc);
void viewTransactionsByType(Account& acc);


int main() {
    printHeader(); // print program header
    
    Account myAccount;
    myAccount.holder = readValidName(); // get valid name
    
    if (!loadFromFile(myAccount)) {     // try to load file, if not found create new account
        cout << "No existing account found. Creating new account...\n";
        cout << "Enter initial balance: $";
        cin >> myAccount.balance; 
        myAccount.type = chooseAccountType(); 
        
        recordTransaction(myAccount, 'D', myAccount.balance, "Initial Balance");         // record initial balance as deposit
    } else {
        cout << "Welcome back, " << myAccount.holder << "!" << endl;
    }

    int choice = 0;

    while (choice != 6) {     // main menu loop
        cout << "\n------------------------------------------------------------\n";
        cout << "Account Holder: " << myAccount.holder 
             << " | Type: " << accountTypeToString(myAccount.type) << endl;
        cout << "Balance: $" << fixed << setprecision(2) << myAccount.balance << endl;
        cout << "------------------------------------------------------------\n";
        
        cout << "1) Deposit\n2) Withdraw\n3) Show Account Details\n4) Set/Change PIN\n5) View by Type\n6) Exit\n";
        cout << "Select an option (1-6): ";
        cin >> choice;

        switch (choice) {
            case 1:
                deposit(myAccount); // deposit money
                break;
            case 2:
                withdraw(myAccount); // withdraw money
                break;
            case 3:
                showDetails(myAccount); // show account info
                break;
            case 4:
                setOrChangePin(myAccount); // set/change pin
                saveToFile(myAccount); 
                break;
            case 5:
                viewTransactionsByType(myAccount); // filter transactions
                break;
            case 6:
                saveToFile(myAccount); // save before exiting
                cout << "Thank you for using the Bank Account Simulator. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    delete[] myAccount.transactions; // free memory
    return 0;
}


// prints header info
void printHeader() {
    cout << "+----------------------------------------------------------------------+" << endl;
    cout << "|                   Computer Science and Engineering                   |" << endl;
    cout << "|                    CSCE 1030 - Computer Science I                    |" << endl;
    cout << "|  Gabriella Vazquez           gv0244   GabriellaVazquez@my.unt.edu    |" << endl;
    cout << "|  Perla Diana Sanchez Rincon  pds0137  perlasanchezrincon@my.unt.edu  |" << endl;
    cout << "+----------------------------------------------------------------------+" << endl;
    cout << "\n======================== Bank Account Simulator ========================" << endl;
    cout << endl;
}

bool isValidName(const string& name) { // checks if name is valid
    if (name.empty()) return false;
    for (char c : name) {
        // only allow letters, digits, spaces
        if (!isalpha(c) && !isdigit(c) && !isspace(c)) return false;
    }
    return true;
}

string readValidName() { // keeps asking until valid name is entered
    string name;
    cout << "Enter account holder name: ";
    getline(cin >> ws, name);
    while (!isValidName(name)) {
        cout << "Name can only contain letters, digits, and spaces. Try again: ";
        getline(cin, name);
    }
    return name;
}

unsigned long hashPin(const string& pin) { // hashes pin using formula
    unsigned long h = 0;
    for (int i = 0; i < (int)pin.size(); i++) {
        h = h * 31 + (unsigned char)pin.at(i);
    }
    return h;
}

bool isValidPin(const string& p) { // checks if pin is 4-6 digits
    if (p.length() < 4 || p.length() > 6) return false;
    for (char c : p) if (!isdigit(c)) return false;
    return true;
}

bool setOrChangePin(Account& acc) { // sets or changes pin
    string p1, p2;
    cout << "Set PIN (4-6 digits): ";
    cin >> p1;
    cout << "Confirm PIN: ";
    cin >> p2;

    if (p1 == p2 && isValidPin(p1)) {
        acc.pinHash = hashPin(p1);
        acc.pinSet = true;
        cout << "PIN set." << endl;
        return true;
    } else {
        cout << "Mismatch or invalid PIN. Try again." << endl;
        return false;
    }
}

AccountType chooseAccountType() { // lets user pick account type
    int choice;
    do {
        cout << "Choose account type:\n1) Checking\n2) Savings\n3) Student\nEnter (1-3): ";
        cin >> choice;
    } while (choice < 1 || choice > 3);
    return static_cast<AccountType>(choice);
}

string accountTypeToString(AccountType t) { // converts enum to string
    if (t == CHECKING) return "Checking";
    if (t == SAVINGS) return "Savings";
    return "Student";
}

// adds new transaction (dynamic array resize)
void recordTransaction(Account& acc, char type, double amount, const string& memo) {
    Transaction* newArr = new Transaction[acc.txnCount + 1];

    for (int i = 0; i < acc.txnCount; i++) {     // copy old transactions
        newArr[i] = acc.transactions[i];
    }

    newArr[acc.txnCount].type = type;     // add new one
    newArr[acc.txnCount].amount = amount;
    newArr[acc.txnCount].memo = memo;

    delete[] acc.transactions; // delete old array
    acc.transactions = newArr;
    acc.txnCount++;

    saveToFile(acc); // save after every change
}

bool requirePin(Account& acc) { // makes sure pin is correct
    if (!acc.pinSet) {
        cout << "No PIN set. Create one to continue." << endl; 
        return setOrChangePin(acc);
    }
    string input;
    cout << "Enter PIN: ";
    cin >> input;
    if (hashPin(input) == acc.pinHash) return true;

    cout << "Incorrect PIN." << endl; 
    return false;
}

void withdraw(Account& acc) { // handles withdraw logic
    if (!requirePin(acc)) return; 

    double amount;
    string memo;

    cout << "Enter withdrawal amount: $";
    cin >> amount;

    cout << "Enter memo: ";
    getline(cin >> ws, memo);

    double newBal = acc.balance - amount; 

    if (newBal >= 0) {
        acc.balance = newBal;
        recordTransaction(acc, 'W', amount, memo); 
        cout << "Withdrew $" << fixed << setprecision(2) << amount 
             << ". New balance: $" << acc.balance << endl;

    } else if (acc.type == CHECKING) {
        // allow overdraft with fee
        acc.balance = newBal - 35.0; 
        recordTransaction(acc, 'W', amount, memo); 
        recordTransaction(acc, 'F', 35.0, "Overdraft Fee"); 

        cout << "Overdraft! Fee $35.00 applied. New balance: $" 
             << acc.balance << endl;

    } else {
        cout << "Withdrawal denied. Insufficient funds." << endl;
    }
}

string makeFileName(const string& holder) { // creates file name
    return holder + ".txt"; 
}

void saveToFile(const Account& acc) { // saves account to file
    ofstream outFile(makeFileName(acc.holder)); 

    if (outFile.is_open()) {
        outFile << acc.holder << "," << acc.balance << "," << (int)acc.type << "," 
                << (acc.pinSet ? 1 : 0) << "," << acc.pinHash << endl;

        outFile << acc.txnCount << endl;

        for (int i = 0; i < acc.txnCount; i++) {
            outFile << acc.transactions[i].type << "," 
                    << acc.transactions[i].amount << "," 
                    << acc.transactions[i].memo << endl;
        }

        outFile.close();
    }
}

bool loadFromFile(Account& acc) { // loads account from file
    ifstream inFile(makeFileName(acc.holder));
    if (!inFile.is_open()) return false;

    string line, temp;

    getline(inFile, acc.holder, ',');
    getline(inFile, temp, ','); acc.balance = stod(temp);
    getline(inFile, temp, ','); acc.type = static_cast<AccountType>(stoi(temp));
    getline(inFile, temp, ','); acc.pinSet = (temp == "1");
    getline(inFile, line); acc.pinHash = stoul(line);

    if (!(inFile >> acc.txnCount)) return true;

    if (acc.txnCount > 0) {
        acc.transactions = new Transaction[acc.txnCount];

        for (int i = 0; i < acc.txnCount; i++) {
            getline(inFile >> ws, temp, ','); acc.transactions[i].type = temp[0];
            getline(inFile, temp, ','); acc.transactions[i].amount = stod(temp);
            getline(inFile, acc.transactions[i].memo);
        }
    }

    inFile.close();
    return true;
}


// handles deposit
void deposit(Account& acc) {
    if (!requirePin(acc)) return;

    double amount;
    string memo;

    cout << "Enter deposit amount: $";
    cin >> amount;

    cout << "Enter memo: ";
    getline(cin >> ws, memo);

    if (amount <= 0) {
        cout << "Invalid amount." << endl;
        return;
    }

    acc.balance += amount;
    recordTransaction(acc, 'D', amount, memo);

    cout << "Deposited $" << fixed << setprecision(2) << amount
         << ". New balance: $" << acc.balance << endl;
}

// shows account info + recent transactions
void showDetails(Account& acc) {
    cout << "\n================ Account Details ================\n";
    cout << "Holder:  " << acc.holder << endl;
    cout << "Type:    " << accountTypeToString(acc.type) << endl;
    cout << "Balance: $" << fixed << setprecision(2) << acc.balance << endl;

    if (acc.pinSet)
        cout << "PIN:     (set)" << endl;
    else
        cout << "PIN:     (not set)" << endl;

    cout << "---------------- Recent Transactions ----------------\n";

    if (acc.txnCount == 0) {
        cout << "(No transactions yet)" << endl;
    } else {
        int shown = 0;

        for (int i = acc.txnCount - 1; i >= 0 && shown < 10; i--) {   // show newest first

            string label;

            if (acc.transactions[i].type == 'D')
                label = "[Deposit ]";
            else if (acc.transactions[i].type == 'W')
                label = "[Withdraw]";
            else
                label = "[  Fee  ]";

            cout << label << " $" << fixed << setprecision(2)
                 << acc.transactions[i].amount
                 << " | " << acc.transactions[i].memo << endl;

            shown++;
        }
    }

    cout << "----------------------------------------------------\n";
    cout << "====================================================\n";
}

void viewTransactionsByType(Account& acc) { // shows transactions by type (D/W/F)
    char type;
    cout << "Show which transactions? (D/W/F): ";
    cin >> type;

    if (type >= 'a' && type <= 'z') { // convert lowercase to uppercase

        type = type - ('a' - 'A');
    }

    bool found = false;

    for (int i = acc.txnCount - 1; i >= 0; i--) {
        if (acc.transactions[i].type == type) {
            string label;

            if (type == 'D')
                label = "[Deposit ]";
            else if (type == 'W')
                label = "[Withdraw]";
            else
                label = "[  Fee  ]";

            cout << label << " $" << fixed << setprecision(2)
                 << acc.transactions[i].amount
                 << " | " << acc.transactions[i].memo << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "(No transactions of that type)" << endl;
    }
}
