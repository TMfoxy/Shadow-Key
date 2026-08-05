#define _CRT_SECURE_NO_WARNINGS
#define _GLIBCXX_HAS_GTHREADS 0
#define _GNU_SOURCE
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <conio.h>
#include <ctime>

using namespace std;

const string DB_FILE = "shadow_database.txt";
const string SYSTEM_KEY = "admin_key";

// ==========================================
// Clean Screen Helper (Using Newlines)
// ==========================================
void clearScreen() {
    for (int i = 0; i < 30; i++) {
        cout << "\n";
    }
}

// ==========================================
// Hidden Input Helper (For PINs)
// ==========================================
string getHiddenInput() {
    string input = "";
    char ch;
    while (true) {
        ch = _getch();

        if (ch == '\r' || ch == '\n') {
            cout << endl;
            break;
        }
        else if (ch == '\b' || ch == 127) {
            if (!input.empty()) {
                input.pop_back();
                cout << "\b \b";
                cout.flush();
            }
        }
        else {
            input += ch;
            cout << '*';
            cout.flush();
        }
    }
    return input;
}

// ==========================================
// Password Generator Helper
// ==========================================
string generatePassword(int length = 12) {
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+";
    string password = "";
    for (int i = 0; i < length; ++i) {
        password += chars[rand() % chars.length()];
    }
    return password;
}

// ==========================================
// Hexadecimal Conversion Helpers
// ==========================================
string stringToHex(const string& input) {
    string output = "";
    char hex[3];
    for (size_t i = 0; i < input.length(); ++i) {
        sprintf(hex, "%02X", (unsigned char)input[i]);
        output += hex;
    }
    return output;
}

string hexToString(const string& hex) {
    string output = "";
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        output += byte;
    }
    return output;
}

// ==========================================
// XOR Cipher for Encryption and Decryption
// ==========================================
string encryptData(string data, string key) {
    string output = data;
    for (size_t i = 0; i < data.size(); i++) {
        output[i] = data[i] ^ key[i % key.length()];
    }
    return stringToHex(output);
}

string decryptData(string hexData, string key) {
    string data = hexToString(hexData);
    string output = data;
    for (size_t i = 0; i < data.size(); i++) {
        output[i] = data[i] ^ key[i % key.length()];
    }
    return output;
}

// ==========================================
// Helper function to pause and wait for '0'
// ==========================================
void goBackPrompt() {
    string back;
    cout << "\n[Enter 0 to go back]: ";
    while (true) {
        cin >> ws;
        getline(cin, back);
        if (back == "0") {
            break;
        }
        cout << "[-] Please enter 0 to go back: ";
    }
    clearScreen();
}

// ==========================================
// Display Developer Info
// ==========================================
void showDeveloperInfo() {
    clearScreen();
    cout << "\n=================================================\n";
    cout << "             About the Developer                 \n";
    cout << "=================================================\n";
    cout << " [*] Developed by : Mohamed Abdelaziz            \n";
    cout << " [*] Faculty      : FCAI, Cairo University       \n";
    cout << " [*] Instagram    : m0hamed_abdelazizz           \n";
    cout << " [*] Email        : tmfoxy8@gmail.com            \n";
    cout << "=================================================\n";
    goBackPrompt();
}

// ==========================================
// Class to manage the user's password vault
// ==========================================
class PasswordManager {
private:
    string currentUser;
    string userPin;

public:
    PasswordManager(string username, string pin) {
        currentUser = username;
        userPin = pin;
    }

    void savePassword() {
        clearScreen();
        string account, password, passChoice;
        cout << "\n--- Add New Password ---\n";
        cout << "Account/Website name (e.g., Facebook) [0 to cancel]: ";
        cin >> ws;
        getline(cin, account);
        if (account == "0") {
            cout << "[*] Operation canceled.\n";
            goBackPrompt();
            return;
        }

        cout << "1. Enter password manually\n2. Auto-generate strong password\n0. Cancel\nChoose an option: ";
        cin >> ws;
        getline(cin, passChoice);

        if (passChoice == "0") {
            cout << "[*] Operation canceled.\n";
            goBackPrompt();
            return;
        }
        else if (passChoice == "2") {
            password = generatePassword(12);
            cout << "[+] Generated Password: " << password << "\n";
        }
        else {
            cout << "Password [0 to cancel]: ";
            cin >> ws;
            getline(cin, password);
            if (password == "0") {
                cout << "[*] Operation canceled.\n";
                goBackPrompt();
                return;
            }
        }

        ofstream file(DB_FILE, ios::app);
        if (file.is_open()) {
            string record = account + " : " + password;
            file << "P|" << currentUser << "|" << encryptData(record, userPin) << endl;
            file.close();
            cout << "[+] Password saved and encrypted successfully!\n";
        }
        else {
            cout << "[-] Error opening the database file!\n";
        }

        goBackPrompt();
    }

