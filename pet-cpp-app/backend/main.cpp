// =====================================================
//  PET REGISTRATION SYSTEM - C++ WEB BACKEND
//  Uses: cpp-httplib (httplib.h) + MySQL Connector/C++
//  Frontend (HTML / CSS / JS) is in ../frontend
// =====================================================

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <memory>
#include <cstdlib>

#include "httplib.h"

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/datatype.h>

using namespace std;


// =====================================================
// ABSTRACT CLASS - Animal
// =====================================================

class Animal
{
public:

    virtual string getSpecies() = 0;

    virtual void makeSound() = 0;

    virtual ~Animal() {}
};


// =====================================================
// INHERITANCE - Dog
// =====================================================

class Dog : public Animal
{
public:

    string getSpecies() override
    {
        return "Dog";
    }

    void makeSound() override
    {
        cout << "Dog makes sound." << endl;
    }
};


// =====================================================
// INHERITANCE - Cat
// =====================================================

class Cat : public Animal
{
public:

    string getSpecies() override
    {
        return "Cat";
    }

    void makeSound() override
    {
        cout << "Cat makes sound." << endl;
    }
};


// =====================================================
// ENCAPSULATION - Pet Class
// =====================================================

class Pet
{
private:

    int petId;
    string petName;
    string species;
    string breed;

public:

    // Default Constructor
    Pet()
    {
        petId = 0;
        petName = "Unknown";
        species = "Unknown";
        breed = "Unknown";
    }

    // Parameterized Constructor
    Pet(int id, string name, string sp, string br)
    {
        petId = id;
        petName = name;
        species = sp;
        breed = br;
    }

    // Setters
    void setPetId(int id)        { petId = id; }
    void setPetName(string name) { petName = name; }
    void setSpecies(string sp)   { species = sp; }
    void setBreed(string br)     { breed = br; }

    // Getters
    int getPetId()       { return petId; }
    string getPetName()  { return petName; }
    string getSpecies()  { return species; }
    string getBreed()    { return breed; }

    // Display
    void displayPet()
    {
        cout << "Pet ID     : " << petId << endl;
        cout << "Pet Name   : " << petName << endl;
        cout << "Species    : " << species << endl;
        cout << "Breed      : " << breed << endl;
    }
};


// =====================================================
// GLOBALS
// =====================================================

sql::Connection* con = nullptr;     // one shared MySQL connection
mutex dbMutex;                      // web server is multi-threaded, so lock DB access
string frontendDir = "../frontend"; // where index.html / css / js live


// =====================================================
// SMALL HELPERS
// =====================================================

string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");

    if (start == string::npos)
    {
        return "";
    }

    size_t end = s.find_last_not_of(" \t\r\n");

    return s.substr(start, end - start + 1);
}


// Make text safe to place inside HTML
string htmlEscape(const string& s)
{
    string out;

    for (char c : s)
    {
        switch (c)
        {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            case '{':  out += "&#123;"; break;  // so user text can never look like a {{PLACEHOLDER}}
            default:   out += c;        break;
        }
    }

    return out;
}


// Escaped text, or "-" when empty
string show(const string& s)
{
    return s.empty() ? "-" : htmlEscape(s);
}


string urlEncode(const string& s)
{
    static const char* hex = "0123456789ABCDEF";
    string out;

    for (unsigned char c : s)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            out += (char)c;
        }
        else
        {
            out += '%';
            out += hex[c >> 4];
            out += hex[c & 15];
        }
    }

    return out;
}


