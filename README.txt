KENNEL MANAGEMENT SYSTEM
Author: Walter Hodgson
Last Updated: [Insert today's date]
Database: kennel_project.db

PROJECT OVERVIEW:

This is a  C++ application that connects to my kennel_project.db SQLite database. 
It allows users to manage operations for the kennel such as: client and pet records, transactions, grooming appointments, and inventory.
To do so, I'm using SQLite's C++ API to execute SQL queries.

PROGRAM FEATURES:

1. Add Records (2 tables)
   - Add a new Client
   - Add a new Pet 

2. Update Records (2 tables)
   - Update Pet details
   - Update Client information

3. Delete Records (2 table)
   - Delete a Pet by ID
   - Delete a Client by ID

4. Transaction (multi-step)
   - Record a customer transaction (general ledger)
   - Add associated items sold
   - Update inventory levels for each item

5. Reports (2 join-based queries)
   - View all Pets with their Owners
   - View Grooming Appointments (with Client & Groomer names)

HOW TO USE:

1. Compile the program:
    (Built on a Mac hence using clang)
   clang++ main.cpp -o out -lsqlite3

2. Run the program:
   ./out

3. Follow the menu prompts:
   - Use numbers to select menu items.
   - Enter prompted information (e.g., names, dates, IDs).
   - To exit the program, choose option 7: "Quit".

4. Notes:
   - All database constraints and foreign key relationships are enforced.
   - Dates should be entered as integers in YYYYMMDD format.
   - Times should be entered as integers in HHMM format (e.g., 930 for 9:30 AM).

FILES INCLUDED IN SUBMISSION:

- main.cpp                --> Source code
- kennel_project.db       --> SQLite database
- README.txt              --> This file