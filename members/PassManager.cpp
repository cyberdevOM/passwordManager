// To whoms is marking this. 
// Please take the time to read the README.md file in the root of the project as it provides context and development plans of this project.

#include <iostream> // cout, cin
#include <string> // string
#include <fstream> // file io
#include <sstream> // stringstream
#include <algorithm> // remove_if (unsure if this is needed but i have left it in incase)
#include <random> // random for generating random ids and passwords
#include <map> // map for userAccountMap and usedIds
#include <typeinfo> // for debugging
#include "../nlohmann/json.hpp" // json for nlohmann library
#include "../headers/PassManager.h" // header file for password manager

using json = nlohmann::json; // json for nlohmann library
using std::map; // map for userAccountMap and usedIds
using namespace std; // std namespace for all other functions

//! create user functions
void createUsers::set_userName() // set username function [working]
{
    sys general;
    string uName;
    int attempts = 0;
    bool valid = false;

    do { // do while loop for username input
        cout << "Please enter a username for your account: ";
        getline(cin, uName);
        
        //? Note 3: uName.erase(remove_if(uName.begin(), uName.end(), ::isspace), uName.end());
        // this was my first mistake as removing the spaces from the user input would result in a username that is not the same as the one the user entered

        if (uName.empty())
        {
            cout << "bye!" << endl;
            exit(0);
        } // if end of file [ctrl + Z] or emptys
        else if (uName.length() > 126 || uName.length() < 1 || uName.find_first_of(" ") != string::npos) // if length < 126 or > 1 or contains a space
        {
            cout << "the username you have entered does not meet the requirements." << endl;
            attempts++; // increment attempts
            continue;
        }

        ifstream file("../task2/db/db.json"); // open file
        if (file.is_open()) {
            json data;
            file >> data; // read json data from file

            if (data.contains(uName)) {
                cout << "That username is already taken" << endl;
                attempts++;
                continue;
            } else {
                valid = true;
            }
            file.close();
        } else {
            cerr << "Error: Unable to open the file for reading." << endl;
            exit(0);
        }

        if (attempts == 5)
        {
            cout << "you have had 5 attempts, please come back when you can read" << endl;
            exit(0);
        } // if user tries 5 times break loop
    } while (!valid);

    this->username = uName; // set username class variable
} // set username [working]

void createUsers::set_PassWord() // set password function [working]
{
    sys general;
    string mPassword;
    int attempts = 0;
    bool valid = true;

    do { // do while loop for password input
        // password requirements
        cout << endl;
        cout << "Your password must satisfy the following conditions:" << endl;
        cout << "----------------------------------------------------" << endl;
        cout << "1. Your password must be at least 8 characters long" << endl;
        cout << "2. Your password must contain at least one number" << endl;
        cout << "3. Your password must contain at least one uppercase letter" << endl;
        cout << "4. Your password must contain at least one special character" << endl;
        cout << "5. Your password must not contain any spaces" << endl << endl;


        cout << "Please enter a password: ";
        getline(cin, mPassword);

        // mPassword.erase(remove_if(mPassword.begin(), mPassword.end(), ::isspace), mPassword.end());
        if (mPassword.empty())
        {
            cout << "bye!" << endl; 
            exit(0); 
        } // if end of file [ctrl + Z] or emptys
        int passLen = mPassword.length();
        if(passLen >= 8 && mPassword.find_first_of("0123456789") != string::npos && mPassword.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != string::npos && mPassword.find_first_of("!?.@#_-&$") != string::npos && mPassword.find_first_of(" ") == string::npos)
        {
            valid = true;
        }// one big if condition to verify if all password conditions are met. 
        else 
        {
            cout << "That password does not satisfy our conditions";
            attempts++; // increment attempts
            valid = false; // set valid to false
        }

        if (attempts == 5)
        {
            cout << "you have had 5 attempts, please come back when you can read" << endl;
            exit(0); // exit program if user tries 5 times
        }
    } while (!valid);

    string encryptedMasterPassword = general.encryption(mPassword); // encrypt password for storage in json data

    this->masterPassword = encryptedMasterPassword; // set master password class variable
}

