#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

int HOEHE   = 42;
int BREITE  = 30;
int WANZAHL = 100;

int SCHWIERIGKEIT = 2;

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

vector<vector<wchar_t>>       grid;
wchar_t                       STANDARD = ' ';
wstring                       woerter[100];
unordered_set<pair<int, int>> avbl;
int                           versuche = 0;

void ausgabe()
{
    for(int i = 0; i < grid.size(); i++) {
        for(int k = 0; k < grid[i].size(); k++) {
            if(k == 0) {
                std::wcout << "\t" << grid[i][k] << " ";
            }
            else if(k == grid[i].size() - 1) {
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
    if(SCHWIERIGKEIT < 2) {
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

void loeschen(vector<vector<wchar_t>> &feld)
{
    for(int i = 0; i < feld.size(); i++) {
        for(int k = 0; k < feld[i].size(); k++) {
            feld[i][k] = STANDARD;
            avbl.insert(make_pair(i, k));
        }
    }
    std::wcout << "avbl " << avbl.size() << std::endl;
}

bool kannEinsetzen(vector<vector<wchar_t>>& feld, const wchar_t* wort, point start, richtung d)
{
    int   i  = 0;
    point np = start;
    while(i < (int)std::char_traits<wchar_t>::length(wort)) {
        try {
            if(feld[np.i][np.k] == STANDARD) {
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

void fuellen(vector<vector<wchar_t>>& feld)
{
    ausgabe(); // nur zum testen
    for(int i = 0; i < feld.size(); i++) {
        for(int k = 0; k < feld[i].size(); k++) {
            if(feld[i][k] == STANDARD) {
                feld[i][k] = zufallsCharakter();
            }
        }
    }
}

void wortEinsetzen(vector<vector<wchar_t>>& feld, const wchar_t* wort)
{
    point    start;
    richtung d;
    do {
        ++versuche;
        auto r  = rand() % avbl.size();
        auto n  = *select_random(avbl, r);
        start.i = n.first;
        start.k = n.second;
        switch(SCHWIERIGKEIT) {
        case 0:
            d = richtung(rand() % 3);
            break;
        case 1:
            d = richtung(rand() % 6);
            break;
        case 2:
            d = richtung(rand() % 9);
        }
        std::wcout << L"h  " << wort << std::endl;
    } while(!kannEinsetzen(feld, wort, start, d));

    int   i  = 0;
    point np = start;
    while(i < (int)std::char_traits<wchar_t>::length(wort)) {
        avbl.erase(make_pair(np.i, np.k));
        feld[np.i][np.k] = (wchar_t)towupper(wort[i]);
        np               = punktBewegen(np, d);
        i++;
    }
}

vector<vector<wchar_t>> dateiAufruf(char* datei)
{
    /*wifstream woerterDatei(datei);
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
    }*/
    vector<vector<wchar_t>> tdv;
    std::wcin >> HOEHE >> BREITE;
    for(int h = 0; h < HOEHE; ++h) {
        vector<wchar_t> t;
        for(int b = 0; b < BREITE; ++b) {
            t.push_back(STANDARD);
        }
        std::wcout << t.size() << std::endl;
        tdv.push_back(t);
    }
    std::wcout << grid.size() << std::endl;
    std::wcin >> WANZAHL;

    int i = 0;
    while(i < WANZAHL) {
        std::wcin >> woerter[i];
        std::wcout << woerter[i] << std::endl;
        ++i;
    }

    return tdv;
}

void extraWoerter(int groesse)
{
    for(int i = 0; i < 10; ++i) {
        std::cout << groesse;
        int     rindex           = rand() % groesse;
        wstring wordOriginal     = woerter[rindex];
        wstring wordDanach       = wordOriginal.substr(0, std::min(3 + rand() % 3, (int)wordOriginal.size() - 1));
        woerter[groesse - i - 1] = wordDanach;
    }
}

void kombinationFinden(vector<vector<wchar_t>> &feld)
{
    int groesse = (int)(sizeof(woerter) / sizeof(woerter[0]));
    if(SCHWIERIGKEIT == 2) {
        extraWoerter(groesse);
    }
    for(int i = 0; i < groesse; i++) {
        wstring wort = woerter[i];
        try {
            wortEinsetzen(feld, wort.c_str());
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
    std::wcout << 0 << std::endl;
    vector<vector<wchar_t>> v = dateiAufruf(argv[1]);
    std::wcout << 1 << std::endl;
    loeschen(v);
    std::wcout << 2 << std::endl;
    kombinationFinden(v);
    std::wcout << 3 << std::endl;
    fuellen(v);
    std::wcout << 4 << std::endl;
    ausgabe();
    return 0;
}
