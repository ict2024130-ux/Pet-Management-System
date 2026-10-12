# Pet Registration System - C++ Backend Web App

we use C++ for the backend and front end has the HTML/CSS/JS languages

```
pet-cpp-app/
├── backend/
│   ├── main.cpp        C++ server + MySQL 
│   └── httplib.h       web server library (single header, MIT) - already included
├── frontend/
│   ├── index.html      HTML template ({{PLACEHOLDERS}} C++ )
│   ├── css/style.css   CSS
│   └── js/app.js       JavaScript (tabs, edit dialog, delete confirm)
└── database/
    └── schema.sql      MySQL tables
```

## 1. Database
```
mysql -u root -p < database/schema.sql
```

## 2. Compile (backend folder eke)
MySQL Connector/C++ (mysql_driver.h or ewa) is installed on a machine that is already installed.
The console version is compileable. It does not contain: `httplib.h` (zip and file) for C++17.

**Linux / Mac**
```
cd backend
g++ -std=c++17 main.cpp -o petapp -lmysqlcppconn -lpthread
```

**Windows (MinGW)**
```
cd backend
g++ -std=c++17 main.cpp -o petapp.exe -I"<connector include path>" -L"<connector lib path>" -lmysqlcppconn -lws2_32
```

**Visual Studio**: Set the C++ Language Standard to C++17 for the project, and keep the connector include/lib paths as they were before.
`httplib.h` must be in the same folder as `main.cpp`.

## 3. Run (find the `../frontend` in backend folder )
```
./petapp            (Windows: petapp.exe)
```
You need to provide the MySQL password. If you want to supply it without a prompt, use an environment variable:
`DB_PASSWORD=yourpassword ./petapp`  (optional: `DB_HOST`, `DB_USER`, `DB_NAME`)

## 4. Browser
http://localhost:3000

Terminal eke "MySQL Connection Successful!" kiyala enna ona. Error ekak awoth ekath ekkama terminal eke pennawa.
