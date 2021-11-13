#include <cctype>
#include <codecvt>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/io.h>
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

class Programm {
    wchar_t                       grid[HOEHE][BREITE];
    wchar_t                       STANDARD;
    wstring                       woerter[100];
    unordered_set<pair<int, int>> avbl;
    int                           versuche = 0;

public:
    Programm();
    char  zufallsCharakter();
    bool  kannEinsetzen(const wchar_t* word, point start, richtung d);
    void  wortEinsetzen(const wchar_t* word);
    void  loeschen();
    void  fuellen();
    void  ausgabe();
    point punktBewegen(point start, richtung d);
    void  dateiAufruf(char* filename);
    void  dateiAuslesen();
};

Programm::Programm()
{
    STANDARD = ' ';
}

char Programm::zufallsCharakter()
{
    if(DIFFICULTY == 0) {
        return 'A' + rand() % 26;
    }
    else if(DIFFICULTY == 1) {
        return ' ';
    }
    else if(DIFFICULTY == 2) {
        return 'A' + rand() % 26;
    }
}

void Programm::loeschen()
{
    for(int i = 0; i < HOEHE; i++) {
        for(int k = 0; k < BREITE; k++) {
            grid[i][k] = STANDARD;
            avbl.insert(make_pair(i, k));
        }
    }
}

bool Programm::kannEinsetzen(const wchar_t* word, point start, richtung d)
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

void Programm::fuellen()
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

void Programm::ausgabe()
{
    for(int i = 0; i < HOEHE; i++) {
        for(int k = 0; k < BREITE; k++) {
            if(k == 0) {
                std::wcout << "\t" << grid[i][k] << " ";
                //printf("\t%c ", grid[i][k]);
            }
            else if(k == BREITE - 1) {
                std::wcout << grid[i][k] << std::endl;
                //printf("%c\n", grid[i][k]);
            }
            else {
                std::wcout << grid[i][k] << " ";
                //printf("%c ", grid[i][k]);
            }
        }
    }
    std::wcout << versuche << std::endl;
}

point Programm::punktBewegen(point start, richtung d)
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

    if(np.i < -1 || np.i > HOEHE || np.k < -1 || np.k > BREITE) {
        throw "Out of Bounds";
    }
    return np;
}

void Programm::wortEinsetzen(const wchar_t* word)
{
    point    start;
    richtung d;
    int      ec = 0;
    do {
        ++ec;
        ++versuche;
        // probability < 0.0001%
        if(ec > 1000) {
            ausgabe();
            throw "limit";
        }
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
            d = richtung(rand() % 8);
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

void Programm::dateiAufruf(char* datei)
{
    wifstream woerterDatei(datei);
    wstring   wort;
    int       zeile = 0;
    if(woerterDatei.is_open()) {
        while(getline(woerterDatei, wort)) {
            woerter[zeile] = wort;
            std::wcout << wort << std::endl;
            ++zeile;
            if(wort.length() < 2 || wort.length() > BREITE) {
                throw "length/gridsize";
            }
        }
    }
}

void Programm::dateiAuslesen()
{
    int groesse = (int)(sizeof(woerter) / sizeof(woerter[0]));
    if(DIFFICULTY == 2) {
        for(int i = 0; i < 10; ++i) {
            std::cout << groesse;
            int rindex = rand() % groesse;
            wstring wordOriginal = woerter[rindex];
            wstring wordDanach   = L"?" + wordOriginal.substr(0, 2 + rand() % 3);
            std::wcout << L"-----------------" << (groesse - i) << std::endl;
            woerter[groesse - i - 1]   = wordDanach;
        }
    }
    for(int i = 0; i < groesse; i++) {
        wstring word = woerter[i];
        try {
            wortEinsetzen(word.c_str());
        }
        catch(const char*& e) {
            //loeschen();
            versuche = 0;
            std::wcout << "er rr" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    Programm gen;
    try {
        gen.dateiAufruf(argv[1]);
    }
    catch(const char* msg) {
        cout << msg << endl;
        exit(1);
    }
    gen.loeschen();
    gen.dateiAuslesen();
    gen.fuellen();
    gen.ausgabe();
    return 0;
}
