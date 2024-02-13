//! I use coloured comments any comments containing ! ? * are for my note taking and place keeping i have removed many of them for the final submission
//! But some may have been missed.

#include "headers/PassManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

int main()
{
    // declare objects
    sys general; // object for general system functions
    createUsers newUser; // object for create user
    passwordManager User; // object for password manager

    int menuChoice1, menuChoice2; // variables for menu choices
    string username, masterPassword, password, encryptedPassword; // variables for user input

    general.createdatabase(); // create database if it doesn't exist, if it does exist do nothing
    //! start program
    menuChoice1 = general.menu1(); // first menu for login or create user
    
    switch (menuChoice1)
    {
        case 1: // create user
        {
            newUser.set_userName(); // set username
            newUser.set_PassWord(); // set password
            newUser.set_email(); // set email
            newUser.set_securityQuestion(); // set security question

            newUser.save_users(); // temp save username and password to file
            break;
        }
        case 2: // login
        {
            User.user_login(); // login
            
            while (true)
            {
                menuChoice2 = general.menu2(); // second menu for password manager
                switch (menuChoice2)
                {
                    case 1: // view passwords
                    {
                        User.view_userPasswords();
                        break;
                    }
                    case 2: // add password
                    {
                        User.add_userPassword();
                        break;
                    }
                    case 3: // edit password
                    {
                        User.edit_userPassword();
                        break;
                    }
                    case 4: // generate password
                    {
                        User.generate_userPassword();
                        break;
                    }
                    case 5: // remove password
                    {
                        User.remove_userPassword();
                        break;
                    }
                    case 6: // logout
                    {
                        User.logout_user();
                        break; // unsure if this is needed but its nice for uniformity
                    }
                    default:
                    {
                        cout << "default" << endl;
                        break;
                    }
                }   
            }
            // do while loop for menu2 take valid as while condition and print menu2 take menuChoice2 as input and switch case for menuChoice2 
            break;
        }
        default: // default if nothing else
        {
            break;
        }
    }
    return 0;
}