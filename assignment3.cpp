#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>

// To run tests: Define _DEBUG. To run program: Comment it out.
//#define _DEBUG 

#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

using namespace std;

enum BeltRank { White, Yellow, Green, Blue, Purple, Brown, Black };

// --- REQUIREMENT D: THE STUDENT CLASS ---
class Student {
private:
    string name;
    int age;
    string startDate;
    int monthsEnrolled;
    BeltRank rank;
    double baseRate;
    bool needsGear;

    static const int youthAgeThreshold = 16;
    static constexpr double youthClassPayment = 80.0;
    static constexpr double adultClassPayment = 120.0;
    static constexpr double gearBundleCost = 125.00;
    static constexpr double taxRate = 0.06;
    static constexpr int minAge = 6;

public:
    Student() : name("N/A"), age(0), startDate("01/24/2026"), monthsEnrolled(0), rank(White), needsGear(false) {
        baseRate = 100.0;
    }

    // Setters (Includes Calculation 1 & Guard Case)
    void setAge(int a) {
        age = (a < 0) ? 0 : a; // Guard against negative
        baseRate = (age < youthAgeThreshold) ? youthClassPayment : adultClassPayment; // Calculation 1
    }
    void setName(string n) { name = n; }
    void setStartDate(string d) { startDate = d; }
    void setMonths(int m) { monthsEnrolled = (m < 0) ? 0 : m; }
    void setRank(BeltRank r) { rank = r; }
    void setGearStatus(bool status) { needsGear = status; }

    // Getters for the File Output
    string getName() const { return name; }
    int getAge() const { return age; }
    string getStartDate() const { return startDate; }
    int getMonths() const { return monthsEnrolled; }
    BeltRank getRank() const { return rank; }
    bool getGearStatus() const { return needsGear; }

    // --- REQUIREMENT D: Class Methods (Calculations 2, 3, 4) ---
    double getEquipmentCost() const {
        if (!needsGear) return 0.0;
        // Bundle: Hoodie ($35.00) + Sparring Set ($90.00) = $125.00
        return gearBundleCost;
    } //Calc 2
    double getSalesTax() const {
        return (baseRate + getEquipmentCost()) * taxRate;
    } //Calc 3
    double getTotalBill() const {
        return baseRate + getEquipmentCost() + getSalesTax();
    } //Calc 4

    bool isValidForTraining() const { return age >= minAge; } // Guard Case
};

// Global helper for Requirement B
string getRankName(BeltRank r) {
    switch (r) {
    case White: return "White"; case Yellow: return "Yellow";
    case Green: return "Green"; case Blue: return "Blue";
    case Purple: return "Purple"; case Brown: return "Brown";
    case Black: return "Black"; default: return "White";
    }
}

// REQUIREMENT C: Array Processing
int countStudentsByRank(const Student students[], int size, BeltRank targetRank) {
    if (size <= 0) return 0; // Guard case
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (students[i].getRank() == targetRank) count++;
    }
    return count;
}

