#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

// To run tests: Define _DEBUG. To run program: Comment it out.
#define _DEBUG 

#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

using namespace std;

// --- Week 01 Enum ---
enum BeltRank { White, Yellow, Green, Blue, Purple, Brown, Black };

string getRankName(BeltRank r) {
    switch (r) {
    case White: return "White"; case Yellow: return "Yellow";
    case Green: return "Green"; case Blue: return "Blue";
    case Purple: return "Purple"; case Brown: return "Brown";
    case Black: return "Black"; default: return "White";
    }
}

// --- Composition Class (Existing) ---
class FinancialRecord {
private:
    double baseRate;
    double gearCost;
    static constexpr double taxRate = 0.06;
public:
    FinancialRecord() : baseRate(0.0), gearCost(0.0) {}
    FinancialRecord(double base, double gear) : baseRate(base), gearCost(gear) {}
    void setBaseRate(double b) { baseRate = b; }
    void setGearCost(double g) { gearCost = g; }
    double calculateTotal() const {
        return (baseRate + gearCost) * (1.0 + taxRate);
    }
};

// --- REQUIREMENT: ABSTRACT BASE CLASS ---
class Participant {
protected:
    int monthsEnrolled;
private:
    string name;
    BeltRank rank;

public:
    Participant() : name("N/A"), monthsEnrolled(0), rank(White) {}
    Participant(string n, int m, BeltRank r) : name(n), monthsEnrolled(m), rank(r) {}

    // Requirement: Virtual Destructor
    virtual ~Participant() {}

    // Requirement: Pure Virtual Function (Makes class abstract)
    virtual string getStudentType() const = 0;

    void setName(string n) { name = n; }
    string getName() const { return name; }
    void setMonths(int m) { monthsEnrolled = m; }
    int getMonths() const { return monthsEnrolled; }
    void setRank(BeltRank r) { rank = r; }
    BeltRank getRank() const { return rank; }

    // Requirement: Keep print() but make it virtual
    virtual void print(ostream& out) const {
        out << "Name: " << name << " | Months: " << monthsEnrolled
            << " | Rank: " << getRankName(rank) << " | Type: " << getStudentType();
    }
};

// --- Derived Class 1 ---
class JuniorStudent : public Participant {
private:
    string guardianName;
    FinancialRecord finance;
public:
    JuniorStudent(string n, int m, BeltRank r, string g, bool gear)
        : Participant(n, m, r), guardianName(g), finance(80.0, gear ? 125.0 : 0.0) {
    }

    // Requirement: Implement pure virtual
    string getStudentType() const override { return "Junior"; }

    void print(ostream& out) const override {
        Participant::print(out);
        out << " | Guardian: " << guardianName << " | Total: $" << fixed << setprecision(2) << finance.calculateTotal();
    }
};

// --- Derived Class 2 ---
class SeniorStudent : public Participant {
private:
    string emergencyContact;
    FinancialRecord finance;
public:
    SeniorStudent(string n, int m, BeltRank r, string e, bool gear)
        : Participant(n, m, r), emergencyContact(e), finance(120.0, gear ? 125.0 : 0.0) {
    }

    // Requirement: Implement pure virtual
    string getStudentType() const override { return "Senior"; }

    void print(ostream& out) const override {
        Participant::print(out);
        out << " | Emergency: " << emergencyContact << " | Total: $" << fixed << setprecision(2) << finance.calculateTotal();
    }
};

// --- REQUIREMENT: MANAGER CLASS (No STL) ---
class DojoManager {
private:
    Participant** items; // Double pointer array
    int size;
    int capacity;

    void resize() {
        capacity *= 2;
        Participant** newItems = new Participant * [capacity];
        for (int i = 0; i < size; i++) newItems[i] = items[i];
        delete[] items;
        items = newItems;
    }

public:
    DojoManager(int cap = 2) : size(0), capacity(cap) {
        items = new Participant * [capacity];
    }

    // Requirement: Safe memory cleanup
    ~DojoManager() {
        for (int i = 0; i < size; i++) delete items[i];
        delete[] items;
    }

    void addParticipant(Participant* p) {
        if (size >= capacity) resize();
        items[size++] = p;
    }

    void removeParticipant(int index) {
        if (index < 0 || index >= size) return;
        delete items[index];
        for (int i = index; i < size - 1; i++) items[i] = items[i + 1];
        size--;
    }

    void printAll(ostream& out) const {
        for (int i = 0; i < size; i++) {
            items[i]->print(out); // Polymorphic print
            out << endl;
        }
    }

    int getSize() const { return size; }
    Participant* getAt(int index) const { return items[index]; }
};


// --- Updated Unit Tests ---
#ifdef _DEBUG
TEST_CASE("DojoManager Verification") {
    DojoManager dm(1); // Force resize
    dm.addParticipant(new JuniorStudent("Timmy", 1, White, "Jane", false));
    dm.addParticipant(new SeniorStudent("Old Joe", 10, Black, "Contact", true));

    SUBCASE("Dynamic Sizing & Addition") {
        CHECK(dm.getSize() == 2);
    }
    SUBCASE("Polymorphism (Pure Virtual Test)") {
        CHECK(dm.getAt(0)->getStudentType() == "Junior");
        CHECK(dm.getAt(1)->getStudentType() == "Senior");
    }
    SUBCASE("Safe Deletion") {
        dm.removeParticipant(0);
        CHECK(dm.getSize() == 1);
        CHECK(dm.getAt(0)->getName() == "Old Joe");
    }
}
#endif

// --- Main Program ---
int main(int argc, char** argv) {
#ifdef _DEBUG
    doctest::Context context; context.applyCommandLine(argc, argv);
    int res = context.run(); if (context.shouldExit()) return res;
#endif

    DojoManager manager;
    int choice;

    do {
        cout << "\n1. Register Junior\n2. Register Senior\n3. View All/Save\n4. Remove Student\n5. Exit\nSelection: ";
        cin >> choice;

        if (choice == 1 || choice == 2) {
            string name, extra; int months, rank; char gear;
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Months: "; cin >> months;
            cout << "Rank (0-6): "; cin >> rank;
            cout << "Gear? (y/n): "; cin >> gear;

            if (choice == 1) {
                cout << "Guardian: "; cin.ignore(); getline(cin, extra);
                manager.addParticipant(new JuniorStudent(name, months, (BeltRank)rank, extra, (gear == 'y')));
            }
            else {
                cout << "Emergency Contact: "; cin.ignore(); getline(cin, extra);
                manager.addParticipant(new SeniorStudent(name, months, (BeltRank)rank, extra, (gear == 'y')));
            }
        }
        else if (choice == 3) {
            manager.printAll(cout);
            ofstream outFile("dojo_records.txt", ios::app);
            if (outFile.is_open()) {
                manager.printAll(outFile);
                outFile.close();
            }
        }
        else if (choice == 4) {
            int idx;
            cout << "Enter index to remove (0 to " << manager.getSize() - 1 << "): ";
            cin >> idx;
            manager.removeParticipant(idx);
        }
    } while (choice != 5);

    return 0;
}