    void viewPasswords() {
        clearScreen();
        ifstream file(DB_FILE);
        string line;
        string searchPrefix = "P|" + currentUser + "|";

        cout << "\n========== Password Vault ==========\n";
        if (file.is_open()) {
            bool hasData = false;
            while (getline(file, line)) {
                if (line.find(searchPrefix) == 0) {
                    hasData = true;
                    string encryptedData = line.substr(searchPrefix.length());
                    cout << decryptData(encryptedData, userPin) << endl;
                }
            }
            if (!hasData) cout << "The vault is currently empty.\n";
            file.close();
        }
        else {
            cout << "[-] Database file not found.\n";
        }
        cout << "====================================\n";

        goBackPrompt();
    }

    void searchPassword() {
        clearScreen();
        string targetAccount;
        cout << "\n--- Search Password ---\n";
        cout << "Enter the account name to search for [0 to cancel]: ";
        cin >> ws;
        getline(cin, targetAccount);

        if (targetAccount == "0") {
            cout << "[*] Operation canceled.\n";
            goBackPrompt();
            return;
        }

        ifstream file(DB_FILE);
        string line;
        string searchPrefix = "P|" + currentUser + "|";
        bool found = false;

        if (file.is_open()) {
            while (getline(file, line)) {
                if (line.find(searchPrefix) == 0) {
                    string encryptedData = line.substr(searchPrefix.length());
                    string decryptedLine = decryptData(encryptedData, userPin);

                    if (decryptedLine.find(targetAccount + " :") == 0) {
                        cout << "\n[+] Account found:\n";
                        cout << ">> " << decryptedLine << "\n";
                        found = true;
                        break;
                    }
                }
            }
            if (!found) cout << "[-] No password found for the account: " << targetAccount << "\n";
            file.close();
        }
        else {
            cout << "[-] Database file not found.\n";
        }

        goBackPrompt();
    }

    void updateOrDeletePassword() {
        clearScreen();
        string targetAccount;
        cout << "\n--- Update / Delete Account ---\n";
        cout << "Enter the account name [0 to cancel]: ";
        cin >> ws;
        getline(cin, targetAccount);

        if (targetAccount == "0") {
            cout << "[*] Operation canceled.\n";
            goBackPrompt();
            return;
        }

        string actionInput;
        cout << "1. Update Password\n2. Delete Account\n0. Cancel\nChoose an option: ";
        cin >> ws;
        getline(cin, actionInput);

        if (actionInput == "0" || (actionInput != "1" && actionInput != "2")) {
            cout << "[*] Operation canceled or invalid choice.\n";
            goBackPrompt();
            return;
        }

        ifstream file(DB_FILE);
        if (!file.is_open()) {
            cout << "[-] Database file not found.\n";
            goBackPrompt();
            return;
        }

        string tempFileName = "temp_" + DB_FILE;
        ofstream tempFile(tempFileName);
        string line;
        string searchPrefix = "P|" + currentUser + "|";
        bool found = false;

        while (getline(file, line)) {
            if (line.find(searchPrefix) == 0) {
                string encryptedData = line.substr(searchPrefix.length());
                string decryptedLine = decryptData(encryptedData, userPin);

                if (decryptedLine.find(targetAccount + " :") == 0) {
                    found = true;

                    if (actionInput == "1") {
                        string newPassword, passChoice;
                        cout << "1. Enter new password manually\n2. Auto-generate strong password\n0. Cancel\nChoose an option: ";
                        cin >> ws;
                        getline(cin, passChoice);

                        if (passChoice == "2") {
                            newPassword = generatePassword(12);
                            cout << "[+] Generated New Password: " << newPassword << "\n";
                        }
                        else if (passChoice == "1") {
                            cout << "Enter new password: ";
                            cin >> ws;
                            getline(cin, newPassword);
                        }
                        else {
                            cout << "[*] Canceled. Keeping old password.\n";
                            tempFile << line << endl;
                            continue;
                        }

                        string newRecord = targetAccount + " : " + newPassword;
                        tempFile << searchPrefix << encryptData(newRecord, userPin) << endl;
                        cout << "[+] Password updated successfully!\n";
                    }
                    else if (actionInput == "2") {
                        cout << "[+] Account deleted successfully!\n";
                    }
                    continue;
                }
            }
            tempFile << line << endl;
        }

        file.close();
        tempFile.close();

        if (!found) {
            cout << "[-] No password found for: " << targetAccount << "\n";
            remove(tempFileName.c_str());
        }
        else {
            remove(DB_FILE.c_str());
            rename(tempFileName.c_str(), DB_FILE.c_str());
        }

        goBackPrompt();
    }