void createUsers::save_users() // save user data to file [working]
{
    // Open the JSON file
    ifstream inFile("../task2/db/db.json");
    nlohmann::json data;

    // If the file exists, load its content into the 'data' object
    if (inFile.good()) {
        inFile >> data;
    }
    inFile.close();

    // Add the new user data
    data[this->username] = {
        {"username", this->username},
        {"mPassword", this->masterPassword},
        {"passwords", nlohmann::json::object()}
    };

    // Write the updated data back to the file
    ofstream outFile("../task2/db/db.json");
    outFile << data.dump(4); // '4' is for pretty-printing
    outFile.close();
} // save user data to file [working]

//! logged in user functions
void passwordManager::save_MapToFile() // save user data from map to file essential to the function of the program [working]
{
    // alot of the data is stored using auto data types, this is a requirement of the nlohmann json library 

    json data, passwordEntry;
    ifstream inFile("../task2/db/db.json");  // open file
    inFile >> data; // read json data from file
    inFile.close();

    auto userAccount = data.find(this->username);  // find user in json data
    
    if (userAccount != data.end()) {
        userAccount.value()["passwords"].clear(); 
    } // clear userAccount passwords this is to enable fresh data to be wrote to the file so i can delete passwords and add new ones without having to worry about duplicates

    for (auto& servicePair : this->userAccountMap) { // itterate through userAccountMap
        auto& service = servicePair.first; 
        auto& accounts = servicePair.second;
        for (auto& accountPair : accounts) {
            auto& username = accountPair.first;
            auto& account = accountPair.second;

            passwordEntry["id"] = account.accountUUID;
            passwordEntry["username"] = account.accountName;
            passwordEntry["password"] = account.accountPassword;

            userAccount.value()["passwords"][service][username] = move(passwordEntry); // add password to json data, move is used to avoid copying the data in memory
        }
    }
    
    ofstream outFile("../task2/db/db.json");
    outFile << setw(4) << data << endl;
    outFile.close();

    // the point behind the usedids map and json file is to allow for easy tracking of used ids across all user accounts allowing for totally unique ids for each account
    // in the future id like to impliment this to only check against the users account to allow for more passwords to be stored across all accounts but for now
    // this is the best solution i could come up with in the time i had and 273342452889765 possible ids is more than enough for the time being.

    ifstream file("../task2/db/usedids.json");
    if (!file.is_open()) {
        cerr << "File could not open." << endl;
    }
    json usedIds; // create json object
    file >> usedIds;
    file.close();

    usedIds = this->usedIds; // set usedIds to usedIds class variable map

    ofstream outFile2("../task2/db/usedids.json");
    outFile2 << usedIds.dump(4); // '4' is for pretty-printing
    outFile2.close(); // close file
} // save user data from map to file essential to the function of the program

void passwordManager::load_MapFromFile() // load user data from file to map essential to the function of the program [working]
{
    sys general;
    ifstream file("../task2/db/db.json");
    if (!file.is_open()) {
        cerr << "File could not open." << endl;
    }
    json data;
    file >> data;
    file.close();
    
    // i have explained this function fully to help future developers understand the logic behind it.

    for (auto& user : data.items()) { // itterate through json data
        if (user.key() == this->username) { // if username is found in json data
            for (auto& service : user.value()["passwords"].items()) { // itterate through json data["passwords"]
                for (auto& account : service.value().items()) { // itterate through json data["passwords"][service]
                    AccountStruct acc; // create account struct
                    acc.accountUUID = account.value()["id"]; // set accountUUID 
                    acc.accountName = account.value()["username"]; // set accountName
                    acc.accountPassword = account.value()["password"]; // set accountPassword

                    this->userAccountMap[service.key()][account.key()] = acc; // add account to userAccountMap
                }
            }
            break;
        }
    }

    for (auto& service : this->userAccountMap) {
        cout << "accounts for " << service.first << endl;
        cout << "-------------------------\n";
        for (auto& account : service.second) {
            cout << "   id: " << account.second.accountUUID << endl;
            cout << "   username: " << account.second.accountName << endl;
            cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
            cout << "   -------------------\n";
        }
    }

    ifstream file2("../task2/db/usedids.json");
    if (!file2.is_open()) {
        cerr << "File could not open." << endl;
    }
    json usedIds;
    file2 >> usedIds;
    file2.close();

    this->usedIds = usedIds;
} // load user data from file to map essential to the function of the program

