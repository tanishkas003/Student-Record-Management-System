#ifndef ADMINFUNCTION_H
#define ADMINFUNCTION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <Windows.h>

using namespace std;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[1;34m";
const string MAGENTA = "\033[1;35m";
const string CYAN = "\033[1;36m";
const string RESET = "\033[0m";

const string DATA_FILE = "data.csv";
const string EXPORT_FILE = "students_export.csv";

inline string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

inline vector<string> splitCSV(const string &line) {
    vector<string> fields;
    string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"' ) {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(cur));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    
    fields.push_back(trim(cur));
    return fields;
}

inline string csvEscape(const string &s) {
    if (s.find('"') != string::npos) {
        string out = "\"";
        for (char c: s) {
            if (c == '"') out += "\"\""; 
            else out.push_back(c);
        }
        out += "\"";
        return out;
    }
    if (s.find(',') != string::npos || s.find('\n') != string::npos) {
        return "\"" + s + "\"";
    }
    return s;
}

struct Student {
    int roll;
    string name;
    string college;
    string branch;
    string attendance; 
    int physics;
    int chemistry;
    int maths;
};

inline vector<Student> loadAllStudents() {
    vector<Student> out;
    ifstream fin(DATA_FILE);
    if (!fin.is_open()) return out;
    string line;
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        auto cols = splitCSV(line);
        if (cols.size() < 8) continue; // ensure complete row
        Student s;
        try { s.roll = stoi(cols[0]); } catch(...) { continue; }
        s.name = cols[1];
        s.college = cols[2];
        s.branch = cols[3];
        s.attendance = cols[4];
        try { s.physics = stoi(cols[5]); } catch(...) { s.physics = 0; }
        try { s.chemistry = stoi(cols[6]); } catch(...) { s.chemistry = 0; }
        try { s.maths = stoi(cols[7]); } catch(...) { s.maths = 0; }
        out.push_back(s);
    }
    fin.close();
    return out;
}

inline void saveAllStudents(const vector<Student> &list) {
    ofstream fout(DATA_FILE, ios::trunc);
    for (size_t i = 0; i < list.size(); ++i) {
        const Student &s = list[i];
        fout << s.roll << "," 
             << csvEscape(s.name) << "," 
             << csvEscape(s.college) << ","
             << csvEscape(s.branch) << ","
             << csvEscape(s.attendance) << ","
             << s.physics << "," << s.chemistry << "," << s.maths;
        if (i + 1 < list.size()) fout << "\n";
    }
    fout.close();
}

void mainMenu(void);
void adminFunction(void);
void addData(void);
void viewData(void);
void updateData(void);
void deleteData(void);
void searchData(void);
void exportCSV(void);
void studentFunction(void);

void mainMenu()
{
    int choice;
    do {
        system("cls");
        cout << BLUE << "\n\n\t====== Student Record Management System ======\n\n" << RESET;
        cout << "1. Admin\n";
        cout << "2. Student (view only)\n";
        cout << "3. Exit\n\n";
        cout << "Enter your choice: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
        switch (choice) {
            case 1:
                adminFunction();
                break;
            case 2:
                studentFunction();
                break;
            case 3:
            {
                cout << GREEN << "\nQuitting the program..." << RESET << "\n";
                Sleep(800);
                exit(0);
            }
            default:
                cout << RED << "Invalid input. Try again.\n" << RESET;
                Sleep(800);
        }
    } while (true);
}

