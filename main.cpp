#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

#define _DEBUG
#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

using namespace std;

// --- Week 01 Enum ---
enum BeltRank { White, Yellow, Green, Blue, Purple, Brown, Black };
string getRankName(BeltRank r) {
    string names[] = { "White", "Yellow", "Green", "Blue", "Purple", "Brown", "Black" };
    return (r >= 0 && r <= 6) ? names[r] : "White";
}

// --- REQUIREMENT: Function Template ---
template <typename T>
T getMax(T a, T b) {
    return (a > b) ? a : b;
}

// --- Composition Class ---
class FinancialRecord {
private:
    double baseRate;
    double gearCost;
    static constexpr double taxRate = 0.06;
public:
    FinancialRecord(double base = 0.0, double gear = 0.0) : baseRate(base), gearCost(gear) {}
    double calculateTotal() const { return (baseRate + gearCost) * (1.0 + taxRate); }
};

// --- ABSTRACT BASE CLASS ---
class Participant {
protected:
    int monthsEnrolled;
    string name;
    BeltRank rank;
public:
    Participant(string n = "N/A", int m = 0, BeltRank r = White) : name(n), monthsEnrolled(m), rank(r) {}
    virtual ~Participant() {}
    virtual string getStudentType() const = 0;
    
    string getName() const { return name; }
    BeltRank getRank() const { return rank; }

    // REQUIREMENT: Overload operator<< using polymorphism
    friend ostream& operator<<(ostream& os, const Participant& p) {
        p.print(os);
        return os;
    }

    virtual void print(ostream& out) const {
        out << "Name: " << name << " | Type: " << getStudentType();
    }
};

// --- Derived Class 1 ---
class JuniorStudent : public Participant {
private:
    string guardianName;
    FinancialRecord finance;
public:
    JuniorStudent(string n, int m, BeltRank r, string g, bool gear) 
        : Participant(n, m, r), guardianName(g), finance(80.0, gear ? 125.0 : 0.0) {}

    string getStudentType() const override { return "Junior"; }

    // REQUIREMENT: Overload operator== 
    bool operator==(const JuniorStudent& other) const {
        // REQUIREMENT: Explicit use of 'this'
        return (this->name == other.name && this->guardianName == other.guardianName);
    }

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
        : Participant(n, m, r), emergencyContact(e), finance(120.0, gear ? 125.0 : 0.0) {}

    string getStudentType() const override { return "Senior"; }

    void print(ostream& out) const override {
        Participant::print(out);
        out << " | Emergency: " << emergencyContact << " | Total: $" << fixed << setprecision(2) << finance.calculateTotal();
    }
};

// --- REQUIREMENT: CLASS TEMPLATE (Replaces DojoManager) ---
template <typename T>
class DynamicArray {
private:
    T* items;
    int size;
    int capacity;

    void resize() {
        capacity *= 2;
        T* newItems = new T[capacity];
        for (int i = 0; i < size; i++) newItems[i] = items[i];
        delete[] items;
        this->items = newItems; // REQUIREMENT: Use 'this'
    }

public:
    DynamicArray(int cap = 2) : size(0), capacity(cap) { items = new T[capacity]; }
    
    ~DynamicArray() {
        for (int i = 0; i < size; i++) delete items[i]; // Cleanup for pointers
        delete[] items;
    }

    // REQUIREMENT: Overload operator[]
    T& operator[](int index) {
        if (index < 0 || index >= size) return items[0]; // Simple bounds safety
        return items[index];
    }

    // REQUIREMENT: Overload operator+=
    void operator+=(T item) {
        if (size >= capacity) resize();
        items[size++] = item;
    }

    // REQUIREMENT: Overload operator-=
    void operator-=(int index) {
        if (index < 0 || index >= size) return;
        delete items[index];
        for (int i = index; i < size - 1; i++) items[i] = items[i + 1];
        size--;
    }

    int getSize() const { return size; }
};

#ifdef _DEBUG
TEST_CASE("New Requirements Verification") {
    DynamicArray<Participant*> da;
    JuniorStudent* j1 = new JuniorStudent("Tim", 1, White, "Jane", false);
    JuniorStudent* j2 = new JuniorStudent("Tim", 1, White, "Jane", false);
    
    SUBCASE("Operator==") { CHECK(*j1 == *j2); }
    SUBCASE("Operator+=") { 
        da += j1; 
        CHECK(da.getSize() == 1); 
    }
    SUBCASE("Operator[]") {
        da += j2;
        CHECK(da[1]->getName() == "Tim");
    }
    SUBCASE("Operator<<") {
        ostringstream oss;
        oss << *j1;
        CHECK(oss.str().find("Tim") != string::npos);
    }
    SUBCASE("Function Template") { CHECK(getMax(10, 20) == 20); }
}
#endif

int main(int argc, char** argv) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#ifdef _DEBUG
    doctest::Context context; context.applyCommandLine(argc, argv);
    int res = context.run(); if (context.shouldExit()) return res;
#endif

    DynamicArray<Participant*> manager;
    int choice;
    do {
        cout << "\n1. Add Junior\n2. Add Senior\n3. View\n4. Remove\n5. Exit\nSelection: ";
        cin >> choice;
        if (choice == 1 || choice == 2) {
            string name, extra; int m, r; char g;
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Months: "; cin >> m; cout << "Rank (0-6): "; cin >> r; cout << "Gear (y/n): "; cin >> g;
            if (choice == 1) {
                cout << "Guardian: "; cin.ignore(); getline(cin, extra);
                manager += new JuniorStudent(name, m, (BeltRank)r, extra, (g == 'y'));
            } else {
                cout << "Emergency: "; cin.ignore(); getline(cin, extra);
                manager += new SeniorStudent(name, m, (BeltRank)r, extra, (g == 'y'));
            }
        } else if (choice == 3) {
            for (int i = 0; i < manager.getSize(); i++) cout << "[" << i << "] " << *manager[i] << endl;
        } else if (choice == 4) {
            int idx; cout << "Index: "; cin >> idx;
            manager -= idx;
        }
    } while (choice != 5);

    return 0;
}