void passwordManager::clear_Map() // clear both maps mostly house keeping and to be used in logout_user function [working]
{
    this->userAccountMap.clear();
    this->usedIds.clear();
} // clear both maps mostly house keeping and to be used in logout_user function

void passwordManager::get_username() // get username function called in user_login [working]
{
    string input;
    cout << "Please enter your username: ";
    getline(cin, input);
    
    //? Note 3: username.erase(remove_if(username.begin(), username.end(), ::isspace), username.end()); // same as in get master password function
    if (input.empty())
    {
        cout << "bye!" << endl;
        exit(0);
    } else if (input.length() > 126 || input.length() < 1 || input.find_first_of(" ") != string::npos)
    {
        cout << "the username you have entered does not meet the requirements." << endl;
        exit(0);
    }
    username = input; // set username class variable
} // get username
void passwordManager::get_masterPassword() // get master password function called in user_login [working]
{
    string input;
    cout << "Please enter your password: ";
    getline(cin, input);

    //? Note 3: password.erase(remove_if(password.begin(), password.end(), ::isspace), password.end());
    // the function above was used to remove spaces from the password, this was removed as the password requirements state that the password cannot contain spaces.

    if (input.empty())
    {
        cout << "bye!" << endl;
        exit(0);
    } else if (input.length() < 8 || input.find_first_of(" ") != string::npos)
    {
        cout << "the password you have entered does not meet the requirements." << endl;
        exit(0);
    }

    masterPassword = input; // set password class variable
    // return password; // old return statement from when this function returned a string and most logic was in main.cpp
} // get password

void passwordManager::user_login() // user login function [working]
{
    int attempts = 0;
    bool valid = false;
    sys general;
    //passwordManager pM; // object for password manager for logout_user, get_username() & other functions that i later learned can be done with 'this->' keyword;

    while (!valid) { // while loop for user login
        
        this->get_username(); // get username function call
        this->get_masterPassword(); // get master password function call

        ifstream file("../task2/db/db.json");
        if (file.is_open()) {
            json data;
            file >> data;

            if (data.contains(this->username)) { // if username is found in json data
                string storedUsername = data[this->username]["username"]; // get stored username from json data
                string storedPassword = data[this->username]["mPassword"]; // get stored password from json data
                string decryptedStoredPassword = general.decryption(storedPassword); // decrypt stored password
                // cout << "decrypted stored password: " << decryptedStoredPassword << endl;

                if (storedUsername == this->username && decryptedStoredPassword == this->masterPassword) { // if username and password match
                    valid = true; // set valid to true
                    cout << "You have been logged in!" << endl;
                    // user is logged in here so add all user data to map
                    // itterate through json data and add passwords to map. no need to add username and master password to map as they are already class variables.
                    this->load_MapFromFile(); // load user data from file to map
                } else {
                    cout << "Invalid username or password!" << endl;
                    attempts++;
                    continue; // continue to next iteration of while loop if invalid username or password
                }
            } else {
                cout << "User not found!" << endl;
                attempts++;
                continue; // continue to next iteration of while loop if user not found
            }
            file.close();

            cout << "Press enter to continue..." << endl;
            cin.ignore(); // hold screen until user presses enter
        } else {
            cerr << "Error: Unable to open the file for reading." << endl;
            this->clear_Map(); // clear maps
            exit(0);
        }

        if (attempts == 3) {
            cout << "Too many attempts, exiting program" << endl;
            this->clear_Map(); // clear maps
            exit(0);
        }
    }
} // user login [working]