#ifdef _DEBUG
TEST_CASE("Dojo Management System Tests") {
    Student s;

    SUBCASE("A) Calculations") {
        // Normal Cases

        //Test Adult and gearNeeded
        s.setGearStatus(true); // $125.00
        s.setAge(20); // Test Adult Rate ($120.00)
        // Total = (120 + 125) = 245. Tax = 245 * 0.06 = 14.70

        CHECK(s.getSalesTax() == doctest::Approx(14.70));
        CHECK(s.getTotalBill() == doctest::Approx(259.70));
        
        //Test Youth and no geadNeeded
        s.setAge(10); // Test Youth Rate ($80.00)
        s.setGearStatus(false);
        // Total = 80. Tax = 80 * 0.06 = 4.80

        CHECK(s.getSalesTax() == doctest::Approx(4.80));
        CHECK(s.getTotalBill() == doctest::Approx(84.80));

        // Edge Case - 0 Age (Boundary)
        s.setAge(0);
        CHECK(s.isValidForTraining() == false); // Guard case for minAge=6

        // Guard Case - Negative Input Handling
        s.setAge(-5);
        CHECK(s.getAge() == 0); // Ensuring the setter clamps negative values
    }

    SUBCASE("B) Enum Logic") {
        CHECK(getRankName(White) == "White");
        CHECK(getRankName(Black) == "Black");
        CHECK(getRankName(static_cast<BeltRank>(99)) == "White"); // Guard
    }

    SUBCASE("C) Array Processing") {
        Student school[3];
        school[0].setRank(White);
        school[1].setRank(Black);
        school[2].setRank(Black);

        CHECK(countStudentsByRank(school, 3, Black) == 2); // Normal
        CHECK(countStudentsByRank(school, 3, Green) == 0); // Edge
        CHECK(countStudentsByRank(school, 0, Black) == 0); // Guard
    }

    SUBCASE("D) Class Method Edge Cases") {
        // Normal Case
        s.setAge(16); // Adult
        CHECK(s.getTotalBill() > 0);

        // Edge Case (0 sessions/months)
        s.setMonths(0);
        CHECK(s.getMonths() == 0);
    }
}
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    return context.run(); // Runs tests and exits
#else
    Student currentStudent;
    int choice;
    bool dataEntered = false;

    do {
        cout << "\n--- Dojo Management System (2026) ---" << endl;
        cout << "1. Register/Add Student Information" << endl;
        cout << "2. Generate and Save Record (File Output)" << endl;
        cout << "3. Student Summary" << endl;
        cout << "4. Exit" << endl;
        cout << "Selection: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Exiting..." << endl;
            break;
        }

        switch (choice) {
        case 1: {
            string tName, tDate; char gearChoice;
            int tAge;
            cout << "First and Last Name: "; cin.ignore(); getline(cin, tName);
            cout << "Age: "; cin >> tAge;
            cout << "Start Date (MM/DD/YYYY): "; cin >> tDate;
            // Show available ranks to the user
            cout << "Available Ranks: ";
            for (int i = White; i <= Black; i++) {
                cout << "[" << i << "] " << getRankName(static_cast<BeltRank>(i)) << (i < Black ? ", " : "");
            }

            int rankChoice;
            cout << "\nSelect Rank (0-6): ";
            cin >> rankChoice;
            currentStudent.setRank(static_cast<BeltRank>(rankChoice));
            cout << "Add Gear Bundle (Hoodie + Sparring Set for $125)? (y/n): "; cin >> gearChoice;

            currentStudent.setName(tName);
            currentStudent.setAge(tAge);
            currentStudent.setStartDate(tDate);
            currentStudent.setGearStatus(gearChoice == 'y' || gearChoice == 'Y');
            dataEntered = true;
            cout << "Information recorded successfully!" << endl;
            break;
        }
        case 2: {
            if (!dataEntered) {
                cout << "Nothing to record yet." << endl;
            }
            else {
                ofstream outFile("record.txt");
                cout << "Saving Record for: " << currentStudent.getName() << endl;

                outFile << "DOJO STUDENT RECORD 2026\n" << string(30, '=') << endl;
                outFile << "Name: " << currentStudent.getName() << endl;
                outFile << "Age: " << currentStudent.getAge() << endl;
                outFile << "RANK: [" << getRankName(currentStudent.getRank()) << " Belt]" << endl;
                outFile << "Gear Included: " << (currentStudent.getGearStatus() ? "Yes ($125.00)" : "No") << endl;
                outFile << "Tax (6%): $" << fixed << setprecision(2) << currentStudent.getSalesTax() << endl;
                outFile << "TOTAL DUE: $" << currentStudent.getTotalBill() << endl;
                outFile.close();
                cout << "Record saved to record.txt" << endl;
            }
            break;
        }
        case 3: { // Add a new case for "View Student Summary"
            if (!dataEntered) {
                cout << "No student data found." << endl;
            }
            else {
                cout << "\n--- 2026 STUDENT STATUS ---" << endl;
                cout << left << setw(15) << "Name:" << currentStudent.getName() << endl;
                cout << left << setw(15) << "Current Rank:" << "[" << getRankName(currentStudent.getRank()) << "]" << endl;
                cout << left << setw(15) << "Total Bill:" << "$" << fixed << setprecision(2) << currentStudent.getTotalBill() << endl;
                cout << "---------------------------" << endl;
            }
            break;
        }
        case 4:
            cout << "Exiting system..." << endl;
            break;
        default:
            cout << "Invalid choice." << endl;
        }
    } while (choice != 4);
#endif
    return 0;
}
[![CodeQL Advanced](https://github.com/MilesDKelly1/Assignment3/actions/workflows/codeql.yml/badge.svg?branch=main&event=public)](https://github.com/MilesDKelly1/Assignment3/actions/workflows/codeql.yml)