string readFile(const string& path)
{
    ifstream file(path, ios::in | ios::binary);

    if (!file.good())
    {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}


void replaceAll(string& text, const string& from, const string& to)
{
    size_t pos = 0;

    while ((pos = text.find(from, pos)) != string::npos)
    {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }
}


// Read a form field and trim it
string param(const httplib::Request& req, const string& name)
{
    return trim(req.get_param_value(name));
}


// Empty text becomes NULL in the database (needed for DATE columns)
void setOptional(sql::PreparedStatement* stmt, int index, const string& value)
{
    if (value.empty())
    {
        stmt->setNull(index, sql::DataType::VARCHAR);
    }
    else
    {
        stmt->setString(index, value);
    }
}


unique_ptr<sql::PreparedStatement> prepare(const string& query)
{
    return unique_ptr<sql::PreparedStatement>(con->prepareStatement(query));
}


// After a POST, go back to the page with a message (success or error)
void redirectTo(httplib::Response& res, const string& tab, const string& text, bool isError)
{
    string url = "/?tab=" + tab + "&" + (isError ? "err=" : "msg=") + urlEncode(text);

    res.set_redirect(url, 303);
}


// Runs a handler with the DB locked, and turns database errors into a message on the page
template <typename Function>
void guarded(httplib::Response& res, const string& tab, Function handler)
{
    lock_guard<mutex> lock(dbMutex);

    try
    {
        handler();
    }
    catch (sql::SQLException& e)
    {
        cerr << "MySQL Error: " << e.what()
             << " (code " << e.getErrorCode() << ")" << endl;

        redirectTo(res, tab, string("Database error: ") + e.what(), true);
    }
    catch (exception& e)
    {
        cerr << "Error: " << e.what() << endl;

        redirectTo(res, tab, string("Error: ") + e.what(), true);
    }
}


// =====================================================
// BUILD THE PAGE (fills {{PLACEHOLDERS}} in index.html)
// =====================================================

string renderPage(const httplib::Request& req)
{
    string page = readFile(frontendDir + "/index.html");

    if (page.empty())
    {
        return "<h1>index.html not found in: " + htmlEscape(frontendDir) +
               "</h1><p>Run the server from the backend folder.</p>";
    }

    string tab = req.has_param("tab") ? req.get_param_value("tab") : "owners";

    if (tab != "owners" && tab != "pets" && tab != "vaccinations")
    {
        tab = "owners";
    }

    string search = trim(req.get_param_value("search"));

    string message;

    if (req.has_param("msg"))
    {
        message = "<div class=\"msg ok\">" + htmlEscape(req.get_param_value("msg")) + "</div>";
    }
    else if (req.has_param("err"))
    {
        message = "<div class=\"msg error\">" + htmlEscape(req.get_param_value("err")) + "</div>";
    }

    string ownerRows;
    string ownerOptions;
    string petRows;
    string petOptions;
    string vaccRows;

    try
    {
        lock_guard<mutex> lock(dbMutex);

        // ---------------- OWNERS ----------------

        {
            auto stmt = prepare(
                "SELECT owner_id, full_name, nic, address, phone, email "
                "FROM owners ORDER BY owner_id"
            );

            unique_ptr<sql::ResultSet> result(stmt->executeQuery());

            while (result->next())
            {
                string id = to_string(result->getInt("owner_id"));

                ownerRows +=
                    "<tr><td>" + id + "</td>"
                    "<td>" + show(result->getString("full_name")) + "</td>"
                    "<td>" + show(result->getString("nic")) + "</td>"
                    "<td>" + show(result->getString("address")) + "</td>"
                    "<td>" + show(result->getString("phone")) + "</td>"
                    "<td>" + show(result->getString("email")) + "</td></tr>\n";

                ownerOptions +=
                    "<option value=\"" + id + "\">#" + id + " - " +
                    htmlEscape(result->getString("full_name")) + " (" +
                    htmlEscape(result->getString("nic")) + ")</option>\n";
            }

            if (ownerRows.empty())
            {
                ownerRows = "<tr><td colspan=\"6\" class=\"empty\">No owners found.</td></tr>";
            }
        }

        // ---------------- PETS ----------------

        {
            string query =
                "SELECT p.pet_id, p.registration_no, p.owner_id, "
                "o.full_name AS owner_name, p.pet_name, p.species, "
                "p.breed, p.gender, p.date_of_birth, p.color "
                "FROM pets p JOIN owners o ON o.owner_id = p.owner_id ";

            if (!search.empty())
            {
                query += "WHERE p.registration_no = ? ";
            }

            query += "ORDER BY p.pet_id";

            auto stmt = prepare(query);

            if (!search.empty())
            {
                stmt->setString(1, search);
            }

            unique_ptr<sql::ResultSet> result(stmt->executeQuery());

            while (result->next())
            {
                string id = to_string(result->getInt("pet_id"));

                petRows +=
                    "<tr><td>" + id + "</td>"
                    "<td>" + show(result->getString("registration_no")) + "</td>"
                    "<td>#" + to_string(result->getInt("owner_id")) + " " +
                        htmlEscape(result->getString("owner_name")) + "</td>"
                    "<td>" + show(result->getString("pet_name")) + "</td>"
                    "<td>" + show(result->getString("species")) + "</td>"
                    "<td>" + show(result->getString("breed")) + "</td>"
                    "<td>" + show(result->getString("gender")) + "</td>"
                    "<td>" + show(result->getString("date_of_birth")) + "</td>"
                    "<td>" + show(result->getString("color")) + "</td>"
                    "<td class=\"actions-cell\">"

                    // Edit button (JS opens the dialog using these data- values)
                    "<button type=\"button\" class=\"small edit-btn\""
                    " data-id=\"" + id + "\""
                    " data-name=\"" + htmlEscape(result->getString("pet_name")) + "\""
                    " data-species=\"" + htmlEscape(result->getString("species")) + "\""
                    " data-breed=\"" + htmlEscape(result->getString("breed")) + "\""
                    " data-gender=\"" + htmlEscape(result->getString("gender")) + "\""
                    " data-dob=\"" + htmlEscape(result->getString("date_of_birth")) + "\""
                    " data-color=\"" + htmlEscape(result->getString("color")) + "\">Edit</button>"

                    // Delete button (plain form POST)
                    "<form method=\"post\" action=\"/pets/delete\" class=\"inline delete-form\">"
                    "<input type=\"hidden\" name=\"pet_id\" value=\"" + id + "\">"
                    "<button type=\"submit\" class=\"small danger\">Delete</button>"
                    "</form>"

                    "</td></tr>\n";
            }

            if (petRows.empty())
            {
                petRows = "<tr><td colspan=\"10\" class=\"empty\">No pets found.</td></tr>";

                if (!search.empty() && message.empty())
                {
                    message = "<div class=\"msg error\">Pet not found!</div>";
                }
            }
        }

        // Pet dropdown for the vaccination form (always all pets)
        {
            auto stmt = prepare(
                "SELECT pet_id, registration_no, pet_name FROM pets ORDER BY pet_id"
            );

            unique_ptr<sql::ResultSet> result(stmt->executeQuery());

            while (result->next())
            {
                string id = to_string(result->getInt("pet_id"));

                petOptions +=
                    "<option value=\"" + id + "\">#" + id + " - " +
                    htmlEscape(result->getString("pet_name")) + " (" +
                    htmlEscape(result->getString("registration_no")) + ")</option>\n";
            }
        }

        // ---------------- VACCINATIONS ----------------

        {
            auto stmt = prepare(
                "SELECT v.vaccination_id, v.pet_id, p.pet_name, v.vaccine_name, "
                "v.vaccination_date, v.next_due_date "
                "FROM vaccinations v JOIN pets p ON v.pet_id = p.pet_id "
                "ORDER BY v.vaccination_id"
            );

            unique_ptr<sql::ResultSet> result(stmt->executeQuery());

            while (result->next())
            {
                vaccRows +=
                    "<tr><td>" + to_string(result->getInt("vaccination_id")) + "</td>"
                    "<td>#" + to_string(result->getInt("pet_id")) + " " +
                        htmlEscape(result->getString("pet_name")) + "</td>"
                    "<td>" + show(result->getString("vaccine_name")) + "</td>"
                    "<td>" + show(result->getString("vaccination_date")) + "</td>"
                    "<td>" + show(result->getString("next_due_date")) + "</td></tr>\n";
            }

            if (vaccRows.empty())
            {
                vaccRows = "<tr><td colspan=\"5\" class=\"empty\">No vaccinations found.</td></tr>";
            }
        }
    }
    catch (sql::SQLException& e)
    {
        cerr << "MySQL Error: " << e.what() << endl;

        message = "<div class=\"msg error\">Database error: " + htmlEscape(e.what()) + "</div>";
    }

    replaceAll(page, "{{MESSAGE}}",       message);
    replaceAll(page, "{{ACTIVE_TAB}}",    tab);
    replaceAll(page, "{{SEARCH_VALUE}}",  htmlEscape(search));
    replaceAll(page, "{{OWNER_OPTIONS}}", "<option value=\"\">Select owner</option>\n" + ownerOptions);
    replaceAll(page, "{{PET_OPTIONS}}",   "<option value=\"\">Select pet</option>\n" + petOptions);
    replaceAll(page, "{{OWNER_ROWS}}",    ownerRows);
    replaceAll(page, "{{PET_ROWS}}",      petRows);
    replaceAll(page, "{{VACC_ROWS}}",     vaccRows);

    return page;
}


// =====================================================
// REGISTER OWNER   (POST /owners/add)
// =====================================================

void registerOwner(const httplib::Request& req, httplib::Response& res)
{
    guarded(res, "owners", [&]()
    {
        string fullName = param(req, "full_name");
        string nic      = param(req, "nic");
        string address  = param(req, "address");
        string phone    = param(req, "phone");
        string email    = param(req, "email");

        if (fullName.empty() || nic.empty())
        {
            redirectTo(res, "owners", "Full name and NIC are required.", true);
            return;
        }

        // Check duplicate NIC
        {
            auto checkStmt = prepare(
                "SELECT owner_id, full_name FROM owners WHERE nic = ?"
            );

            checkStmt->setString(1, nic);

            unique_ptr<sql::ResultSet> checkResult(checkStmt->executeQuery());

            if (checkResult->next())
            {
                redirectTo(res, "owners",
                    "Owner already exists! Owner ID " +
                    to_string(checkResult->getInt("owner_id")) + " - " +
                    string(checkResult->getString("full_name")), true);
                return;
            }
        }

        // Insert owner
        {
            auto stmt = prepare(
                "INSERT INTO owners (full_name, nic, address, phone, email) "
                "VALUES (?, ?, ?, ?, ?)"
            );

            stmt->setString(1, fullName);
            stmt->setString(2, nic);
            setOptional(stmt.get(), 3, address);
            setOptional(stmt.get(), 4, phone);
            setOptional(stmt.get(), 5, email);

            stmt->executeUpdate();
        }

        // Get new owner ID
        int newId = 0;

        {
            auto idStmt = prepare("SELECT owner_id FROM owners WHERE nic = ?");

            idStmt->setString(1, nic);

            unique_ptr<sql::ResultSet> idResult(idStmt->executeQuery());

            if (idResult->next())
            {
                newId = idResult->getInt("owner_id");
            }
        }

        redirectTo(res, "owners",
            "Owner registered successfully! Owner ID: " + to_string(newId), false);
    });
}


// =====================================================
// REGISTER PET   (POST /pets/add)
// =====================================================

void registerPet(const httplib::Request& req, httplib::Response& res)
{
    guarded(res, "pets", [&]()
    {
        string ownerIdText   = param(req, "owner_id");
        string registrationNo = param(req, "registration_no");
        string petName       = param(req, "pet_name");
        string species       = param(req, "species");
        string breed         = param(req, "breed");
        string gender        = param(req, "gender");
        string dateOfBirth   = param(req, "date_of_birth");
        string color         = param(req, "color");

        if (ownerIdText.empty() || registrationNo.empty() || petName.empty())
        {
            redirectTo(res, "pets", "Owner, registration no and pet name are required.", true);
            return;
        }

        int ownerId = atoi(ownerIdText.c_str());

        // Check owner
        {
            auto ownerCheck = prepare("SELECT owner_id FROM owners WHERE owner_id = ?");

            ownerCheck->setInt(1, ownerId);

            unique_ptr<sql::ResultSet> ownerResult(ownerCheck->executeQuery());

            if (!ownerResult->next())
            {
                redirectTo(res, "pets", "Owner ID does not exist!", true);
                return;
            }
        }

        // Check duplicate registration number
        {
            auto checkStmt = prepare("SELECT pet_id FROM pets WHERE registration_no = ?");

            checkStmt->setString(1, registrationNo);

            unique_ptr<sql::ResultSet> checkResult(checkStmt->executeQuery());

            if (checkResult->next())
            {
                redirectTo(res, "pets",
                    "Pet with Registration No " + registrationNo +
                    " already exists. Existing Pet ID: " +
                    to_string(checkResult->getInt("pet_id")), true);
                return;
            }
        }

        // Insert pet
        {
            auto stmt = prepare(
                "INSERT INTO pets (registration_no, owner_id, pet_name, species, "
                "breed, gender, date_of_birth, color) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
            );

            stmt->setString(1, registrationNo);
            stmt->setInt(2, ownerId);
            stmt->setString(3, petName);
            setOptional(stmt.get(), 4, species);
            setOptional(stmt.get(), 5, breed);
            setOptional(stmt.get(), 6, gender);
            setOptional(stmt.get(), 7, dateOfBirth);
            setOptional(stmt.get(), 8, color);

            stmt->executeUpdate();
        }

        redirectTo(res, "pets", "Pet registered successfully!", false);
    });
}


// =====================================================
// REGISTER VACCINATION   (POST /vaccinations/add)
// =====================================================

void registerVaccination(const httplib::Request& req, httplib::Response& res)
{
    guarded(res, "vaccinations", [&]()
    {
        string petIdText      = param(req, "pet_id");
        string vaccineName    = param(req, "vaccine_name");
        string vaccinationDate = param(req, "vaccination_date");
        string nextDueDate    = param(req, "next_due_date");

        if (petIdText.empty() || vaccineName.empty())
        {
            redirectTo(res, "vaccinations", "Pet and vaccine name are required.", true);
            return;
        }

        int petId = atoi(petIdText.c_str());

        string petName;

        // Check pet
        {
            auto checkStmt = prepare("SELECT pet_id, pet_name FROM pets WHERE pet_id = ?");

            checkStmt->setInt(1, petId);

            unique_ptr<sql::ResultSet> checkResult(checkStmt->executeQuery());

            if (!checkResult->next())
            {
                redirectTo(res, "vaccinations", "Pet ID does not exist!", true);
                return;
            }

            petName = checkResult->getString("pet_name");
        }

        // Insert vaccination
        {
            auto stmt = prepare(
                "INSERT INTO vaccinations "
                "(pet_id, vaccine_name, vaccination_date, next_due_date) "
                "VALUES (?, ?, ?, ?)"
            );

            stmt->setInt(1, petId);
            stmt->setString(2, vaccineName);
            setOptional(stmt.get(), 3, vaccinationDate);
            setOptional(stmt.get(), 4, nextDueDate);

            stmt->executeUpdate();
        }

        redirectTo(res, "vaccinations",
            "Vaccination registered successfully for " + petName + "!", false);
    });
}


// =====================================================
// UPDATE PET   (POST /pets/update)
// =====================================================

void updatePet(const httplib::Request& req, httplib::Response& res)
{
    guarded(res, "pets", [&]()
    {
        int petId = atoi(param(req, "pet_id").c_str());

        string petName     = param(req, "pet_name");
        string species     = param(req, "species");
        string breed       = param(req, "breed");
        string gender      = param(req, "gender");
        string dateOfBirth = param(req, "date_of_birth");
        string color       = param(req, "color");

        if (petName.empty())
        {
            redirectTo(res, "pets", "Pet name is required.", true);
            return;
        }

        // Check pet
        {
            auto checkStmt = prepare("SELECT pet_id FROM pets WHERE pet_id = ?");

            checkStmt->setInt(1, petId);

            unique_ptr<sql::ResultSet> checkResult(checkStmt->executeQuery());

            if (!checkResult->next())
            {
                redirectTo(res, "pets", "Pet ID does not exist!", true);
                return;
            }
        }

        // Update
        {
            auto stmt = prepare(
                "UPDATE pets SET "
                "pet_name = ?, species = ?, breed = ?, "
                "gender = ?, date_of_birth = ?, color = ? "
                "WHERE pet_id = ?"
            );

            stmt->setString(1, petName);
            setOptional(stmt.get(), 2, species);
            setOptional(stmt.get(), 3, breed);
            setOptional(stmt.get(), 4, gender);
            setOptional(stmt.get(), 5, dateOfBirth);
            setOptional(stmt.get(), 6, color);
            stmt->setInt(7, petId);

            stmt->executeUpdate();
        }

        redirectTo(res, "pets", "Pet updated successfully!", false);
    });
}


// =====================================================
// DELETE PET   (POST /pets/delete)
// =====================================================

void deletePet(const httplib::Request& req, httplib::Response& res)
{
    guarded(res, "pets", [&]()
    {
        int petId = atoi(param(req, "pet_id").c_str());

        // Check pet
        {
            auto checkStmt = prepare("SELECT pet_id FROM pets WHERE pet_id = ?");

            checkStmt->setInt(1, petId);

            unique_ptr<sql::ResultSet> checkResult(checkStmt->executeQuery());

            if (!checkResult->next())
            {
                redirectTo(res, "pets", "Pet ID does not exist!", true);
                return;
            }
        }

        con->setAutoCommit(false);

        try
        {
            // Delete vaccinations first
            {
                auto vaccinationStmt = prepare("DELETE FROM vaccinations WHERE pet_id = ?");

                vaccinationStmt->setInt(1, petId);
                vaccinationStmt->executeUpdate();
            }

            // Delete pet
            {
                auto deleteStmt = prepare("DELETE FROM pets WHERE pet_id = ?");

                deleteStmt->setInt(1, petId);
                deleteStmt->executeUpdate();
            }

            con->commit();
            con->setAutoCommit(true);
        }
        catch (...)
        {
            con->rollback();
            con->setAutoCommit(true);
            throw;
        }

        redirectTo(res, "pets", "Pet deleted successfully!", false);
    });
}


// =====================================================
// MAIN PROGRAM
// =====================================================

int main()
{
    // =================================================
    // OOP TESTS (same as the console version)
    // =================================================

    cout << "==================================" << endl;
    cout << "     PET REGISTRATION SYSTEM" << endl;
    cout << "==================================" << endl;

    cout << endl << "POLYMORPHISM TEST" << endl;
    cout << "-----------------" << endl;

    Animal* animal1 = new Dog();
    Animal* animal2 = new Cat();

    cout << "Animal Type: " << animal1->getSpecies() << endl;
    animal1->makeSound();

    cout << "Animal Type: " << animal2->getSpecies() << endl;
    animal2->makeSound();

    delete animal1;
    delete animal2;

    cout << endl << "ENCAPSULATION TEST" << endl;
    cout << "------------------" << endl;

    Pet pet1;
    pet1.setPetId(1);
    pet1.setPetName("Buddy");
    pet1.setSpecies("Dog");
    pet1.setBreed("Labrador");
    pet1.displayPet();

    cout << endl << "CONSTRUCTOR OVERLOADING TEST" << endl;
    cout << "-----------------------------" << endl;

    Pet pet2(2, "Kitty", "Cat", "Persian");
    pet2.displayPet();


    // =================================================
    // FIND FRONTEND FOLDER
    // =================================================

    if (!ifstream(frontendDir + "/index.html").good())
    {
        if (ifstream("frontend/index.html").good())
        {
            frontendDir = "frontend";
        }
    }

    // =================================================
    // MYSQL SETTINGS (environment variables, or defaults)
    // =================================================

    auto env = [](const char* name, const string& fallback)
    {
        const char* value = getenv(name);
        return value ? string(value) : fallback;
    };

    string dbHost = env("DB_HOST", "tcp://127.0.0.1:3306");
    string dbUser = env("DB_USER", "root");
    string dbName = env("DB_NAME", "pet_registration");
    string dbPass;

    if (getenv("DB_PASSWORD"))
    {
        dbPass = getenv("DB_PASSWORD");
    }
    else
    {
        cout << endl << "Enter MySQL password for user '" << dbUser << "': ";
        getline(cin, dbPass);
    }


    // =================================================
    // MYSQL CONNECTION
    // =================================================

    cout << endl << "MYSQL DATABASE CONNECTION" << endl;
    cout << "-------------------------" << endl;

    try
    {
        cout << "Connecting to MySQL..." << endl;

        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

        con = driver->connect(dbHost, dbUser, dbPass);

        con->setSchema(dbName);

        cout << "MySQL Connection Successful!" << endl;
        cout << "Database: " << dbName << endl;
    }
    catch (sql::SQLException& e)
    {
        cout << endl;
        cout << "MySQL Connection / Database Error!" << endl;
        cout << "Error: " << e.what() << endl;
        cout << "MySQL Error Code: " << e.getErrorCode() << endl;
        cout << "SQL State: " << e.getSQLState() << endl;
        cout << endl;
        cout << "Check: MySQL running? password correct? database/schema.sql imported?" << endl;

        return 1;
    }


    // =================================================
    // WEB SERVER
    // =================================================

    httplib::Server svr;

    // CSS and JS files
    if (!svr.set_mount_point("/css", frontendDir + "/css") ||
        !svr.set_mount_point("/js",  frontendDir + "/js"))
    {
        cout << "Warning: could not find css/js folders in " << frontendDir << endl;
    }

    // Page
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res)
    {
        res.set_content(renderPage(req), "text/html; charset=utf-8");
    });

    // Actions (HTML forms POST here)
    svr.Post("/owners/add",          registerOwner);
    svr.Post("/pets/add",            registerPet);
    svr.Post("/pets/update",         updatePet);
    svr.Post("/pets/delete",         deletePet);
    svr.Post("/vaccinations/add",    registerVaccination);

    const int port = 3000;

    cout << endl;
    cout << "==================================" << endl;
    cout << " Open in browser: http://localhost:" << port << endl;
    cout << " Press Ctrl+C to stop" << endl;
    cout << "==================================" << endl;

    if (!svr.listen("127.0.0.1", port))
    {
        cout << "Could not start the server. Is port " << port << " already in use?" << endl;
    }

    delete con;
    con = nullptr;

    return 0;
}