void passwordManager::add_userPassword() // add password function [working]
{
    sys general;
    string comparison_string, service, id, servicePassword, serviceUsername; // variables for password entry
    int attempts = 0; // variable for while loop

    cout << "Please enter the service name: ";
    getline(cin, service);
    while (service.empty()) {
        attempts++;
        if (attempts == 5) {
            this->logout_user(true); // logout user and save current map data to file
        }
        cout << "Service cannot be empty. Please enter the service name: "; // prompt user for service name
        getline(cin, service);
    } // sanity check for empty service name
    

    cout << "Please enter the username: "; // prompt user for username
    getline(cin, serviceUsername);
    while (serviceUsername.empty()) {
        attempts++;
        if (attempts == 5) {
            this->logout_user(true); // logout user and save current map data to file
        }
        cout << "Username cannot be empty. Please enter the username: ";
        getline(cin, serviceUsername);
    } // sanity check for empty username

    /* change note:
        we dont need to do a full check of the password as this a password the user has created for a service, 
        not the master password therefor we can assume the user has followed the password requirements of the respective service
    */
    cout << "Please enter the password: ";
    getline(cin, servicePassword);
    while (servicePassword.empty()) {
        attempts++;
        if (attempts == 5) {
            this->logout_user(true);
        }
        cout << "Password cannot be empty. Please enter the password: ";
        getline(cin, servicePassword);
    } // sanity check for empty password
    if (servicePassword.find_first_of("£") != string::npos) {
        cout << "Password cannot contain the '£' character." << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore();
        return;
    } // check for invalid character in password
    servicePassword = general.encryption(servicePassword); // encrypt password for storage in json data

    // Open the JSON file

    // i would prefer to use the map data to check for the user account but im out of time and will fix this after the deadline
    ifstream file("../task2/db/db.json");
    if (!file.is_open()) {
        this->logout_user(true);
    }
    json data;
    file >> data;
    file.close();

    // Find the user in the JSON data
    id = general.generateRandomId(); // generate random id
    auto account = data.find(this->username); // find user in json data

    if (account != data.end()) { // if account is found in json data
        // create a new password entry
        AccountStruct acc;

        acc.accountUUID = id;
        acc.accountName = serviceUsername;
        acc.accountPassword = servicePassword;
        
        // add the new password entry to the "userAccountMap" map
        userAccountMap[service][serviceUsername] = acc;

        // add the new password entry to the "usedIds" map
        usedIds[id] = this->username;

        cout << "Password added successfully!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore();
    } else {
        cout << "User not found!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore();
    }
} // add password [working]

