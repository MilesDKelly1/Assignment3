#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>

// To run tests: Define _DEBUG. To run program: Comment it out.
#define _DEBUG 

#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

using namespace std;

// --- Requirement: Week 01 Enum ---
enum BeltRank { White, Yellow, Green, Blue, Purple, Brown, Black };

// Global helper for Rank Names
string getRankName(BeltRank r) {
    switch (r) {
    case White: return "White"; case Yellow: return "Yellow";
    case Green: return "Green"; case Blue: return "Blue";
    case Purple: return "Purple"; case Brown: return "Brown";
    case Black: return "Black"; default: return "White";
    }
}

// --- REQUIREMENT: COMPOSITION CLASS ---
class FinancialRecord {
private:
    double baseRate;
    double gearCost;
    static constexpr double taxRate = 0.06;

public:
    FinancialRecord() : baseRate(0.0), gearCost(0.0) {}
    FinancialRecord(double base, double gear) : baseRate(base), gearCost(gear) {}

    // Getters/Setters
    void setBaseRate(double b) { baseRate = b; }
    void setGearCost(double g) { gearCost = g; }

    // Helper Method
    double calculateTotal() const {
        return (baseRate + gearCost) * (1.0 + taxRate);
    }
};

// --- REQUIREMENT: BASE CLASS ---
class Participant {
protected:
    int monthsEnrolled; // Protected: derived classes need access
private:
    string name;
    BeltRank rank;

public:
    Participant() : name("N/A"), monthsEnrolled(0), rank(White) {}
    Participant(string n, int m, BeltRank r) : name(n), monthsEnrolled(m), rank(r) {}

    virtual ~Participant() {}

    // Getters/Setters
    void setName(string n) { name = n; }
    string getName() const { return name; }
    void setMonths(int m) { monthsEnrolled = m; }
    int getMonths() const { return monthsEnrolled; }
    void setRank(BeltRank r) { rank = r; }
    BeltRank getRank() const { return rank; }

    // Requirement: print() that handles both console and file
    virtual void print(ostream& out) const {
        out << "Name: " << name << "\n"
            << "Months: " << monthsEnrolled << "\n"
            << "Rank: " << getRankName(rank) << "\n";
    }
};

// --- REQUIREMENT: DERIVED CLASS 1 ---
class JuniorStudent : public Participant {
private:
    string guardianName;
    FinancialRecord finance; // Composition

public:
    JuniorStudent() : Participant(), guardianName("N/A"), finance(80.0, 0.0) {}
    JuniorStudent(string n, int m, BeltRank r, string g, bool gear)
        : Participant(n, m, r), guardianName(g), finance(80.0, gear ? 125.0 : 0.0) {
    }

    string getGuardianName() const { return guardianName; }

    // Overriding print() - Calls Base version
    void print(ostream& out) const override {
        out << "[JUNIOR STUDENT RECORD]\n";
        Participant::print(out); // Call Base version
        out << "Guardian: " << guardianName << "\n"
            << "Total Due (inc. tax): $" << fixed << setprecision(2) << finance.calculateTotal() << endl;
    }
};

// --- REQUIREMENT: DERIVED CLASS 2 ---
class SeniorStudent : public Participant {
private:
    string emergencyContact;
    FinancialRecord finance; // Composition

public:
    SeniorStudent() : Participant(), emergencyContact("N/A"), finance(120.0, 0.0) {}
    SeniorStudent(string n, int m, BeltRank r, string e, bool gear)
        : Participant(n, m, r), emergencyContact(e), finance(120.0, gear ? 125.0 : 0.0) {
    }

    // Overriding print() - Calls Base version
    void print(ostream& out) const override {
        out << "[SENIOR STUDENT RECORD]\n";
        Participant::print(out); // Call Base version
        out << "Emergency Contact: " << emergencyContact << "\n"
            << "Total Due (inc. tax): $" << fixed << setprecision(2) << finance.calculateTotal() << endl;
    }
};

// --- DOCTEST UNIT TESTS ---
#ifdef _DEBUG
TEST_CASE("System Verification") {
    SUBCASE("Composition & Calculation") {
        FinancialRecord f(100.0, 50.0); // 150 * 1.06 = 159.00
        CHECK(f.calculateTotal() == doctest::Approx(159.00));
    }
    SUBCASE("Base/Derived Initialization") {
        JuniorStudent y("Timmy", 12, Yellow, "Jane Doe", true);
        CHECK(y.getName() == "Timmy");
        CHECK(y.getMonths() == 12);
        CHECK(y.getGuardianName == "Jane Doe"); // Direct access for test (requires friend or making it public for test)
    }
    SUBCASE("Default Constructors") {
        SeniorStudent a;
        CHECK(a.getName() == "N/A");
    }
}
#endif

// --- MAIN PROGRAM ---
int main(int argc, char** argv) {
#ifdef _DEBUG
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();
    if (context.shouldExit()) return res;
#endif

    // Pointer to base class to allow for "Student" to be either Junior or Senior
    Participant* currentStudent = nullptr;
    int choice;

    do {
        cout << "\n--- Dojo Management System 2.0 ---" << endl;
        cout << "1. Register Junior Student" << endl;
        cout << "2. Register Senior Student" << endl;
        cout << "3. Save Record to File" << endl;
        cout << "4. Exit" << endl;
        cout << "Selection: ";
        cin >> choice;

        if (choice == 1 || choice == 2) {
            string name, extra;
            int months, rankIdx;
            char gear;

            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Months Enrolled: "; cin >> months;
            cout << "Rank Index (0-White, 6-Black): "; cin >> rankIdx;
            cout << "Add Gear Bundle? (y/n): "; cin >> gear;

            if (choice == 1) {
                cout << "Guardian Name: "; cin.ignore(); getline(cin, extra);
                currentStudent = new JuniorStudent(name, months, (BeltRank)rankIdx, extra, (gear == 'y'));
            }
            else {
                cout << "Emergency Contact: "; cin.ignore(); getline(cin, extra);
                currentStudent = new SeniorStudent(name, months, (BeltRank)rankIdx, extra, (gear == 'y'));
            }
            cout << "Student added successfully!\n";
        }
        else if (choice == 3) {
            if (currentStudent == nullptr) {
                cout << "No student data to save!" << endl;
            }
            else {
                ofstream outFile("dojo_records.txt", ios::app);
                if (outFile.is_open()) {
                    // Logic: Passing the file stream to the print function
                    currentStudent->print(outFile);
                    outFile << "--------------------------\n";
                    outFile.close();
                    cout << "Record saved to dojo_records.txt" << endl;

                    // Also show to console
                    currentStudent->print(cout);
                }
            }
        }

    } while (choice != 4);

    delete currentStudent; // Clean up memory
    return 0;
}
