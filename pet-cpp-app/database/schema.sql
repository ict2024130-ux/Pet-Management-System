CREATE DATABASE IF NOT EXISTS pet_registration;
USE pet_registration;

CREATE TABLE IF NOT EXISTS owners (
    owner_id  INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    nic       VARCHAR(20)  NOT NULL UNIQUE,
    address   VARCHAR(255),
    phone     VARCHAR(20),
    email     VARCHAR(100)
);

CREATE TABLE IF NOT EXISTS pets (
    pet_id          INT AUTO_INCREMENT PRIMARY KEY,
    registration_no VARCHAR(50) NOT NULL UNIQUE,
    owner_id        INT NOT NULL,
    pet_name        VARCHAR(100) NOT NULL,
    species         VARCHAR(50),
    breed           VARCHAR(50),
    gender          VARCHAR(10),
    date_of_birth   DATE,
    color           VARCHAR(50),
    FOREIGN KEY (owner_id) REFERENCES owners(owner_id)
);

CREATE TABLE IF NOT EXISTS vaccinations (
    vaccination_id   INT AUTO_INCREMENT PRIMARY KEY,
    pet_id           INT NOT NULL,
    vaccine_name     VARCHAR(100) NOT NULL,
    vaccination_date DATE,
    next_due_date    DATE,
    FOREIGN KEY (pet_id) REFERENCES pets(pet_id) ON DELETE CASCADE
);
