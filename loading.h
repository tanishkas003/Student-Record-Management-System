#ifndef LOADING_H
#define LOADING_H

#include <iostream>
#include <Windows.h>
using namespace std;

inline void loadingBar() {
    system("cls");
    cout << "\n\n\tLoading ";
    for (int i = 0; i < 20; ++i) {
        cout << ".";
        Sleep(80);
    }
    cout << "\n";
    Sleep(200);
}

#endif 