    void exportData() {
        clearScreen();
        cout << "\n--- Export Passwords to CSV ---\n";
        cout << "[SECURITY] Enter your PIN to authorize export [0 to cancel]: ";
        string confirmPin = getHiddenInput();

        if (confirmPin == "0" || confirmPin != userPin) {
            cout << "[-] Canceled or incorrect PIN.\n";
            goBackPrompt();
            return;
        }

        string csvFileName = currentUser + "_passwords.csv";
        ofstream csvFile(csvFileName);
        ifstream dbFile(DB_FILE);
        string line;
        string searchPrefix = "P|" + currentUser + "|";
        bool hasData = false;

        if (csvFile.is_open() && dbFile.is_open()) {
            csvFile << "Website/Account,Password\n";

            while (getline(dbFile, line)) {
                if (line.find(searchPrefix) == 0) {
                    hasData = true;
                    string encryptedData = line.substr(searchPrefix.length());
                    string decryptedLine = decryptData(encryptedData, userPin);

                    size_t delimiterPos = decryptedLine.find(" : ");
                    if (delimiterPos != string::npos) {
                        string acc = decryptedLine.substr(0, delimiterPos);
                        string pwd = decryptedLine.substr(delimiterPos + 3);
                        csvFile << acc << "," << pwd << "\n";
                    }
                }
            }
            csvFile.close();
            dbFile.close();

            if (hasData) {
                cout << "[+] Data successfully exported to '" << csvFileName << "'!\n";
            }
            else {
                cout << "[-] Vault is empty.\n";
                remove(csvFileName.c_str());
            }
        }
        else {
            cout << "[-] Error opening files.\n";
        }

        goBackPrompt();
    }
};

// ==========================================
// Class to manage the profile system
// ==========================================
class ProfileManager {
private:
    bool isValidPin(string pin) {
        if (pin.length() != 4) return false;
        for (char c : pin) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

public:
    void createProfile() {
        clearScreen();
        string username, pin;
        cout << "\n--- Create New Profile ---\n";
        cout << "Enter profile name [0 to cancel]: ";
        cin >> ws;
        getline(cin, username);

        if (username == "0") return;

        while (true) {
            cout << "Enter a 4-digit PIN [0 to cancel]: ";
            pin = getHiddenInput();
            if (pin == "0") return;
            if (isValidPin(pin)) break;
            cout << "[-] Invalid PIN! Must be 4 digits.\n";
        }

        ofstream file(DB_FILE, ios::app);
        if (file.is_open()) {
            string encryptedPin = encryptData(pin, SYSTEM_KEY);
            file << "U|" << username << "|" << encryptedPin << endl;
            file.close();
            cout << "[+] Profile created successfully!\n";
        }
        goBackPrompt();
    }

    bool login(string& loggedInUser, string& loggedInPin) {
        clearScreen();
        ifstream file(DB_FILE);
        string line;
        vector<pair<string, string>> users;

        if (file.is_open()) {
            while (getline(file, line)) {
                if (line.find("U|") == 0) {
                    string userData = line.substr(2);
                    size_t delimPos = userData.find('|');
                    if (delimPos != string::npos) {
                        string fileUser = userData.substr(0, delimPos);
                        string fileEncryptedPin = userData.substr(delimPos + 1);
                        users.push_back(make_pair(fileUser, fileEncryptedPin));
                    }
                }
            }
            file.close();
        }

        if (users.empty()) {
            cout << "[-] No profiles found!\n";
            goBackPrompt();
            return false;
        }

        cout << "\n--- Select a Profile ---\n";
        for (size_t i = 0; i < users.size(); i++) {
            cout << i + 1 << ". " << users[i].first << "\n";
        }
        cout << "0. Cancel\nChoose option: ";

        string choiceStr;
        cin >> ws;
        getline(cin, choiceStr);

        if (choiceStr == "0") return false;

        int choice = atoi(choiceStr.c_str());
        if (choice < 1 || choice >(int)users.size()) {
            cout << "[-] Invalid choice!\n";
            goBackPrompt();
            return false;
        }

        string selectedUser = users[choice - 1].first;
        string expectedEncryptedPin = users[choice - 1].second;

        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter PIN for " << selectedUser << " (Attempts left: " << attempts << ") [0 to cancel]: ";
            string pin = getHiddenInput();

            if (pin == "0") return false;

            if (decryptData(expectedEncryptedPin, SYSTEM_KEY) == pin) {
                loggedInUser = selectedUser;
                loggedInPin = pin;
                clearScreen();
                return true;
            }

            attempts--;
            if (attempts > 0) cout << "[-] Incorrect PIN!\n";
        }

        cout << "\n[!] Account locked due to failed attempts.\n";
        goBackPrompt();
        return false;
    }