void passwordManager::view_userPasswords() // view passwords function [working]
{
    if (this->userAccountMap.empty()) { // check for empty map
        cout << "No password data found!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore();
        return;
    }

    sys general;
    string service, id, servicePassword, serviceUsername; // variables for password entry
    
    int choice = general.m2opt1Menu(); // get user choice from menu
    switch (choice) // switch statement for menu choice
    {
        case 1:
        { // print all accounts for a user
            //working exluxively with map data more efficient than json data
            for (auto& service : this->userAccountMap) {
                cout << "accounts for " << service.first << endl;
                cout << "-------------------------\n";
                for (auto& account : service.second) {
                    cout << "   id: " << account.second.accountUUID << endl;
                    cout << "   username: " << account.second.accountName << endl;
                    cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
                    cout << "   -------------------\n";
                }
            }

            cout << "Press enter to continue..." << endl;
            cin.ignore(); // hold screen until user presses enter
            break;
        } // case 1 [working]
        case 2:
        { // print all accounts for a specific service
            int attempts = 0;
            cout << "Please enter the service name: ";
            getline(cin, service);
            while (service.empty() || this->userAccountMap.find(service) == this->userAccountMap.end()) { // check for empty or invalid service name
                attempts++;
                if (attempts == 5 || cin.eof()) {
                    this->logout_user(true);
                }
                cout << "Service must be a valid service. Please enter the service name: ";
                getline(cin, service);
            } // check for empty or invalid service name

            cout << "accounts for " << service << ":\n";
            cout << "-------------------------\n";
            for (auto& account : this->userAccountMap[service]) { // itterate through userAccountMap
                auto acc = this->userAccountMap[service][account.first];
                cout << "   id: " << acc.accountUUID << endl;
                cout << "   username: " << acc.accountName << endl;
                cout << "   password: " << general.decryption(acc.accountPassword) << endl;
                cout << "   -------------------\n";
            }

            cout << "Press enter to continue..." << endl;
            cin.ignore();
            break;
        } // case 2 [working]
        case 3:
        { // i dont want to talk about how long this took me... its 3am... i need sleep
            string inputId, choice;
            int attempts = 0;
            bool found = false;

            cout << "would you like to search by username or id?" << endl;
            cout << "username/id: ";
            getline(cin, choice);
            if (choice.empty()) {
                this->logout_user(true); // logout user and save current map data to file
            } else {
                transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
                if (choice != "username" && choice != "id") { // check for invalid input
                    cout << "Invalid input!" << endl;
                    cout << "Press enter to continue..." << endl;
                    cin.ignore(); // hold screen until user presses enter
                    return;
                } else if (choice == "username") { // search for a specific password using username
                    cout << "Please enter the username for the password you would like to view: ";
                    getline(cin, inputId);

                    while (inputId.empty()){
                        attempts++;
                        if (attempts == 5 || cin.eof()) {
                            this->logout_user(true); // logout user if they have tried 5 times
                        }
                        cout << "Username cannot be empty. Please enter the username for the password you would like to view: ";
                        getline(cin, inputId);
                    } // check for empty username

                    cout << "accounts with username " << inputId << ":\n"; 
                    int accountsFound = 0; // variable to check if any accounts were found
                    for (auto& service : this->userAccountMap) { // itterate through userAccountMap
                        for (auto& account : service.second) { 
                            if (account.second.accountName == inputId) {
                                accountsFound++; // increment accountsFound
                                cout << "account in " << service.first << ":\n"; // print account data
                                cout << "   id: " << account.second.accountUUID << endl;
                                cout << "   username: " << account.second.accountName << endl;
                                cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
                                cout << "   -------------------\n";
                            }
                        }
                    }
                    if (accountsFound == 0) {
                        cout << "No accounts found with that username!" << endl;
                        cout << "Press enter to continue..." << endl;
                        cin.ignore();
                    } else {
                        cout << "Press enter to continue..." << endl;
                        cin.ignore();
                    }

                } else { // search for a specific password using id
                    cout << "Please enter the id for the password you would like to view: ";
                    getline(cin, inputId);

                    while (inputId.empty()){
                        attempts++;
                        if (attempts == 5 || cin.eof()) {
                            this->logout_user(true);
                        }
                        cout << "Id cannot be empty. Please enter the id for the password you would like to view: ";
                        getline(cin, inputId);
                    } // check for empty id

                    for (auto& service : this->userAccountMap) {
                        for (auto& account : service.second) {
                            if (account.second.accountUUID == inputId) {
                                cout << "account in " << service.first << ":\n";
                                cout << "   id: " << account.second.accountUUID << endl;
                                cout << "   username: " << account.second.accountName << endl;
                                cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
                                cout << "   -------------------\n";
                                found = true;
                                break;
                            }
                        } 
                        if (found) {
                            break;
                        }
                    }
                    if (!found) {
                        cout << "No password found with that id!" << endl;
                    }

                    cout << "Press enter to continue..." << endl;
                    cin.ignore();
                    break;
                }
            } // search for a specific password using username or id [working]
            break;
        } // search for a specific password using username [working]
        default:
        {
            cout << "Invalid choice!" << endl;
            break;
        } // default case for invalid input [working]
    } // switch statement for menu choice [working]
    
} // [working]