void adminFunction()
{
    int option;
    do {
        system("cls");
        cout << GREEN << "\n\n\t\t| Logged In as Admin |\n\n" << RESET;
        cout << "1. Add Student Record\n";
        cout << "2. Update Student Record\n";
        cout << "3. Delete Student Record\n";
        cout << "4. View Records (with sort)\n";
        cout << "5. Search Records\n";
        cout << "6. Export Records to CSV\n";
        cout << "7. Back to Main Menu\n";
        cout << "8. Exit\n\n";
        cout << "Enter choice: ";
        if (!(cin >> option)) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        switch (option) {
            case 1: addData(); break;
            case 2: updateData(); break;
            case 3: deleteData(); break;
            case 4:
            {
                cout << "Sort by: 0=None  1=Roll  2=Name  3=Total marks (desc)\n";
                cout << "Enter option: ";
                int s; cin >> s;
                vector<Student> list = loadAllStudents();
                if (s == 1) {
                    sort(list.begin(), list.end(), [](const Student &a,const Student &b){ return a.roll < b.roll; });
                } else if (s == 2) {
                    sort(list.begin(), list.end(), [](const Student &a,const Student &b){ return a.name < b.name; });
                } else if (s == 3) {
                    sort(list.begin(), list.end(), [](const Student &a,const Student &b){
                        int ta = a.physics + a.chemistry + a.maths;
                        int tb = b.physics + b.chemistry + b.maths;
                        return ta > tb;
                    });
                }
                
                system("cls");
                cout << MAGENTA << "\n| Roll | Name | College | Branch | Attendance | Phys | Chem | Maths | Total |\n" << RESET;
                cout << string(100, '-') << "\n";
                for (auto &st : list) {
                    int total = st.physics + st.chemistry + st.maths;
                    cout << setw(5) << st.roll << " | "
                         << setw(18) << left << st.name << right << " | "
                         << setw(10) << st.college << " | "
                         << setw(8) << st.branch << " | "
                         << setw(9) << st.attendance << " | "
                         << setw(4) << st.physics << " | "
                         << setw(4) << st.chemistry << " | "
                         << setw(4) << st.maths << " | "
                         << setw(5) << total << "\n";
                }
                cout << string(100, '-') << "\n";
                system("pause");
                break;
            }
            case 5: searchData(); break;
            case 6: exportCSV(); break;
            case 7: return; 
            case 8:
                cout << GREEN << "\nQuitting the program..." << RESET << "\n";
                Sleep(800);
                exit(0);
            default:
                cout << RED << "Invalid option\n" << RESET;
                Sleep(700);
        }
    } while (true);
}

void addData()
{
    system("cls");
    Student s;
    cout << CYAN << "\n| Enter Student Data |" << RESET << "\n\n";

    cout << "Roll Number (integer): ";
    while (!(cin >> s.roll)) { cin.clear(); cin.ignore(10000,'\n'); cout << "Enter valid integer: "; }
    cin.ignore(); 

    cout << "Name: ";
    getline(cin, s.name);

    cout << "College: ";
    getline(cin, s.college);

    cout << "Branch: ";
    getline(cin, s.branch);

    cout << "Attendance (e.g., 85% or 85): ";
    getline(cin, s.attendance);

    cout << "Physics marks (0-100): ";
    while (!(cin >> s.physics)) { cin.clear(); cin.ignore(10000,'\n'); cout << "Enter integer: "; }
    cout << "Chemistry marks (0-100): ";
    while (!(cin >> s.chemistry)) { cin.clear(); cin.ignore(10000,'\n'); cout << "Enter integer: "; }
    cout << "Maths marks (0-100): ";
    while (!(cin >> s.maths)) { cin.clear(); cin.ignore(10000,'\n'); cout << "Enter integer: "; }

    vector<Student> list = loadAllStudents();
    // check duplicate roll nos.
    for (auto &st : list) {
        if (st.roll == s.roll) {
            cout << RED << "\nA student with this roll already exists. Use update instead.\n" << RESET;
            system("pause");
            return;
        }
    }
    list.push_back(s);
    saveAllStudents(list);
    cout << GREEN << "\nStudent added successfully.\n" << RESET;
    Sleep(700);
}

void viewData()
{
    system("cls");
    vector<Student> list = loadAllStudents();
    cout << MAGENTA << "\n| Roll | Name | College | Branch | Attendance | Phys | Chem | Maths |\n" << RESET;
    cout << string(100, '-') << "\n";
    for (auto &st : list) {
        cout << setw(5) << st.roll << " | "
             << setw(18) << left << st.name << right << " | "
             << setw(10) << st.college << " | "
             << setw(8) << st.branch << " | "
             << setw(9) << st.attendance << " | "
             << setw(4) << st.physics << " | "
             << setw(4) << st.chemistry << " | "
             << setw(4) << st.maths << "\n";
    }
    cout << string(100, '-') << "\n";
    system("pause");
}

