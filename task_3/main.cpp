#include <cctype>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

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

wchar_t                       STANDARD = ' ';
wstring                       woerter[100];
unordered_set<pair<int, int>> avbl;
int                           versuche = 0;

void ausgabe(const vector<vector<wchar_t>>& feld)
{
    std::wcout << std::endl;
    for(int i = 0; i < feld.size(); ++i) {
        for(int k = 0; k < feld[i].size(); ++k) {
            if(k == 0) {
                std::wcout << "\t" << feld[i][k] << " ";
            }
            else if(k == feld[i].size() - 1) {
                std::wcout << feld[i][k] << std::endl;
            }
            else {
                std::wcout << feld[i][k] << " ";
            }
        }
    }
    std::wcout << versuche << " Versuche";
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

void punktBewegen(vector<vector<wchar_t>>& feld, richtung rt, point& np)
{
    int i = np.i;
    int k = np.k;

    switch(rt) {
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
}

void loeschen(vector<vector<wchar_t>>& feld)
{
    for(int i = 0; i < feld.size(); i++) {
        for(int k = 0; k < feld[i].size(); k++) {
            feld[i][k] = STANDARD;
            avbl.insert(make_pair(i, k));
        }
    }
}

bool kannEinsetzen(vector<vector<wchar_t>>& feld, const wchar_t* wort, point start, richtung d)
{
    int   i  = 0;
    point np = start;
    while(i < (int)std::char_traits<wchar_t>::length(wort)) {
        if(feld[np.i][np.k] == STANDARD) {
            punktBewegen(feld, d, np);
            if(np.i < 0 || np.i >= feld.size() || np.k < 0 || np.k >= feld[0].size()) {
                return false;
            }
            i++;
        }
        else {
            return false;
        }
    }
    return true;
}

void fuellen(vector<vector<wchar_t>>& feld)
{
    ausgabe(feld);
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
        if(versuche > 5000)
            return;
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
            d = richtung(rand() % 8);
        }
    } while(!kannEinsetzen(feld, wort, start, d));

    int   i = 0, l = (int)std::char_traits<wchar_t>::length(wort);
    point np = start;
    while(i < l) {
        avbl.erase(make_pair(np.i, np.k));
        if(SCHWIERIGKEIT == 2) {
            if(rand() % 2 == 0) {
                feld[np.i][np.k] = (wchar_t)towupper(wort[i]);
            }
            else {
                feld[np.i][np.k] = (wchar_t)towlower(wort[i]);
            }
        }
        else {
            feld[np.i][np.k] = (wchar_t)towupper(wort[i]);
        }
        punktBewegen(feld, d, np);
        i++;
    }
}

vector<vector<wchar_t>> eingabe()
{
    vector<vector<wchar_t>> tdv;
    int                     H, B;
    std::wcin >> H >> B;
    for(int h = 0; h < H; ++h) {
        vector<wchar_t> t;
        for(int b = 0; b < B; ++b) {
            t.push_back(STANDARD);
        }
        tdv.push_back(t);
    }
    int w, i = 0;
    std::wcin >> w;
    while(i < w) {
        std::wcin >> woerter[i];
        //std::wcout << woerter[i] << std::endl;
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

void kombinationFinden(vector<vector<wchar_t>>& feld)
{
    int groesse = (int)(sizeof(woerter) / sizeof(woerter[0]));
    if(SCHWIERIGKEIT == 2) {
        extraWoerter(groesse);
    }
    for(int i = 0; i < groesse; i++) {
        wstring wort = woerter[i];
        try {
            wortEinsetzen(feld, wort.c_str());
            /*if(wiederholung == true){
                wiederholung = false;
                loeschen(feld);
                kombinationFinden(feld);
                break;
            }*/
        }
        catch(const char*& e) {
            versuche = 0;
            std::wcout << "Generiere neuen Versuch.." << e << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    auto feld = eingabe();
    loeschen(feld);
    kombinationFinden(feld);
    fuellen(feld);
    ausgabe(feld);
    return 0;
}
