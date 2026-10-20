# 🐾 Pet Registration System

A web-based Pet Registration System developed using **C++ for the backend**, **HTML, CSS, and JavaScript for the frontend**, and **MySQL for database management**.

The system is designed to manage pet owners, their pets, and vaccination records through a web interface.

---

## 📌 Table of Contents

- [Project Overview](#-project-overview)
- [Features](#-features)
- [Technologies Used](#-technologies-used)
- [Project Structure](#-project-structure)
- [System Architecture](#-system-architecture)
- [Entity-Relationship (ER) Diagram](#-entity-relationship-er-diagram)
- [Database Design](#-database-design)
- [Prerequisites](#-prerequisites)
- [Installation and Setup](#-installation-and-setup)
- [Database Configuration](#-database-configuration)
- [Compilation](#-compilation)
- [Running the Application](#-running-the-application)
- [Accessing the Application](#-accessing-the-application)
- [Troubleshooting](#-troubleshooting)
- [Future Improvements](#-future-improvements)
- [Author](#-author)

---

## 📖 Project Overview

The Pet Registration System provides a centralized platform for maintaining pet registration information.

The application uses a C++ backend to handle server-side operations and communicate with a MySQL database. The frontend provides a user-friendly interface built with HTML, CSS, and JavaScript.

The system organizes information into three main entities:

1. **Owner** – Stores pet owner information.
2. **Pet** – Stores information about registered pets.
3. **Vaccinations** – Stores vaccination records associated with pets.

The application runs locally on port `3000`.

---

## ✨ Features

### Owner Management
- Store pet owner information.
- Maintain owner names and contact details.
- Store National Identity Card (NIC) numbers and addresses.
- Associate owners with their registered pets.

### Pet Management
- Register pets under their respective owners.
- Store pet names and other identifying information.
- Maintain species, breed, gender, and color details.
- Record pet registration information.

### Vaccination Management
- Maintain vaccination records for pets.
- Store vaccine names and vaccination dates.
- Record the next vaccination due date.
- Associate vaccination records with the relevant pet.

### Web Interface
- Access the system through a web browser.
- Use HTML and CSS to display the application interface.
- Use JavaScript to manage interactive features.
- Communicate with the C++ backend through HTTP requests.

> Note: The availability of individual features depends on their implementation in the source code.

---

## 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| C++ | Backend application and server-side logic |
| C++17 | C++ language standard |
| HTML5 | Frontend page structure |
| CSS3 | Frontend styling |
| JavaScript | Frontend interactions and event handling |
| MySQL | Relational database management |
| MySQL Connector/C++ | Connecting the C++ application to MySQL |
| cpp-httplib | HTTP server functionality |
| Git | Version control |
| GitHub | Source code hosting and project documentation |

---

## 📂 Project Structure

```text
pet-cpp-app/
│
├── backend/
│   ├── main.cpp
│   ├── httplib.h
│   └── petapp
│
├── frontend/
│   ├── index.html
│   ├── css/
│   │   └── style.css
│   └── js/
│       └── app.js
│
├── database/
│   └── schema.sql
│
└── README.md
```

### Folder Description

**`backend/`**

Contains the C++ backend application.

- `main.cpp` – Implements the HTTP server and backend database operations.
- `httplib.h` – Header-only HTTP library used by the server.
- `petapp` – Compiled application executable generated during compilation.

**`frontend/`**

Contains the frontend files.

- `index.html` – Main webpage.
- `css/style.css` – Styles and layouts.
- `js/app.js` – JavaScript for interactive functionality.

**`database/`**

Contains the MySQL database schema.

- `schema.sql` – Defines the database and its tables.

**`README.md`**

Contains project documentation and setup instructions.

---

## 🏗️ System Architecture

The application follows a simple three-layer architecture.

```text
+----------------------------------+
|          User / Browser          |
|                                  |
|       HTML + CSS + JavaScript    |
+----------------+-----------------+
                 |
                 | HTTP Requests
                 |
                 v
+----------------------------------+
|          C++ Backend             |
|                                  |
|       cpp-httplib HTTP Server    |
|       Application Logic          |
+----------------+-----------------+
                 |
                 | MySQL Connector/C++
                 |
                 v
+----------------------------------+
|          MySQL Database          |
|                                  |
|       Owner                       |
|       Pet                         |
|       Vaccinations                |
+----------------------------------+
```

### How It Works

1. The user opens the application in a web browser.
2. The frontend displays the pet registration interface.
3. JavaScript sends HTTP requests to the C++ backend when required.
4. The backend processes requests and communicates with MySQL.
5. MySQL stores or retrieves the relevant information.
6. The backend returns responses to the frontend.
7. The frontend updates the displayed information.

---

## 📊 Entity-Relationship (ER) Diagram

![Pet Management System ER Diagram](docs/eer_diagram.jpeg)
<p align="center">
  <img src="ERMS FINAL ER _page-0001.jpg" alt="ER Diagram" width="800"/>
</p>

### ER Diagram Explanation

#### 1. OWNER Entity

The `OWNER` entity stores information about pet owners.

| Attribute | Description |
|---|---|
| OwnerID | Unique identifier for each owner |
| FullName | Full name of the owner |
| Phone | Owner's contact number |
| NIC | National Identity Card number |
| Address | Owner's residential address |

**Primary Key:** `OwnerID`

#### 2. PET Entity

The `PET` entity stores information about registered pets.

| Attribute | Description |
|---|---|
| PetID | Unique identifier for each pet |
| OwnerID | Identifies the pet's owner |
| PetName | Name of the pet |
| Gender | Gender of the pet |
| Species | Species of the pet |
| Breed | Breed of the pet |
| Color | Color of the pet |
| Registration | Pet registration information |

**Primary Key:** `PetID`

**Foreign Key:** `OwnerID` references `OWNER.OwnerID`.

#### 3. VACCINATIONS Entity

The `VACCINATIONS` entity stores vaccination information.

| Attribute | Description |
|---|---|
| VaccinationID | Unique identifier for each vaccination record |
| PetID | Identifies the associated pet |
| VaccineName | Name of the vaccine |
| Vaccination_date | Date the vaccination was administered |
| Next_due_day | Next scheduled vaccination date |

**Primary Key:** `VaccinationID`

**Foreign Key:** `PetID` references `PET.PetID`.

### Relationships

**Owner to Pet — One-to-Many**

One owner can register multiple pets. Each pet belongs to one owner.

**Pet to Vaccinations — One-to-Many**

One pet can have multiple vaccination records. Each vaccination record belongs to one pet.

> **Important:** This ER diagram represents the intended logical database design. Confirm the actual column names, data types, and foreign-key constraints against `database/schema.sql`. The Mermaid diagram must be updated if the implemented schema differs.

---

## 🗄️ Database Design

The application uses MySQL as its relational database management system.

The intended database contains the following tables:

| Table | Purpose |
|---|---|
| `Owner` | Stores pet owner details |
| `Pet` | Stores pet registration information |
| `Vaccinations` | Stores vaccination records |

### Database Relationships

- `Owner.OwnerID` identifies an owner.
- `Pet.PetID` identifies a pet.
- `Pet.OwnerID` associates a pet with its owner.
- `Vaccinations.VaccinationID` identifies a vaccination record.
- `Vaccinations.PetID` associates a vaccination record with a pet.

The corresponding foreign-key constraints should be defined in the SQL schema to enforce these relationships.

---

## 💻 Prerequisites

Before running the application, ensure that the following software and libraries are installed.

- C++ compiler supporting C++17.
- MySQL Server.
- MySQL Connector/C++.
- GNU Make or other build tools, if required by your environment.
- A modern web browser.
- Git, if you want to clone the repository.

### Linux / Ubuntu

Check whether the required applications are available:

```bash
g++ --version
mysql --version
git --version
```

Check the MySQL service:

```bash
sudo systemctl status mysql
```

If the service is stopped, start it:

```bash
sudo systemctl start mysql
```

> MySQL Connector/C++ and the `httplib.h` header must also be available to the compiler. The commands below assume the connector is installed and configured for the linker options shown in your project instructions.

---

## 🚀 Installation and Setup

### Step 1: Clone the Repository

If your project is hosted on GitHub, clone it using:

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
```

Navigate into the project directory:

```bash
cd pet-cpp-app
```

If you already have the project on your computer, open a terminal in the existing project directory instead.

### Step 2: Verify the Project Files

Check that the required files exist:

```text
backend/main.cpp
backend/httplib.h
frontend/index.html
frontend/css/style.css
frontend/js/app.js
database/schema.sql
```

Make sure `httplib.h` is in the same directory as `main.cpp`.

---

## 🗄️ Database Configuration

### Step 1: Start MySQL

On Ubuntu:

```bash
sudo systemctl start mysql
```

### Step 2: Create the Database and Tables

From the project root directory, execute:

```bash
mysql -u root -p < database/schema.sql
```

Enter the MySQL password when prompted.

This command executes the SQL statements defined in `database/schema.sql`.

Make sure the SQL file creates the database and tables required by the application.

### Step 3: Verify the Database

Log in to MySQL:

```bash
mysql -u root -p
```

List the databases:

```sql
SHOW DATABASES;
```

Select your project database:

```sql
USE your_database_name;
```

Replace `your_database_name` with the database name defined in `schema.sql`.

List the tables:

```sql
SHOW TABLES;
```

Inspect a table's structure:

```sql
DESCRIBE Owner;
```

Repeat the `DESCRIBE` command for the other tables, using their actual names.

Exit MySQL:

```sql
EXIT;
```

---

## ⚙️ Database Connection Configuration

The backend needs valid MySQL connection settings.

The application supports the following optional environment variables, according to the project configuration:

| Environment Variable | Purpose |
|---|---|
| `DB_HOST` | MySQL server hostname |
| `DB_USER` | MySQL username |
| `DB_NAME` | Database name |
| `DB_PASSWORD` | MySQL password |

The application must read these variables for them to take effect.

If no custom values are supplied, the defaults implemented in `main.cpp` will be used.

### Set Environment Variables on Linux / Ubuntu

For the current terminal session:

```bash
export DB_HOST=localhost
export DB_USER=root
export DB_NAME=your_database_name
```

Set the password without placing it directly in the command history:

```bash
read -s -p "MySQL password: " DB_PASSWORD
echo
export DB_PASSWORD
```

Replace `your_database_name` with your actual database name.

These variables remain available in the current shell session and its child processes.

---

## 🔨 Compilation

### Linux / Ubuntu / macOS

Navigate to the backend directory:

```bash
cd backend
```

Compile the application:

```bash
g++ -std=c++17 main.cpp -o petapp -lmysqlcppconn -lpthread
```

This command:

- Enables the C++17 language standard.
- Compiles `main.cpp`.
- Generates an executable named `petapp`.
- Links the MySQL Connector/C++ library.
- Links the POSIX threads library.

If your MySQL Connector/C++ installation uses different library names or locations, update the compilation command to match your installation.

### Windows — MinGW

Open a terminal in the backend directory:

```bash
cd backend
```

Compile using your installed connector paths:

```bash
g++ -std=c++17 main.cpp -o petapp.exe -I"<connector include path>" -L"<connector lib path>" -lmysqlcppconn -lws2_32
```

Replace the placeholder include and library paths with the paths on your computer.

### Windows — Visual Studio

1. Open the C++ project in Visual Studio.
2. Set the C++ language standard to C++17.
3. Configure the MySQL Connector/C++ include directory.
4. Configure the required library directory.
5. Link the appropriate MySQL Connector/C++ library.
6. Ensure `httplib.h` is accessible to `main.cpp`.
7. Build the project.

> The compilation commands above assume your installed MySQL Connector/C++ version provides the specified library. Some connector versions use different library names or APIs, so adjust the configuration if necessary.

---

## ▶️ Running the Application

After successful compilation, remain in the `backend` directory.

Run the application:

```bash
./petapp
```

On Windows, run:

```bash
petapp.exe
```

If your application requests the MySQL password interactively, enter it in the terminal.

If the backend reads the `DB_PASSWORD` environment variable, you can set it before running the application.

For example:

```bash
DB_PASSWORD='your_password' ./petapp
```

Replace `your_password` with your actual MySQL password.

**Security note:** Avoid committing database passwords or other credentials to GitHub.

### Expected Output

When the database connection succeeds, the terminal should display:

```text
MySQL Connection Successful!
```

The exact output depends on the implementation in `main.cpp`.

If a database connection or server startup error occurs, check the terminal output for details.

---

## 🌐 Accessing the Application

Once the C++ server has started successfully, open your web browser.

Navigate to:

**http://localhost:3000**

The browser should display the Pet Registration System interface.

### Application Workflow

1. Start the MySQL server.
2. Run the database setup SQL file if the database has not been initialized.
3. Configure the database connection settings.
4. Compile the C++ backend.
5. Start the backend executable.
6. Open `http://localhost:3000` in your browser.
7. Use the available interface to manage pet registration information.

Keep the backend terminal running while using the application.

---

## 🧰 Troubleshooting

### 1. MySQL Connection Failed

**Possible causes:**
- MySQL Server is not running.
- The username or password is incorrect.
- The database name is incorrect.
- The required database has not been created.
- The MySQL server is listening on a different host or port.

**Solutions:**

Check the MySQL service:

```bash
sudo systemctl status mysql
```

Start the service if necessary:

```bash
sudo systemctl start mysql
```

Test your credentials:

```bash
mysql -u root -p
```

Verify that the database exists:

```sql
SHOW DATABASES;
```

Confirm that the database settings used by `main.cpp` match your MySQL configuration.

### 2. `httplib.h: No Such File or Directory`

**Cause:** The compiler cannot find the HTTP library header.

**Solution:**

Check the backend directory:

```bash
ls backend
```

Ensure that `httplib.h` is in the same directory as `main.cpp`.

If you are already inside `backend`, use:

```bash
ls
```

### 3. `mysql_driver.h: No Such File or Directory`

**Cause:** The MySQL Connector/C++ headers cannot be found.

**Solution:**

Check the connector installation and configure the compiler's include path.

For example:

```bash
g++ -std=c++17 main.cpp -I"<connector include path>" -o petapp -lmysqlcppconn -lpthread
```

Replace the placeholder with the actual include directory.

### 4. `cannot find -lmysqlcppconn`

**Cause:** The linker cannot locate a library with that name.

**Solution:**

Verify the installed MySQL Connector/C++ version and its library files.

Configure the library search path using `-L` if needed, and use the correct library name for your connector installation.

### 5. Port 3000 Is Already in Use

**Cause:** Another application is using port `3000`.

**Solution:**

Check which process is listening on the port:

```bash
sudo lsof -i :3000
```

If another application is using it, stop that application if appropriate, or configure your server to use another available port.

### 6. The Browser Cannot Open the Application

**Possible causes:**
- The C++ backend is not running.
- The server failed to start.
- The application uses a different port.
- A firewall or networking configuration is blocking access.

**Solutions:**

1. Check the backend terminal.
2. Confirm that the server started successfully.
3. Verify that the server uses port `3000`.
4. Open `http://localhost:3000` again.

### 7. Database Tables Are Missing

**Cause:** The database schema has not been imported successfully.

**Solution:**

From the project root directory, run:

```bash
mysql -u root -p < database/schema.sql
```

Then verify the tables:

```bash
mysql -u root -p
```

```sql
USE your_database_name;
SHOW TABLES;
```

Replace `your_database_name` with the actual database name.

---

## 🔐 Security Considerations

The following practices are recommended for a safer application:

- Do not commit passwords or credentials to version control.
- Use environment variables or a secure configuration mechanism for database credentials.
- Use parameterized SQL queries to help prevent SQL injection.
- Validate and sanitize user input on the backend.
- Handle database errors without exposing sensitive information.
- Restrict database permissions to the operations the application requires.
- Use appropriate access controls if the application is deployed beyond localhost.
- Configure HTTPS and other production security measures before exposing the application publicly.

---

## 🚧 Future Improvements

Potential improvements include:

- Owner registration and management.
- Pet registration, editing, and deletion.
- Vaccination history and reminders.
- Search and filtering for pets.
- Input validation and improved error handling.
- User authentication and role-based access control.
- Responsive frontend design for mobile devices.
- Automated database backups.
- Unit and integration testing.
- Deployment to a production server.

These are possible future enhancements; their implementation depends on project requirements.

---

## 👩‍💻 Author

**Project Name:** Pet Registration System

**Backend:** C++

**Frontend:** HTML, CSS, JavaScript

**Database:** MySQL

**HTTP Server Library:** cpp-httplib

**Language Standard:** C++17

**Project Type:** Web Application

---

## 📄 License

The project license should be specified here.

The `httplib.h` library is distributed under the MIT License. Check the license of each other dependency and include the appropriate notices when distributing the project.

---

## ⭐ Acknowledgements

- The C++ community.
- MySQL documentation and development tools.
- The cpp-httplib project.
- Open-source software contributors.



## Demonstration Video & Walkthrough

To see the live deployment, interface navigation, and database connectivity in action, please watch our video demonstration.

📺 Watch the Demonstration

https://youtu.be/8DTMPoQSihw?si=suls8MZKJvJoLJxM

---

**Pet Registration System — Managing pet owners, pets, and vaccination records in one place.**
