#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

// 1. ENUMS AND STRUCTS
enum AccountType { CHECKING = 1, SAVINGS = 2, STUDENT = 3 };

struct Transaction {
    char type;     // 'D', 'W', or 'F' 
    double amount; 
    string memo;   
};

struct Account {
    string holder;
    double balance;
    AccountType type;
    bool pinSet = false;
    unsigned long pinHash;
    Transaction* transactions = nullptr; 
    int txnCount = 0;
};

// 2. FUNCTION PROTOTYPES
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

// --- PARTNER B STUBS ---
// void deposit(Account& acc);
// void showDetails(Account& acc);
// void viewTransactionsByType(Account& acc);

// 3. MAIN ENGINE
int main() {
    printHeader(); 
    
    Account myAccount;
    myAccount.holder = readValidName();
    
    // Logic for loading or creating account
    if (!loadFromFile(myAccount)) {
        cout << "No existing account found. Creating new account...\n";
        cout << "Enter initial balance: $";
        cin >> myAccount.balance; 
        myAccount.type = chooseAccountType(); 
        recordTransaction(myAccount, 'D', myAccount.balance, "Initial Balance");
    } else {
        cout << "Welcome back, " << myAccount.holder << "!" << endl;
    }

    int choice = 0;
    while (choice != 6) { 
        cout << "\n------------------------------------------------------------\n";
        cout << "Account Holder: " << myAccount.holder << " | Type: " << accountTypeToString(myAccount.type) << endl;
        cout << "Balance: $" << fixed << setprecision(2) << myAccount.balance << endl;
        cout << "------------------------------------------------------------\n";
        
        cout << "1) Deposit\n2) Withdraw\n3) Show Account Details\n4) Set/Change PIN\n5) View by Type\n6) Exit\n";
        cout << "Select an option (1-6): ";
        cin >> choice;

        switch (choice) {
            case 1:
                // deposit(myAccount); // Partner B Task
                break;
            case 2:
                withdraw(myAccount); 
                break;
            case 3:
                // showDetails(myAccount); // Partner B Task
                break;
            case 4:
                setOrChangePin(myAccount);
                saveToFile(myAccount); 
                break;
            case 5:
                // viewTransactionsByType(myAccount); // Partner B Task
                break;
            case 6:
                saveToFile(myAccount);
                cout << "Thank you for using the Bank Account Simulator. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    delete[] myAccount.transactions; 
    return 0;
}

// 4. FUNCTION DEFINITIONS

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

bool isValidName(const string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!isalpha(c) && !isdigit(c) && !isspace(c)) return false;
    }
    return true;
}

string readValidName() {
    string name;
    cout << "Enter account holder name: ";
    getline(cin >> ws, name); // Clear buffer and get name
    while (!isValidName(name)) {
        cout << "Name can only contain letters, digits, and spaces. Try again: ";
        getline(cin, name);
    }
    return name;
}

unsigned long hashPin(const string& pin) {
    unsigned long h = 0;
    for (int i = 0; i < (int)pin.size(); i++) {
        h = h * 31 + (unsigned char)pin.at(i);
    }
    return h;
}

bool isValidPin(const string& p) {
    if (p.length() < 4 || p.length() > 6) return false;
    for (char c : p) if (!isdigit(c)) return false;
    return true;
}

bool setOrChangePin(Account& acc) {
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

AccountType chooseAccountType() {
    int choice;
    do {
        cout << "Choose account type:\n1) Checking\n2) Savings\n3) Student\nEnter (1-3): ";
        cin >> choice;
    } while (choice < 1 || choice > 3);
    return static_cast<AccountType>(choice);
}

string accountTypeToString(AccountType t) {
    if (t == CHECKING) return "Checking";
    if (t == SAVINGS) return "Savings";
    return "Student";
}

void recordTransaction(Account& acc, char type, double amount, const string& memo) {
    Transaction* newArr = new Transaction[acc.txnCount + 1];
    for (int i = 0; i < acc.txnCount; i++) {
        newArr[i] = acc.transactions[i];
    }
    newArr[acc.txnCount].type = type;
    newArr[acc.txnCount].amount = amount;
    newArr[acc.txnCount].memo = memo;

    delete[] acc.transactions;
    acc.transactions = newArr;
    acc.txnCount++;
    saveToFile(acc); 
}

bool requirePin(Account& acc) {
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

void withdraw(Account& acc) {
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
        cout << "Withdrew $" << fixed << setprecision(2) << amount << ". New balance: $" << acc.balance << endl;
    } else if (acc.type == CHECKING) { 
        acc.balance = newBal - 35.0; 
        recordTransaction(acc, 'W', amount, memo); 
        recordTransaction(acc, 'F', 35.0, "Overdraft Fee"); 
        cout << "Overdraft! Fee $35.00 applied. New balance: $" << acc.balance << endl;
    } else {
        cout << "Withdrawal denied. Insufficient funds." << endl;
    }
}

string makeFileName(const string& holder) { return holder + ".txt"; }

void saveToFile(const Account& acc) {
    ofstream outFile(makeFileName(acc.holder)); 
    if (outFile.is_open()) {
        outFile << acc.holder << "," << acc.balance << "," << (int)acc.type << "," 
                << (acc.pinSet ? 1 : 0) << "," << acc.pinHash << endl;
        outFile << acc.txnCount << endl;
        for (int i = 0; i < acc.txnCount; i++) {
            outFile << acc.transactions[i].type << "," << acc.transactions[i].amount 
                    << "," << acc.transactions[i].memo << endl;
        }
        outFile.close();
    }
}

bool loadFromFile(Account& acc) {
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