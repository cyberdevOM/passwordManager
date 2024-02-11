#include <string>
#include <map>
using std::string; using std::map;

class passwordRecovery 
{
    private:
        string username; // username for user
        string masterPassword; // master password for user
        string email; // email for OTP [future implementation]

        map<string, map<string,string>> recoveryCodes; // map for recovery codes "codes {code : used},"
        map<string, map<string,string>> securityQuestions; // map for security questions "questions {question : answer},"
    public:
        void get_username();
        void get_email();

        void get_recoveryCodes(); // get recovery codes from file, save to map
        
        void get_securityQuestions(); // get security questions from file, save to map

        void reset_password();

        void make_OTP(); // make OTP for email [future implementation]
        void send_OTP(); // send OTP to email [future implementation]
};

struct AccountStruct { // struct for account details
    string accountUUID;
    string accountName;
    string accountPassword;
};

class passwordManager { // class for password manager
    private:
        string username;
        string masterPassword;
        string email;
        bool verified;

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
        string email;

        map<string, string> securityQuestions; // map for security questions "questions {question : answer},"
        map<string, string> recoveryCodes; // map for recovery codes "codes {code : used},"
    public:
        //! user creation functions
        void set_userName();
        void set_PassWord();
        void set_email();

        //! user recovery functions
        void set_securityQuestions();
        void generate_recoveryCodes();

        void save_users(); // username, masterPassword

        //! private variable functions
        string fetch_username(); // function to fetch username not yet implemented
        string fetch_masterPassword(); // function to fetch master password not yet implemented
        string fetch_email(); // function to fetch email not yet implemented
        string fetch_securityQuestions(); // function to fetch security questions not yet implemented
        string fetch_recoveryCodes(); // function to fetch recovery codes not yet implemented

        void clear_Maps();
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