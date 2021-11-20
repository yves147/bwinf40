#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdlib.h>
#include <string>
#include <unordered_set>

using namespace std;

const int HOEHE  = 42;
const int BREITE = 30;

const int DIFFICULTY = 2;

template<typename S>
auto select_random(const S& s, size_t n)
{
    auto it = std::begin(s);
    std::advance(it, n);
    return it;
}

namespace std {
template<>
struct hash<std::pair<int, int>> {
    inline size_t operator()(const std::pair<int, int>& v) const
    {
        std::hash<int> int_hasher;
        return int_hasher(v.first) ^ int_hasher(v.second);
    }
};
} // namespace std

enum richtung {
    HOCH,
    RUNTER,
    RECHTS,
    LINKS,
    RUNTER_LINKS,
    RUNTER_RECHTS,
    HOCH_LINKS,
    HOCH_RECHTS
};

struct point {
    int i, k;
};

wchar_t                       grid[HOEHE][BREITE];
wchar_t                       STANDARD = ' ';
wstring                       woerter[100];
unordered_set<pair<int, int>> avbl;
int                           versuche = 0;

void ausgabe()
{
    for(int i = 0; i < HOEHE; i++) {
        for(int k = 0; k < BREITE; k++) {
            if(k == 0) {
                std::wcout << "\t" << grid[i][k] << " ";
            }
            else if(k == BREITE - 1) {
                std::wcout << grid[i][k] << std::endl;
            }
            else {
                std::wcout << grid[i][k] << " ";
            }
        }
    }
    std::wcout << versuche << std::endl;
}

wchar_t zufallsCharakter()
{
    if(DIFFICULTY < 2) {
        return 'A' + rand() % 26;
    }
    else {
        if(rand() % 2 == 0) {
            return 'A' + rand() % 26;
        }
        else {
            return 'a' + rand() % 26;
        }
    }
}

point punktBewegen(point start, richtung d)
{
    int   i = start.i;
    int   k = start.k;
    point np;
    switch(d) {
    case HOCH:
        np.i = i - 1;
        np.k = k;
        break;
    case RUNTER:
        np.i = i + 1;
        np.k = k;
        break;
    case LINKS:
        np.i = i;
        np.k = k - 1;
        break;
    case RECHTS:
        np.i = i;
        np.k = k + 1;
        break;
    case HOCH_LINKS:
        np.i = i - 1;
        np.k = k - 1;
        break;
    case HOCH_RECHTS:
        np.i = i - 1;
        np.k = k + 1;
        break;
    case RUNTER_LINKS:
        np.i = i + 1;
        np.k = k - 1;
        break;
    case RUNTER_RECHTS:
        np.i = i + 1;
        np.k = k + 1;
        break;
    default:
        np.i = i;
        np.k = k;
        break;
    }

    if(np.i < -1 || np.k < -1 || np.i > HOEHE || np.k > BREITE) {
        throw "Ausserhalb";
    }
    return np;
}

void loeschen()
{
    for(int i = 0; i < HOEHE; i++) {
        for(int k = 0; k < BREITE; k++) {
            grid[i][k] = STANDARD;
            avbl.insert(make_pair(i, k));
        }
    }
}

bool kannEinsetzen(const wchar_t* word, point start, richtung d)
{
    int   i  = 0;
    point np = start;
    while(i < (int)std::char_traits<wchar_t>::length(word)) {
        try {
            if(grid[np.i][np.k] == STANDARD) {
                np = punktBewegen(np, d);
                i++;
            }
            else {
                return false;
            }
        }
        catch(const char* msg) {
            return false;
        }
    }
    return true;
}

void fuellen()
{
    ausgabe(); // nur zum testen
    for(int i = 0; i < HOEHE; i++) {
        for(int k = 0; k < BREITE; k++) {
            if(grid[i][k] == STANDARD) {
                grid[i][k] = zufallsCharakter();
            }
        }
    }
}

void wortEinsetzen(const wchar_t* word)
{
    point    start;
    richtung d;
    do {
        ++versuche;
        auto r  = rand() % avbl.size();
        auto n  = *select_random(avbl, r);
        start.i = n.first;
        start.k = n.second;
        switch(DIFFICULTY) {
        case 0:
            d = richtung(rand() % 3);
            break;
        case 1:
            d = richtung(rand() % 6);
            break;
        case 2:
            d = richtung(rand() % 9);
        }
    } while(!kannEinsetzen(word, start, d));

    int   i  = 0;
    point np = start;
    while(i < (int)std::char_traits<wchar_t>::length(word)) {
        avbl.erase(make_pair(np.i, np.k));
        grid[np.i][np.k] = (wchar_t)towupper(word[i]);
        np               = punktBewegen(np, d);
        i++;
    }
}

void dateiAufruf(char* datei)
{
    wifstream woerterDatei(datei);
    wstring   wort;
    int       zeile = 0;
    if(woerterDatei.is_open()) {
        while(getline(woerterDatei, wort)) {
            woerter[zeile] = wort;
            std::wcout << wort << std::endl;
            ++zeile;
            if(wort.length() < 2 || wort.length() > BREITE && wort.length() > HOEHE) {
                throw "length/gridsize";
            }
        }
    }
}

void extraWoerter(int groesse)
{
    for(int i = 0; i < 10; ++i) {
        std::cout << groesse;
        int     rindex           = rand() % groesse;
        wstring wordOriginal     = woerter[rindex];
        wstring wordDanach       = wordOriginal.substr(0, std::min(2 + rand() % 3, (int)wordOriginal.size() - 1));
        woerter[groesse - i - 1] = wordDanach;
    }
}

void dateiAuslesen()
{
    int groesse = (int)(sizeof(woerter) / sizeof(woerter[0]));
    if(DIFFICULTY == 2) {
        extraWoerter(groesse);
    };
    for(int i = 0; i < groesse; i++) {
        wstring word = woerter[i];
        try {
            wortEinsetzen(word.c_str());
        }
        catch(const char*& e) {
            versuche = 0;
            std::wcout << "er rr" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    try {
        dateiAufruf(argv[1]);
    }
    catch(const char* msg) {
        cout << msg << endl;
        exit(1);
    }
    loeschen();
    dateiAuslesen();
    fuellen();
    ausgabe();
    return 0;
}