void passwordManager::remove_userPassword() // remove password [working]
{
    sys general;
    string inputId, input;
    int attempts = 0;
    bool found = false;

    cout << "Please enter the id for the password you would like to view: "; // prompt user for id of password to remove
    getline(cin, inputId); // get user input

    while (inputId.empty()){
        attempts++;
        if (attempts == 5 || cin.eof()) {
            this->logout_user(true); // logout user and save current map data to file
        }
        cout << "Id cannot be empty. Please enter the id for the password you would like to view: ";
        getline(cin, inputId);
    } // check for empty id

    for (auto& service : this->userAccountMap) {
        for (auto& account : service.second) {
            if (account.second.accountUUID == inputId) {
                cout << "would you like to delete this user account?" << endl;
                cout << "account in " << service.first << ":\n";
                cout << "   id: " << account.second.accountUUID << endl;
                cout << "   username: " << account.second.accountName << endl;
                cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
                cout << "   -------------------\n";
                cout << "y/n: "; // prompt user for confirmation
                getline(cin, input); 
                if (input.empty()) { // check for empty input
                    this->logout_user(true); 
                } else if (input.find_first_not_of("yn") != string::npos) { // check for invalid input of non y or n characters
                    cout << "Invalid input!" << endl;
                    cout << "Press enter to continue..." << endl;
                    cin.ignore();
                    return; 
                } else {
                    transform(input.begin(), input.end(), input.begin(), ::tolower); // convert input to lowercase
                    if (input == "y") {
                        this->userAccountMap[service.first].erase(account.first); // remove account from userAccountMap
                        if (this->userAccountMap[service.first].empty()) { // check if service is empty
                            this->userAccountMap.erase(service.first); // remove service from userAccountMap
                        } 


                        // remove id from usedids.json file, move to map
                        this->usedIds.erase(account.second.accountUUID);

                        cout << "password removed successfully!" << endl; // print success message
                    } else {
                        cout << "password not removed!" << endl; // print failure message
                    }
                }
                found = true; // set found to true to exit loops
                break; // break loop
            }
        } 
        if (found) { // if found is true
            break; // break loop 
        }
    }
    if (!found) {
        cout << "No password found with that id!" << endl; // print failure message
    }
    
    cout << "Press enter to continue..." << endl;
    cin.ignore();

    // i chose for this function to only work with an id as it is the most accurate identifier for a password account.
} // remove password [working] use view_userPasswords() to find id of password to remove, 