    bool deleteProfile(string username) {
        clearScreen();
        string confirmation;
        cout << "\n[WARNING] Delete profile '" << username << "' and all data?\nType 'YES' or '0' to cancel: ";
        cin >> ws;
        getline(cin, confirmation);

        if (confirmation != "YES") return false;

        ifstream file(DB_FILE);
        string tempFileName = "temp_" + DB_FILE;
        ofstream tempFile(tempFileName);
        string line;
        bool found = false;

        string userPrefix = "U|" + username + "|";
        string dataPrefix = "P|" + username + "|";

        if (file.is_open()) {
            while (getline(file, line)) {
                if (line.find(userPrefix) == 0 || line.find(dataPrefix) == 0) {
                    found = true;
                    continue;
                }
                tempFile << line << endl;
            }
            file.close();
            tempFile.close();

            if (found) {
                remove(DB_FILE.c_str());
                rename(tempFileName.c_str(), DB_FILE.c_str());
                cout << "[+] Profile deleted successfully!\n";
                goBackPrompt();
                return true;
            }
        }
        remove(tempFileName.c_str());
        return false;
    }
};

// ==========================================
// Main Function
// ==========================================
int main() {
    srand((unsigned)time(0));

    ProfileManager pm;
    string currentUser = "", currentPin = "";
    string initialChoice;

    clearScreen();
    while (true) {
        cout << "=====================================\n";
        cout << "       Welcome to Shadow Key         \n";
        cout << "=====================================\n";
        cout << "1. Login to an existing profile\n";
        cout << "2. Create a new profile\n";
        cout << "3. About the Developer\n";
        cout << "4. Exit system\n";
        cout << "Choose an option: ";
        cin >> ws;
        getline(cin, initialChoice);

        if (initialChoice == "1") {
            if (pm.login(currentUser, currentPin)) {
                string vaultChoice;
                PasswordManager vault(currentUser, currentPin);

                do {
                    clearScreen();
                    cout << "\n--- Vault Menu (" << currentUser << ") ---\n";
                    cout << "1. Add a new password\n";
                    cout << "2. View all passwords\n";
                    cout << "3. Search for a password\n";
                    cout << "4. Update or Delete an account\n";
                    cout << "5. Export Data to CSV\n";
                    cout << "6. Delete this Profile\n";
                    cout << "7. Logout\n";
                    cout << "Choose an option: ";
                    cin >> ws;
                    getline(cin, vaultChoice);

                    if (vaultChoice == "1") vault.savePassword();
                    else if (vaultChoice == "2") vault.viewPasswords();
                    else if (vaultChoice == "3") vault.searchPassword();
                    else if (vaultChoice == "4") vault.updateOrDeletePassword();
                    else if (vaultChoice == "5") vault.exportData();
                    else if (vaultChoice == "6") {
                        if (pm.deleteProfile(currentUser)) {
                            vaultChoice = "7";
                        }
                    }

                } while (vaultChoice != "7");

                clearScreen();
            }
        }
        else if (initialChoice == "2") {
            pm.createProfile();
            clearScreen();
        }
        else if (initialChoice == "3") {
            showDeveloperInfo();
            clearScreen();
        }
        else if (initialChoice == "4") {
            cout << "System closed safely.\n";
            break;
        }
        else {
            cout << "Invalid choice! Press Enter to continue...";
            _getch();
            clearScreen();
        }
    }

    return 0;
}