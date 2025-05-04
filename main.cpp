/*
 * Program: Kennel Management System
 * Author: Walter Hodgson
 * Date: Final Project Submission
 * Purpose: Menu-based application for interacting with kennel_project.db
 */

 #include <iostream>
 #include <sqlite3.h>
 #include <string>
 #include <limits>
 
 using namespace std;
 
// Add functions
void addClient(sqlite3* db);
void addPet(sqlite3* db);

// Update functions
void updateClient(sqlite3* db);
void updatePet(sqlite3* db);

// Delete functions
void deleteClient(sqlite3* db);
void deletePet(sqlite3* db);

// Function for a transaction
void makeSaleTransaction(sqlite3* db);

// Report functions
void viewBoardingHistoryForClient(sqlite3* db);   // Joins pets and clients
void viewGroomingAppointments(sqlite3* db);       // Joins groomers and clients and appointments

// Function for menu utility
int promptForInt(const std::string& prompt);
 
 int main() {
    sqlite3* db;
    if (sqlite3_open("kennel_project.db", &db) != SQLITE_OK) { // Attempting to open the database
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    cout << "Connected to kennel_project.db successfully.\n";

    int choice;
    do { // Main loop for the menu
        cout << "\n--- Kennel Management Menu ---" << endl;
        cout << "1. Add Record" << endl;
        cout << "2. Update Record" << endl;
        cout << "3. Delete Record" << endl;
        cout << "4. Process Transaction (Sale)" << endl;
        cout << "5. Report: Boarding History for Client" << endl;
        cout << "6. Report: Grooming Appointments" << endl;
        cout << "7. Quit" << endl;

        choice = promptForInt("Enter choice: "); // Getting user input
        // Switch and case for handling the user choice
        switch (choice) {
            case 1: { // Add menu choice
                cout << "\nAdd Menu:\n1. Add Client\n2. Add Pet" << endl;
                int sub = promptForInt("Enter choice: ");
                if (sub == 1) addClient(db);
                else if (sub == 2) addPet(db);
                else cout << "Invalid option." << endl;
                break;
            }
            case 2: { // Update
                cout << "\nUpdate Menu:\n1. Update Client\n2. Update Pet" << endl;
                int sub = promptForInt("Enter choice: ");
                if (sub == 1) updateClient(db);
                else if (sub == 2) updatePet(db);
                else cout << "Invalid option." << endl;
                break;
            }
            case 3: { // Delete menu 
                cout << "\nDelete Menu:\n1. Delete Client\n2. Delete Pet" << endl;
                int sub = promptForInt("Enter choice: ");
                if (sub == 1) deleteClient(db);
                else if (sub == 2) deletePet(db);
                else cout << "Invalid option." << endl;
                break;
            }
            case 4: // Runs the transaction function for processing a sale
                makeSaleTransaction(db);
                break;
            case 5: // Displays a report for the boarding reservation
                viewBoardingHistoryForClient(db);
                break;
            case 6: // Join for the grooming appointment
                viewGroomingAppointments(db);
                break;
            case 7: // Exit the program
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }

    } while (choice != 7);

    sqlite3_close(db); // Closing the database
    return 0;
}

 // Utility function
 int promptForInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt; // Display the prompt
        cin >> value; // Getting user input
        if (cin.fail()) { // Check if the input is valid
            cin.clear(); // Clear a fail state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the input in the buffer
            cout << "Invalid input. Please enter a number." << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Return a valid choice
            return value;
        }
    }
}

