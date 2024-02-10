// To whoms is marking this. 
// Please take the time to read the README.md file in the root of the project as it provides context and development plans of this project.

#include <string>
#include <map>
using std::string; using std::map;

struct AccountStruct { // struct for account details
    string accountUUID;
    string accountName;
    string accountPassword;
};

class passwordManager { // class for password manager
    private:
        string username;
        string masterPassword;

        map<string, map<string, AccountStruct>> userAccountMap; // map for user accounts
        map<string, string> usedIds; // map for used ids
    public:
        //! user login
        void get_username();
        void get_masterPassword();

        void user_login();

        //! user password functions
        void view_userPasswords();
        void add_userPassword();
        void edit_userPassword();
        void remove_userPassword();
        void generate_userPassword();
        void logout_user(bool force = false);

        //! private variable functions
        string fetch_username(); // function to fetch username not yet implemented
        string fetch_masterPassword(); // function to fetch master password not yet implemented
        
        void save_MapToFile();
        void load_MapFromFile();
        void clear_Map();

};

class createUsers {
    private:
        string username; 
        string masterPassword;
    public:
        void set_userName();
        void set_PassWord(); 

        void save_users(); // username, masterPassword
};

class sys {
    public:
        int menu1();
        int menu2();
        int m2opt1Menu();
        
        string encryption(string rawPassword);
        string decryption(string cryptPassword);

        string generateRandomId();

        void createdatabase();
        void clearScreen();
};

// this file was left largely uncommented as it doesnt contain any true code, only function prototypes and class definitions which are self explanatory.