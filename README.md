# Pet Registration System - C++ Backend Web App

Backend eka **C++** (oyage original code eka). Frontend (HTML/CSS/JS) saha database wenama folders walata.
JSON naha: HTML forms C++ server ekata POST karanawa, C++ eka MySQL update karala page eka aluthin hadala denawa.

```
pet-cpp-app/
├── backend/
│   ├── main.cpp        C++ server + MySQL (oyage classes tikath thiyenawa)
│   └── httplib.h       web server library (single header, MIT) - already included
├── frontend/
│   ├── index.html      HTML template ({{PLACEHOLDERS}} C++ eken fill wenawa)
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
MySQL Connector/C++ (mysql_driver.h wala ewa) oyage machine eke already thiyenawa, ethakota oya kalin
console version eka compile karapu widiyatama compile karanna. Aluthin ona de: `httplib.h` (zip eke thiyenawa) saha C++17.

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

**Visual Studio**: project eke C++ Language Standard = C++17 karanna, connector include/lib paths kalin wage thiyanna.
`main.cpp` ekka `httplib.h` same folder eke thiyenna ona.

## 3. Run (backend folder eke dammama `../frontend` hoyaganna)
```
./petapp            (Windows: petapp.exe)
```
MySQL password eka ahanawa. Prompt eka nathuwa dana ona nam environment variable ekak use karanna:
`DB_PASSWORD=yourpassword ./petapp`  (optional: `DB_HOST`, `DB_USER`, `DB_NAME`)

## 4. Browser
http://localhost:3000

Terminal eke "MySQL Connection Successful!" kiyala enna ona. Error ekak awoth ekath ekkama terminal eke pennawa.