// Function for adding a new client to the table
 void addClient(sqlite3* db) {
    string name, phone, email, address;
    // Getting the client details
    cout << "\n=== Add New Client ===" << endl;
    cout << "Enter client name: ";
    getline(cin, name);
    cout << "Enter phone number: ";
    getline(cin, phone);
    cout << "Enter email: ";
    getline(cin, email);
    cout << "Enter address: ";
    getline(cin, address);
    // SQL insert statement using parameters 
    const char* sql = "INSERT INTO client (client_name, phone, email, client_address) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    // Preparing the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding user input to the statement parameters
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, address.c_str(), -1, SQLITE_TRANSIENT);
    // Execute the statement and give a prompt if successful 
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "✅ Client added successfully." << endl;
    } else {
        cerr << "❌ Failed to add client: " << sqlite3_errmsg(db) << endl;
    }
    // Finalize statement
    sqlite3_finalize(stmt);
}

// Function for adding a pet to the table
void addPet(sqlite3* db) {
    string name, breed, medicalCondition, dietRestriction, emergencyContact;
    int age, friendly, clientId;
    // Getting the pet details
    cout << "\n=== Add New Pet ===" << endl;
    cout << "Enter pet name: ";
    getline(cin, name);
    cout << "Enter breed: ";
    getline(cin, breed);
    age = promptForInt("Enter age: ");
    cout << "Enter medical condition (or leave blank): ";
    getline(cin, medicalCondition);
    cout << "Enter diet restriction (or leave blank): ";
    getline(cin, dietRestriction);
    // Validating the input for friendliness
    while (true) {
        friendly = promptForInt("Is the pet friendly? (1 = yes, 0 = no): ");
        if (friendly == 0 || friendly == 1) break;
        cout << "Please enter 1 (yes) or 0 (no)." << endl;
    }

    cout << "Enter emergency contact: ";
    getline(cin, emergencyContact);
    clientId = promptForInt("Enter associated client ID: ");
    // SQL insert statement with parameters ? is a placeholder
    const char* sql = "INSERT INTO pet (pet_name, breed, age, medical_condition, diet_restriction, friendly, emergency_contact, client_id) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) { // Preparing the SQL statement
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding values to the statement
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, breed.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, medicalCondition.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, dietRestriction.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, friendly);
    sqlite3_bind_text(stmt, 7, emergencyContact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, clientId);
    // Execute the statement and printing a message if successful
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        cout << "✅ Pet added successfully." << endl;
    } else {
        cerr << "❌ Failed to add pet: " << sqlite3_errmsg(db) << endl;
    }
    // Finalizing the statement
    sqlite3_finalize(stmt);
}