void updateData()
{
    system("cls");
    cout << YELLOW << "\nUpdate Student Record\n" << RESET;
    cout << "Enter roll number to update: ";
    int r; if (!(cin >> r)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    vector<Student> list = loadAllStudents();
    bool found = false;
    for (auto &s : list) {
        if (s.roll == r) {
            found = true;
            cin.ignore();
            cout << "Updating student: " << s.name << " (" << s.roll << ")\n";
            cout << "Leave input blank (press Enter) to keep current value.\n";

            cout << "New name (current: " << s.name << "): ";
            string tmp; getline(cin, tmp);
            if (!tmp.empty()) s.name = tmp;

            cout << "New college (current: " << s.college << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.college = tmp;

            cout << "New branch (current: " << s.branch << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.branch = tmp;

            cout << "New attendance (current: " << s.attendance << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.attendance = tmp;

            cout << "New Physics marks (current: " << s.physics << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.physics = stoi(tmp);

            cout << "New Chemistry marks (current: " << s.chemistry << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.chemistry = stoi(tmp);

            cout << "New Maths marks (current: " << s.maths << "): ";
            getline(cin, tmp); if (!tmp.empty()) s.maths = stoi(tmp);

            saveAllStudents(list);
            cout << GREEN << "Record updated.\n" << RESET;
            system("pause");
            break;
        }
    }
    if (!found) {
        cout << RED << "Roll not found.\n" << RESET;
        system("pause");
    }
}

void deleteData()
{
    system("cls");
    cout << YELLOW << "\nDelete Student Record\n" << RESET;
    cout << "Enter roll number to delete: ";
    int rollnum; if (!(cin >> rollnum)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    vector<Student> list = loadAllStudents();
    size_t before = list.size();
    list.erase(remove_if(list.begin(), list.end(), [&](const Student &s){ return s.roll == rollnum; }), list.end());
    if (list.size() < before) {
        saveAllStudents(list);
        cout << GREEN << "Record deleted.\n" << RESET;
    } else {
        cout << RED << "Record not found.\n" << RESET;
    }
    system("pause");
}

void searchData()
{
    system("cls");
    cout << CYAN << "\nSearch Records\n" << RESET;
    cout << "1. By Roll\n2. By Name (partial)\n3. By Branch\nChoice: ";
    int ch; if (!(cin >> ch)) { cin.clear(); cin.ignore(10000,'\n'); return; }
    cin.ignore();
    vector<Student> list = loadAllStudents();
    bool found = false;
    if (ch == 1) {
        cout << "Enter roll: "; int r; cin >> r;
        for (auto &s : list) {
            if (s.roll == r) {
                cout << GREEN << "\nFound: " << s.roll << " - " << s.name << " | Branch: " << s.branch << " | Attendance: " << s.attendance << "\n" << RESET;
                found = true;
                break;
            }
        }
    } else if (ch == 2) {
        cout << "Enter name or partial: ";
        string q; getline(cin, q);
        string qlow = q; transform(qlow.begin(), qlow.end(), qlow.begin(), ::tolower);
        for (auto &s : list) {
            string nameLow = s.name; transform(nameLow.begin(), nameLow.end(), nameLow.begin(), ::tolower);
            if (nameLow.find(qlow) != string::npos) {
                cout << s.roll << " - " << s.name << " | Branch: " << s.branch << " | Attendance: " << s.attendance << "\n";
                found = true;
            }
        }
    } else if (ch == 3) {
        cout << "Enter branch: ";
        string q; getline(cin, q);
        for (auto &s : list) {
            if (s.branch == q) {
                cout << s.roll << " - " << s.name << " | Attendance: " << s.attendance << "\n";
                found = true;
            }
        }
    } else {
        cout << RED << "Invalid option\n" << RESET;
    }

    if (!found) cout << YELLOW << "No matching records found.\n" << RESET;
    system("pause");
}

void exportCSV()
{
    system("cls");
    vector<Student> list = loadAllStudents();
    ofstream fout(EXPORT_FILE, ios::trunc);
    fout << "Roll,Name,College,Branch,Attendance,Physics,Chemistry,Maths\n";
    for (auto &s : list) {
        fout << s.roll << ","
             << csvEscape(s.name) << ","
             << csvEscape(s.college) << ","
             << csvEscape(s.branch) << ","
             << csvEscape(s.attendance) << ","
             << s.physics << ","
             << s.chemistry << ","
             << s.maths << "\n";
    }
    fout.close();
    cout << GREEN << "Data exported to " << EXPORT_FILE << "\n" << RESET;
    system("pause");
}

void studentFunction()
{
    system("cls");
    cout << MAGENTA << "\n\t\t| Students Records (View Only) |\n\n" << RESET;
    viewData(); 
    cout << "\nPress enter to return to main menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

#endif 