void passwordManager::edit_userPassword() // edit password [working]
{
    sys general;
    string inputId, input;
    int attempts = 0;
    bool found = false;

    cout << "Please enter the id for the password you would like to view: ";
    getline(cin, inputId);

    while (inputId.empty()){
        attempts++;
        if (attempts == 5 || cin.eof()) {
            this->logout_user(true);
        }
        cout << "Id cannot be empty. Please enter the id for the password you would like to view: ";
        getline(cin, inputId);
    } // check for empty id

    for (auto& service : this->userAccountMap) {
        for (auto& account : service.second) {
            if (account.second.accountUUID == inputId) {
                cout << "what value would you like to edit?" << endl;
                cout << "account in " << service.first << ":\n";
                cout << "   username: " << account.second.accountName << endl;
                cout << "   password: " << general.decryption(account.second.accountPassword) << endl;
                cout << "   -------------------\n";
                cout << "username/password: "; // prompt user for input of username or password to edit
                getline(cin, input);
                if (input.empty()) {
                    logout_user(true);
                } else {
                    transform(input.begin(), input.end(), input.begin(), ::tolower); // convert input to lowercase
                    if (input == "username") {
                        cout << "Please enter the new username: ";
                        getline(cin, account.second.accountName); // prompt user for new username and save at same time.
                    } else if (input == "password") {
                        string newPassword;
                        cout << "Please enter the new password: "; // prompt user for new password
                        getline(cin, newPassword);
                        account.second.accountPassword = general.encryption(newPassword); // encrypt password for storage in json data
                    } else {
                        cout << "Invalid input!" << endl;
                        cout << "Press enter to continue..." << endl;
                        cin.ignore();
                        return;
                    }
                } // note you are unable to edit the id of a password as that could cause issues with the usedids.json file
                found = true; // set found to true to exit loops
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        cout << "No password found with that id!" << endl;
    }               
}

void passwordManager::generate_userPassword() // generate password [working]
{
    string generatedPassword;
    string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!?.@#_-&$"; // characters to be used in password
    random_device rd; 
    mt19937 gen(rd()); 
    uniform_int_distribution<> dis(0, characters.size() - 1); // random number generator

    cout << "Generating password..." << endl; 

    for (int i = 0; i < 20; ++i) { // loop to generate password
        generatedPassword += characters[dis(gen)]; // add random character to password
    } // loop to generate password

    cout << "Generated password: " << generatedPassword << endl; // print generated password

    cout << "Press enter to continue..." << endl; 
    cin.ignore(); // hold screen until user presses enter

    // id like to impliment a function to copy the generated password to the clipboard but that is out of the scope of this project.
}

void passwordManager::logout_user(bool force) // logout [working]
{
    if (force) { // if force is true print message 
        cout << "You have been logged out due to undetermined conditions." << endl;
        this->save_MapToFile(); // save user data to file
    } else { // if force is false print message
        cout << "Thank you for using the norton service. please come back again soon!" << endl;
        this->save_MapToFile(); // save user data to file
    }
    this->clear_Map(); // clear maps
    exit(0); // exit program
} // logout
//! general system functions
int sys::menu1() // main menu [working]
{
    string input;
    int choice;

    cout << "      Main Menu" << endl;
    cout << "---------------------" << endl << endl;
    cout << "1. Create Accout." << endl;
    cout << "2. Login" << endl << endl;
    cout << "Welcome to norton password manager, please login or if you are a new user create an accout\n >> ";
    getline(cin, input);

    if (input.empty()){
        cout << "bye!" << endl;
        exit(0);
    } else if (input.find_first_not_of("12") != string::npos) {
        cout << "The input given contained more than this program can handle!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore(); // hold screen until user presses enter

        return 0;
    } else {
        stringstream(input) >> choice; // convert input to int
        return choice; // return choice
    } // check for invalid input and return choice

    return 0; // return 0 if input is invalid

    // basic crude main menu function it works and is easy to understand and use.
}

int sys::menu2() // secondary menu [working]
{
    passwordManager pm; // object for password manager for logout_user function

    string input; // input string
    int choice; // input int
    
    this->clearScreen(); // clear screen (cross platform because its 5am and yes) 
    // i enjoy reading these comments after the fact, beacuse it was 5am at the time of writing and i had really put my all into it that day.

    cout << "    welcome back"<< endl;
    cout << "--------------------" << endl << endl;
    cout << "1. list Passwords." << endl;
    cout << "2. Add Password." << endl;
    cout << "3. Edit Password." << endl;
    cout << "4. Generate Password." << endl;
    cout << "5. Remove Password." << endl;
    cout << "6. Logout." << endl << endl;
    cout << "Please select an option from the menu above\n >> ";

    getline(cin, input); // get input from user
    if(input.empty()) { // if input is empty
        pm.logout_user(true);
    } else if (input.find_first_not_of("0123456789") != string::npos) {
        cout << "The input given contained more than this program can handle!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore(); // hold screen until user presses enter

        return 0; // if input contains anything other than numbers
    } else { // if input is valid
        stringstream(input) >> choice; // convert input to int
        return choice; // return choice
    }

    return choice; // return choice
} // secondary menu [working] 

int sys::m2opt1Menu() // menu for view_userPasswords function [working]
{
    passwordManager pm; // object for password manager for logout_user function

    string input; // input string
    int choice; // input int
    
    this->clearScreen(); // clear screen for ease of use 

    cout << "   List passwords"<< endl;
    cout << "--------------------" << endl << endl;
    cout << "1. show all passwords." << endl;
    cout << "2. show all data for a service." << endl;
    cout << "3. show a specific password." << endl;
    cout << "Please select an option from the menu above\n >> ";

    getline(cin, input); // get input from user
    if(input.empty()) { // if input is empty
        pm.logout_user(true);
    } else if (input.find_first_not_of("0123456789") != string::npos) {
        cout << "The input given contained more than this program can handle!" << endl;
        cout << "Press enter to continue..." << endl;
        cin.ignore(); // hold screen until user presses enter

        return 0; // if input contains anything other than numbers
    } else { // if input is valid
        stringstream(input) >> choice; // convert input to int
        return choice; // return choice
    }

    return choice;

    // it was important for these menues to take in a string and output an int for the switch statement in main.cpp and for the error checking.
}

string sys::encryption(string msg) // encryption function [working]
{
    string key = "verysecurekey"; // key for encryption
    string encryptedMsg; // encrypted message

    // Convert key to the same length as the input string
    while (key.size() < msg.size()) { 
        key += key;
    }
    key = key.substr(0, msg.size()); 

    // Encryption loop
    for (int i = 0; i < msg.size(); ++i) {
        if (isalpha(msg[i])) {
            char encryptedChar = ((toupper(msg[i]) - 'A' + (toupper(key[i]) - 'A')) % 26) + 'A';
            if (islower(msg[i])) {
                encryptedMsg += tolower(encryptedChar);
            } else {
                encryptedMsg += encryptedChar;
            }
        } else {
            encryptedMsg += msg[i];
        }
    }
    return encryptedMsg; // return encrypted message

    // i am very proud of this function, both encryption and decryption were a labour of love and took many days to get right.
}

string sys::decryption(string msg) // decryption function [working]
{
    string key = "verysecurekey";
    string decryptedMsg;

    // Convert key to the same length as the input string
    while (key.size() < msg.size()) {
        key += key;
    } // while key is less than msg size, double key length
    key = key.substr(0, msg.size()); // substr takes the start index 0 and the length of msg to create a substring of key that is the same length as msg

    // Decryption loop
    for (int i = 0; i < msg.size(); ++i) {
        if (isalpha(msg[i])) { // if msg[i] is a letter
            char decryptedChar = ((toupper(msg[i]) - toupper(key[i]) + 26) % 26) + 'A'; // decrypt msg[i]
            if (islower(msg[i])) { // if msg[i] is lowercase, this ensures the correct casing is used in the final string
                decryptedMsg += tolower(decryptedChar); // add decryptedChar to decryptedMsg as lowercase
            } else { // else msg[i] is uppercase so add decryptedChar to decryptedMsg as uppercase
                decryptedMsg += decryptedChar;
            }
        } else { // else msg[i] is not a letter so add account to decryptedMsg, can not be encrypted. 
            decryptedMsg += msg[i]; // current flaw in code as this is the only part of the code that does not encrypt, id like to change this in the future using the crypt++ library
        }
    }

    return decryptedMsg;
}

string sys::generateRandomId() // generate random id function [working]
{
    passwordManager pm; // object for password manager for logout_user function

    ifstream file("../task2/db/usedids.json"); // open usedids.json file
    if (!file.is_open()) {
        pm.logout_user(true); // logout user and save current map data to file
    }
    json data;
    file >> data;
    file.close();

    // same standard setup as in passwordManager::generate_userPassword() function
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9);
    uniform_int_distribution<> dis2(0, 25);

    string id;
    while (id.empty()) { // while id is empty
        for (int i = 0; i < 4; ++i) {
            string section = "Aa1"; // section is a string of 3 characters upper case, lower case, and numbers
            section += '0' + dis(gen); // add random number to section
            section[0] += dis2(gen); // add random number to section
            section[1] += dis2(gen); // add random number to section
            shuffle(section.begin(), section.end(), gen); // shuffle section
            id += section; // add section to id
            if (i < 3) id += '-'; // add '-' to id to separate sections
        }

        string element; // element is a string
        bool valid = false; 
        for (auto& element : data.items()) { // itterate through data
            auto storedId = element.key(); // storedId is the key of the current element
            auto storedUser = element.value(); // storedUser is the value of the current element
            if (storedId == id) { // if storedId is equal to id
                id.clear(); // clear id
                break; // break loop
            } 
        } // ensures that all ids are unique
    } // this will loop until a unique id is generated

    return id;
}

void sys::createdatabase() // create database function [working]
{
    if (ifstream("../task2/db/db.json")) {
        return;
    } else {
        ofstream file("../task2/db/db.json");
        file << "{}";
        file.close();
    } // create db.json if it does not exist run once at start of program otherwise everything breaks.

    if (ifstream("../task2/db/usedids.json")) {
        return;
    } else {
        ofstream file("../task2/db/usedids.json");
        file << "{}";
        file.close();
    } // create usedids.json if it does not exist run once at start of program otherwise everything breaks.
}

void sys::clearScreen() // clear screen function [working]
{
    #ifdef _WIN32 // Check if the operating system is Windows
        system("cls"); // Use the "cls" command to clear the console
    #else // For other operating systems (e.g., Linux, macOS)
        system("clear"); // Use the "clear" command to clear the console
    #endif

    // this function took some learning to get working cross platform but it was worth it in the end.
    // some of this function was taken from stack overflow and other sources, i was sure to understand the code before using it however.
} 
