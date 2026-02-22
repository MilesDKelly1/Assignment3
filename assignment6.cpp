#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

// To run tests: Define _DEBUG. To run program: Comment it out.
#define _DEBUG 

#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

using namespace std;

// --- Requirement: Function Template ---
template <typename T>
bool isEqual(T a, T b) {
    return a == b;
}

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

// --- REQUIREMENT: CLASS TEMPLATE (Replaces manual Week 5 logic) ---
template <typename T>
class CustomArray {
private:
    T* items;
    int size;
    int capacity;

    void resize() {
        capacity *= 2;
        T* newItems = new T[capacity];
        for (int i = 0; i < size; i++) newItems[i] = items[i];
        delete[] items;
        items = newItems;
    }

public:
    CustomArray(int cap = 2) : size(0), capacity(cap) {
        items = new T[capacity];
    }

    ~CustomArray() { delete[] items; }

    // REQUIREMENT: operator+= (Adds item)
    void operator+=(T item) {
        if (size >= capacity) resize();
        items[size++] = item;
    }

    // REQUIREMENT: operator-= (Removes item and shifts)
    void operator-=(int index) {
        if (index < 0 || index >= size) return;
        for (int i = index; i < size - 1; i++) items[i] = items[i + 1];
        size--;
    }

    // REQUIREMENT: operator[] with bounds checking
    T& operator[](int index) {
        if (index < 0 || index >= size) {
            return items[0]; // Safe fallback for invalid index
        }
        return items[index];
    }

    int getSize() const { return size; }
};

// --- Composition Class ---
class FinancialRecord {
private:
    double baseRate;
    double gearCost;
    static constexpr double taxRate = 0.06;
public:
    FinancialRecord() : baseRate(0.0), gearCost(0.0) {}
    FinancialRecord(double base, double gear) : baseRate(base), gearCost(gear) {}
    double calculateTotal() const { return (baseRate + gearCost) * (1.0 + taxRate); }
};

// --- REQUIREMENT: ABSTRACT BASE CLASS ---
class Participant {
protected:
    int monthsEnrolled;
    string name;
    BeltRank rank;

public:
    Participant(string n, int m, BeltRank r) : name(n), monthsEnrolled(m), rank(r) {}
    virtual ~Participant() {}

    // REQUIREMENT: Use 'this' pointer
    void incrementEnrollment() {
        this->monthsEnrolled++;
    }

    virtual string getStudentType() const = 0;

    // REQUIREMENT: Virtual function for polymorphic operator<<
    virtual void toStream(ostream& out) const {
        out << name << " | Rank: " << getRankName(rank) << " | Type: " << getStudentType();
    }

    // REQUIREMENT: Global operator<< for Base Class
    friend ostream& operator<<(ostream& out, const Participant& p) {
        p.toStream(out);
        return out;
    }

    string getName() const { return name; }
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

    string getStudentType() const override { return "Junior"; }

    // REQUIREMENT: Overload operator== 
    bool operator==(const JuniorStudent& other) const {
        return (this->name == other.name && this->guardianName == other.guardianName);
    }

    void toStream(ostream& out) const override {
        Participant::toStream(out);
        out << " | Guardian: " << guardianName;
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

    string getStudentType() const override { return "Senior"; }

    void toStream(ostream& out) const override {
        Participant::toStream(out);
        out << " | Emergency: " << emergencyContact;
    }
};

// --- MANAGER CLASS (Uses the CustomArray Template) ---
class DojoManager {
private:
    CustomArray<Participant*> members;

public:
    ~DojoManager() {
        for (int i = 0; i < members.getSize(); i++) delete members[i];
    }

    void addParticipant(Participant* p) { members += p; }

    void removeParticipant(int index) {
        if (index >= 0 && index < members.getSize()) {
            delete members[index]; // Memory cleanup before removal
            members -= index;      // Array shift
        }
    }

    void printAll(ostream& out) const {
        for (int i = 0; i < members.getSize(); i++) {
            out << *members[i] << endl; // Uses overloaded <<
        }
    }

    int getSize() const { return members.getSize(); }
    Participant* operator[](int i) { return members[i]; }
};

// --- Updated Unit Tests ---
#ifdef _DEBUG
TEST_CASE("Week 06 Template & Operator Tests") {
    DojoManager dm;
    JuniorStudent* j1 = new JuniorStudent("Timmy", 1, White, "Jane", false);
    JuniorStudent* j2 = new JuniorStudent("Timmy", 1, White, "Jane", false);
    JuniorStudent* j3 = new JuniorStudent("Billy", 1, White, "Bob", false);

    SUBCASE("Equality Operator") {
        CHECK(isEqual<JuniorStudent>(*j1, *j2) == true);
        CHECK(isEqual<JuniorStudent>(*j1, *j3) == false);
    }

    SUBCASE("Operator<< & Polymorphism") {
        std::ostringstream oss;
        oss << *j1;
        CHECK(oss.str().find("Junior") != std::string::npos);
        CHECK(oss.str().find("Guardian: Jane") != std::string::npos);
    }

    SUBCASE("Template Addition & Indexing") {
        dm.addParticipant(j1);
        dm.addParticipant(j3);
        CHECK(dm.getSize() == 2);
        CHECK(dm[0]->getName() == "Timmy");
    }

    delete j2; // j1 and j3 are handled by DojoManager
}
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
    doctest::Context context; context.applyCommandLine(argc, argv);
    int res = context.run(); if (context.shouldExit()) return res;
#endif

    DojoManager manager;
    int choice;
    do {
        cout << "\n1. Register Junior\n2. Register Senior\n3. View All\n4. Remove Student\n5. Exit\nSelection: ";
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
        }
        else if (choice == 4) {
            int idx;
            cout << "Index to remove: "; cin >> idx;
            manager.removeParticipant(idx);
        }
    } while (choice != 5);
    return 0;