// Function for updating a client's information
void updateClient(sqlite3* db) {
    int clientId = promptForInt("\nEnter the client ID to update: "); // Getting the client ID to update it
    // Preparing a SELECT query to fetch the current client information
    const char* selectSQL = "SELECT client_name, phone, email, client_address FROM client WHERE client_id = ?;";
    sqlite3_stmt* selectStmt;
    // Compiling the SELECT statement
    if (sqlite3_prepare_v2(db, selectSQL, -1, &selectStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding the client ID to the SELECT statement
    sqlite3_bind_int(selectStmt, 1, clientId);
    if (sqlite3_step(selectStmt) != SQLITE_ROW) { // Executing the SELECT statement
        cout << "❌ No client found with that ID." << endl;
        sqlite3_finalize(selectStmt);
        return;
    }

    // Show current client information
    cout << "\nCurrent values:" << endl;
    cout << "Name: " << sqlite3_column_text(selectStmt, 0) << endl;
    cout << "Phone: " << sqlite3_column_text(selectStmt, 1) << endl;
    cout << "Email: " << sqlite3_column_text(selectStmt, 2) << endl;
    cout << "Address: " << sqlite3_column_text(selectStmt, 3) << endl;

    sqlite3_finalize(selectStmt); // Cleaning up the SELECT statement before modification

    // Prompt for new values
    string name, phone, email, address;
    cout << "\nEnter new name: ";
    getline(cin, name);
    cout << "Enter new phone: ";
    getline(cin, phone);
    cout << "Enter new email: ";
    getline(cin, email);
    cout << "Enter new address: ";
    getline(cin, address);

    const char* updateSQL = // Preparing the UPDATE SQL statement
        "UPDATE client SET client_name = ?, phone = ?, email = ?, client_address = ? WHERE client_id = ?;";
    
    sqlite3_stmt* updateStmt; // Compiling the UPDATE statement
    if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare UPDATE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding the new values and client ID to the statement
    sqlite3_bind_text(updateStmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 2, phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 4, address.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(updateStmt, 5, clientId);

    if (sqlite3_step(updateStmt) == SQLITE_DONE) { // Executing the UPDATE and printing a confirmation 
        cout << "✅ Client updated successfully." << endl;
    } else {
        cerr << "❌ Failed to update client: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(updateStmt);
}

// Function to update a pet's information
void updatePet(sqlite3* db) {
    int petId = promptForInt("\nEnter the pet ID to update: "); // Getting the pet ID to update
    // Preparing a SELECT query to get the pet details
    const char* selectSQL = "SELECT pet_name, breed, age, medical_condition, diet_restriction, friendly, emergency_contact, client_id FROM pet WHERE pet_id = ?;";
    sqlite3_stmt* selectStmt;
    // Compiling the SELECT statement
    if (sqlite3_prepare_v2(db, selectSQL, -1, &selectStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding the pet ID to the SELECT statement
    sqlite3_bind_int(selectStmt, 1, petId);
    if (sqlite3_step(selectStmt) != SQLITE_ROW) { // Checking if the pet exists
        cout << "❌ No pet found with that ID." << endl;
        sqlite3_finalize(selectStmt);
        return;
    }

    // Showing current values
    cout << "\nCurrent values:" << endl;
    cout << "Name: " << sqlite3_column_text(selectStmt, 0) << endl;
    cout << "Breed: " << sqlite3_column_text(selectStmt, 1) << endl;
    cout << "Age: " << sqlite3_column_int(selectStmt, 2) << endl;
    cout << "Medical Condition: " << sqlite3_column_text(selectStmt, 3) << endl;
    cout << "Diet Restriction: " << sqlite3_column_text(selectStmt, 4) << endl;
    cout << "Friendly: " << sqlite3_column_int(selectStmt, 5) << endl;
    cout << "Emergency Contact: " << sqlite3_column_text(selectStmt, 6) << endl;
    cout << "Client ID: " << sqlite3_column_int(selectStmt, 7) << endl;

    sqlite3_finalize(selectStmt);

    // Promptting for new values
    string name, breed, medicalCondition, dietRestriction, emergencyContact;
    int age, friendly, clientId;

    cout << "\nEnter new name: ";
    getline(cin, name);
    cout << "Enter new breed: ";
    getline(cin, breed);
    age = promptForInt("Enter new age: ");
    cout << "Enter new medical condition: ";
    getline(cin, medicalCondition);
    cout << "Enter new diet restriction: ";
    getline(cin, dietRestriction);
    
    while (true) { // Friendliness field has to be a 0 or a 1
        friendly = promptForInt("Is the pet friendly? (1 = yes, 0 = no): ");
        if (friendly == 0 || friendly == 1) break;
        cout << "Please enter 1 (yes) or 0 (no)." << endl;
    }

    cout << "Enter new emergency contact: ";
    getline(cin, emergencyContact);
    clientId = promptForInt("Enter new associated client ID: ");
    // Preparing the UPDATE statement for pet
    const char* updateSQL =
        "UPDATE pet SET pet_name = ?, breed = ?, age = ?, medical_condition = ?, diet_restriction = ?, friendly = ?, emergency_contact = ?, client_id = ? "
        "WHERE pet_id = ?;";
    // Compiling the UPDATE statement
    sqlite3_stmt* updateStmt;
    if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare UPDATE statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Binding the updated values to the UPDATE statement
    sqlite3_bind_text(updateStmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 2, breed.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(updateStmt, 3, age);
    sqlite3_bind_text(updateStmt, 4, medicalCondition.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(updateStmt, 5, dietRestriction.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(updateStmt, 6, friendly);
    sqlite3_bind_text(updateStmt, 7, emergencyContact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(updateStmt, 8, clientId);
    sqlite3_bind_int(updateStmt, 9, petId);

    if (sqlite3_step(updateStmt) == SQLITE_DONE) { // Executing UPDATE
        cout << "✅ Pet updated successfully." << endl;
    } else {
        cerr << "❌ Failed to update pet: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(updateStmt); // Finalizing the statement
}

// Function to delete a client from the database
void deleteClient(sqlite3* db) {
    cout << "\n==== Delete Client ====" << endl;

    // Display all the clients for reference
    const char* selectSQL = "SELECT client_id, client_name FROM client;";
    sqlite3_stmt* selectStmt;

    if (sqlite3_prepare_v2(db, selectSQL, -1, &selectStmt, nullptr) != SQLITE_OK) { // Preparing the SELECT statement
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Display the list of clients
    cout << "Client List:\n";
    while (sqlite3_step(selectStmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(selectStmt, 0);
        const unsigned char* nameText = sqlite3_column_text(selectStmt, 1);
        cout << id << ": " << (nameText ? reinterpret_cast<const char*>(nameText) : "(Unnamed)") << endl;
    }
    sqlite3_finalize(selectStmt);

    // Prompt for ID to delete
    int clientId = promptForInt("Enter the client ID to delete: ");

    // Confirm are you super certain about this?
    string confirm;
    cout << "Are you sure you want to delete client ID " << clientId << "? This will also delete any dependent pets or records. (yes/no): ";
    getline(cin, confirm);
    if (confirm != "yes") {
        cout << "Canceled deletion." << endl;
        return;
    }

    // Preparing the DELETE
    const char* deleteSQL = "DELETE FROM client WHERE client_id = ?;";
    sqlite3_stmt* deleteStmt;

    if (sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(deleteStmt, 1, clientId); // Bind the client ID to the DELETE statement

    if (sqlite3_step(deleteStmt) == SQLITE_DONE) { // Execute the DELETE and print success
        cout << "✅ Client deleted successfully." << endl;
    } else {
        cerr << "❌ Failed to delete client: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(deleteStmt); // Cleaning up
}

// Function to delete a pet
void deletePet(sqlite3* db) {
    cout << "\n==== Delete Pet ====" << endl;

    // Display all the pets for reference
    const char* selectSQL = "SELECT pet_id, pet_name FROM pet;";
    sqlite3_stmt* selectStmt;
    // Preparing the SELECT statement
    if (sqlite3_prepare_v2(db, selectSQL, -1, &selectStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Displaying a list of the pets
    cout << "Pet List:\n";
    while (sqlite3_step(selectStmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(selectStmt, 0);
        const unsigned char* nameText = sqlite3_column_text(selectStmt, 1);
        cout << id << ": " << (nameText ? reinterpret_cast<const char*>(nameText) : "(Unnamed)") << endl;
    }
    sqlite3_finalize(selectStmt);

    // Getting the ID to delete
    int petId = promptForInt("Enter the pet ID to delete: ");

    // How do you actually feel about this?
    string confirm;
    cout << "Are you sure you want to delete pet ID " << petId << "? This will also delete any dependent records. (yes/no): ";
    getline(cin, confirm);
    if (confirm != "yes") {
        cout << "Canceled deletion." << endl;
        return;
    }

    // Preparing the delete statement
    const char* deleteSQL = "DELETE FROM pet WHERE pet_id = ?;";
    sqlite3_stmt* deleteStmt;

    if (sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(deleteStmt, 1, petId); // Binding the pet ID to the statement

    if (sqlite3_step(deleteStmt) == SQLITE_DONE) { // Executing and reporting the result
        cout << "✅ Pet deleted successfully." << endl;
    } else {
        cerr << "❌ Failed to delete pet: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(deleteStmt);
}

// Function to process a retail sale
void makeSaleTransaction(sqlite3* db) {
    cout << "\n==== New Sale Transaction ====" << endl;
    // Getting all the transaction information
    int clientId = promptForInt("Enter client ID: ");
    int employeeId = promptForInt("Enter employee ID: ");
    int date = promptForInt("Enter transaction date (e.g., 20230501): ");
    int time = promptForInt("Enter transaction time (e.g., 1430 for 2:30 PM): ");
    string paymentMethod;
    cout << "Enter payment method: ";
    getline(cin, paymentMethod);

    // Beinning of a transaction below
    char* errMsg = nullptr;
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Failed to begin transaction: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    // Inserting blank transaction details for entry into the general_ledger table
    const char* insertLedgerSQL = R"(
        INSERT INTO general_ledger (ledger_date, ledger_time, item_or_service_purchase, amount, discount, payment_method, client_id, employee_id)
        VALUES (?, ?, 'Retail Sale', 0.0, 0.0, ?, ?, ?);
    )";
    sqlite3_stmt* ledgerStmt;
    if (sqlite3_prepare_v2(db, insertLedgerSQL, -1, &ledgerStmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    // Binding values into the statement for general_ledger
    sqlite3_bind_int(ledgerStmt, 1, date);
    sqlite3_bind_int(ledgerStmt, 2, time);
    sqlite3_bind_text(ledgerStmt, 3, paymentMethod.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(ledgerStmt, 4, clientId);
    sqlite3_bind_int(ledgerStmt, 5, employeeId);

    if (sqlite3_step(ledgerStmt) != SQLITE_DONE) { // Executing the INSERT
        cerr << "Insert ledger failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(ledgerStmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(ledgerStmt);

    // Getting the autogenerated ledger_id for reference
    int ledgerId = sqlite3_last_insert_rowid(db);

    float totalAmount = 0.0;

    // Allowing the user to add one or more items into sale
    while (true) {
        int itemId = promptForInt("Enter item ID (0 to finish): ");
        if (itemId == 0) break;

        int quantity = promptForInt("Enter quantity: ");

        // Checking stock to see if item is available and getting the item price
        const char* stockSQL = "SELECT stock_level, price FROM retail_item WHERE item_id = ?;";
        sqlite3_stmt* stockStmt;
        if (sqlite3_prepare_v2(db, stockSQL, -1, &stockStmt, nullptr) != SQLITE_OK) {
            cerr << "Prepare stock check failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return;
        }
        sqlite3_bind_int(stockStmt, 1, itemId);
        if (sqlite3_step(stockStmt) != SQLITE_ROW) {
            cerr << "Item not found." << endl;
            sqlite3_finalize(stockStmt);
            continue;
        }

        int currentStock = sqlite3_column_int(stockStmt, 0);
        float price = sqlite3_column_double(stockStmt, 1);
        sqlite3_finalize(stockStmt);

        if (quantity > currentStock) { // Reject statement if there isn't enough 
            cout << "Not enough stock. Available: " << currentStock << endl;
            continue;
        }

        // Insert ledger_item entry for the item sold
        const char* itemSQL = "INSERT INTO ledger_item (ledger_id, item_id, quantity) VALUES (?, ?, ?);";
        sqlite3_stmt* itemStmt;
        sqlite3_prepare_v2(db, itemSQL, -1, &itemStmt, nullptr);
        sqlite3_bind_int(itemStmt, 1, ledgerId);
        sqlite3_bind_int(itemStmt, 2, itemId);
        sqlite3_bind_int(itemStmt, 3, quantity);

        if (sqlite3_step(itemStmt) != SQLITE_DONE) {
            cerr << "Failed to insert ledger item: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(itemStmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return;
        }
        sqlite3_finalize(itemStmt);

        // Updating the stock levels
        const char* updateStockSQL = "UPDATE retail_item SET stock_level = stock_level - ? WHERE item_id = ?;";
        sqlite3_stmt* updateStmt;
        sqlite3_prepare_v2(db, updateStockSQL, -1, &updateStmt, nullptr);
        sqlite3_bind_int(updateStmt, 1, quantity);
        sqlite3_bind_int(updateStmt, 2, itemId);

        if (sqlite3_step(updateStmt) != SQLITE_DONE) {
            cerr << "Stock update failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(updateStmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return;
        }
        sqlite3_finalize(updateStmt);

        totalAmount += price * quantity; // Update total 
    }

    // Updating the total sale amount in the general_ledger
    const char* updateLedgerTotal = "UPDATE general_ledger SET amount = ? WHERE general_ledger_id = ?;";
    sqlite3_stmt* updateLedgerStmt;
    sqlite3_prepare_v2(db, updateLedgerTotal, -1, &updateLedgerStmt, nullptr);
    sqlite3_bind_double(updateLedgerStmt, 1, totalAmount);
    sqlite3_bind_int(updateLedgerStmt, 2, ledgerId);
    if (sqlite3_step(updateLedgerStmt) != SQLITE_DONE) {
        cerr << "Failed to update ledger total: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(updateLedgerStmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(updateLedgerStmt);

    // Committing the transaction assuming everything worked
    if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Commit failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    cout << "✅ Sale transaction completed successfully. Total charged: $" << totalAmount << endl; // Confirmation for user
}

// Function to display the boarding history for a client joining with pet table
void viewBoardingHistoryForClient(sqlite3* db) {
    int clientId = promptForInt("\nEnter client ID to view their pet's boarding history: "); // Getting the client ID
    // SQL query will join the boarding reservation and the pet table
    const char* sql = R"(
        SELECT pet.pet_name, boarding_reservation.check_in, boarding_reservation.check_out, boarding_reservation.amount
        FROM boarding_reservation
        JOIN pet ON boarding_reservation.pet_id = pet.pet_id
        WHERE boarding_reservation.client_id = ?
        ORDER BY boarding_reservation.check_in DESC;
    )";
    // Preparing the SQL query
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Query error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, clientId); // Binding client ID to the statement
    cout << "\n=== Boarding History ===\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) { // Step through rows and display
        string petName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int checkIn = sqlite3_column_int(stmt, 1); // Check-In
        int checkOut = sqlite3_column_int(stmt, 2); // Check-Out
        float amount = sqlite3_column_double(stmt, 3); // Amount charged

        cout << "Pet: " << petName << " | Check-In: " << checkIn << " | Check-Out: " << checkOut << " | Amount: $" << amount << endl; // Outputting details 
    }
    sqlite3_finalize(stmt);
}

// Function to display all the grooming appointments with client and groomer info
void viewGroomingAppointments(sqlite3* db) { 
    const char* sql = R"(
        SELECT g.appointment_id, c.client_name, gr.groomer_name, g.grooming_date, g.grooming_time
        FROM grooming_appointment g
        JOIN client c ON g.client_id = c.client_id
        JOIN groomer gr ON g.groomer_id = gr.groomer_id
        ORDER BY g.grooming_date DESC, g.grooming_time;
    )";
    // Preparing the SQL statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Query error: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\n=== Grooming Appointments ===\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) { // Loop through the results and print details
        int appointmentId = sqlite3_column_int(stmt, 0); // Appointment ID
        string clientName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); // Client
        string groomerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); // Groomer
        int date = sqlite3_column_int(stmt, 3); // Appointment date
        int time = sqlite3_column_int(stmt, 4); // Appointment time
        // Output information
        cout << "Appt ID: " << appointmentId 
             << " | Client: " << clientName 
             << " | Groomer: " << groomerName 
             << " | Date: " << date 
             << " | Time: " << time << endl;
    }
    sqlite3_finalize(stmt);
}
