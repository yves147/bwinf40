#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

int SCHWIERIGKEIT = 0;
bool fehler = false;

// Funktion zur zufälligen Selektierung aus einen Vektor
template<typename S>
auto select_random(const S& s, size_t n)
{
    auto it = std::begin(s);
    std::advance(it, n);
    return it;
}

// Funktion zum Vergleichen von zwei wstring
struct compare_length {
    bool operator()(const std::wstring& l, const std::wstring& r) const
    {
        return l.size() > r.size();
    }
};

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

// Ein Punkt auf den Feld
struct point {
    // i = Y-Koordinate
    // k = X-Koordinate
    int i, k;
};

wchar_t                       STANDARD = ' ';
wstring                       woerter[100];
// Verfügbare Punkte
unordered_set<pair<int, int>> verfuegbar;
// Versuche, die benötigt wurden
int                           versuche = 0;

// Ausgabe des 2D-Vektors
void ausgabe(const vector<vector<wchar_t>>& feld)
{
    std::wcout << std::endl;
    for(int i = 0; i < feld.size(); ++i) {
        for(int k = 0; k < feld[i].size(); ++k) {
            if(k == 0) {
                std::wcout << "" << feld[i][k] << " ";
            }
            else if(k == feld[i].size() - 1) {
                std::wcout << feld[i][k] << std::endl;
            }
            else {
                std::wcout << feld[i][k] << " ";
            }
        }
    }
}

// Abhängig von der Schwierigkeit gibt diese Funktion einen zufälligen
// Buchstaben zurück. Wenn ein schweres Worträtsel generiert werden soll,
// werden auch kleine Buchstaben mit einer Chance von 50% zurückgegeben.
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

// Diese Funktion ändert einen Punkt np abhängig von der Richtung rt auf
// einen Feld feld so, das der neue Punkt, dem nächsten Schritt
// in die angegebene Richtung entspricht.
// Bsp.: (0,0) wird zu (0,1) mit der Richtung richtung::RECHTS
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

// Diese Methode setzt alle Werte aus dem 2D-Vektor auf den Standard-Wert
// und sagt zudem, dass alle Werte wieder besetzt werden können.
void loeschen(vector<vector<wchar_t>>& feld)
{
    for(int i = 0; i < feld.size(); i++) {
        for(int k = 0; k < feld[i].size(); k++) {
            feld[i][k] = STANDARD;
            verfuegbar.insert(make_pair(i, k));
        }
    }
}

// Diese Methode überprüft, ob ein Wort in eine Richtung, von einen gegebenen Startpunkt
// aus, eingesetzt werden kann.
bool kannEinsetzen(vector<vector<wchar_t>>& feld, const wchar_t* wort, point start, richtung d)
{
    int   i = 0, l = (int)std::char_traits<wchar_t>::length(wort);
    point np = start;
    while(i < l) {
        if(np.i < 0 || np.i >= feld.size() || np.k < 0 || np.k >= feld[0].size()) {
            return false;
        }
        if(feld[np.i][np.k] == STANDARD || feld[np.i][np.k] == wort[i]) {
            punktBewegen(feld, d, np);
            i++;
        }
        else {
            return false;
        }
    }
    return true;
}

// Diese Methode füllt alle noch nicht belegten Punkte mit Buchstaben
// aus zufallsCharakter aus
void fuellen(vector<vector<wchar_t>>& feld)
{
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
    int      laenge = (int)std::char_traits<wchar_t>::length(wort);
    point    start;
    richtung rt;
    do {
        ++versuche;
        pair<int, int> n;
        if(laenge == feld[0].size()) {
            // Fügt alle Punkte, die ganz links im Feld sind, in einen
            // eigenen Vektor hinzu
            vector<pair<int, int>> t;
            for(int hi = 0; hi < feld[0].size(); ++hi) {
                pair<int, int> j   = std::make_pair<int, int>((int)hi, 0);
                auto           sit = verfuegbar.find(j);
                if(sit != verfuegbar.end()) {
                    t.push_back(*sit);
                }
            }
            // zufälliger Punkt aus denen, die ganz links im Feld sind
            auto r = rand() % t.size();
            n      = *select_random(t, r);
            rt      = richtung::RECHTS;
        }
        else if (laenge > feld[0].size() && laenge > feld.size()) {
            // Fügt alle Punkte, die ganz oben im Feld sind, in einen
            // eigenen Vektor hinzu
            vector<pair<int, int>> t;
            for(int hi = 0; hi < feld.size(); ++hi) {
                pair<int, int> j   = std::make_pair<int, int>(0, (int)hi);
                auto           sit = verfuegbar.find(j);
                if(sit != verfuegbar.end()) {
                    t.push_back(*sit);
                }
            }
            // zufälliger Punkt aus denen, die ganz oben im Feld sind
            auto r = rand() % t.size();
            n      = *select_random(t, r);
            rt      = richtung::RUNTER;
        }
        else {
            // Falls es sich um ein Wort mit normaler Länge handelt
            // wird ein zufälliger Punkt aus den verfügbaren ausgewählt
            auto r = rand() % verfuegbar.size();
            n      = *select_random(verfuegbar, r);
            // Je nach Schwierigkeit sind verschiedene zufällige Richtungen möglich
            switch(SCHWIERIGKEIT) {
            case 0:
                rt = richtung(rand() % 3);
                break;
            case 1:
                rt = richtung(rand() % 6);
                break;
            case 2:
                rt = richtung(rand() % 8);
            }
        }
        start.i = n.first;
        start.k = n.second;
    // falls diese Kombination nicht möglich ist,
    // finde einen neuen Startpunkt
    } while(!kannEinsetzen(feld, wort, start, rt));

    // Wenn das Wort eingesetzt werden kann,
    // setze es ein
    int   i  = 0;
    point np = start;
    while(i < laenge) {
        verfuegbar.erase(make_pair(np.i, np.k));
        // Falls das Rätsel schwer sein soll,
        // setze jeweils 50% Groß- und Kleinbuchstaben ein
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
        punktBewegen(feld, rt, np);
        i++;
    }
}

// Eingabe
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
        ++i;
    }

    return tdv;
}

// Diese Methode generiert neue Teilwörter, die zu dem woerter-Array hinzugefügt
// werden 
void extraWoerter(int groesse)
{
    for(int i = 0; i < 10; ++i) {
        int     rindex           = rand() % groesse;
        wstring wordOriginal     = woerter[rindex];
        wstring wordDanach       = wordOriginal.substr(0, std::min(5 + rand() % 3, (int)wordOriginal.size() - 1));
        woerter[groesse - i - 1] = wordDanach;
    }
}

// Diese Methode versucht alle Wörter zu einen Feld hinzuzufügen
// und ruft dafür die oben beschriebenen Methoden auf 
void kombinationFinden(vector<vector<wchar_t>>& feld)
{
    int groesse = (int)(sizeof(woerter) / sizeof(woerter[0]));
    // Nach der Einlese, generiere ggf. Extra-Wörter
    if(SCHWIERIGKEIT == 2) {
        extraWoerter(groesse);
    }
    // Für jedes Wort: Versuche es in das Feld hinzuzufügen.
    for(int i = 0; i < groesse; i++) {
        wstring wort = woerter[i];
        try {
            // Wenn ein Wort zu groß ist, zeige einen Fehler
            if(wort.size() > feld.size() && wort.size() > feld[0].size()) {
                fehler = true;
                throw "Keine Loesung";
            }
            wortEinsetzen(feld, wort.c_str());
        }
        catch(const char*& e) {
            versuche = 0;
            std::cout << e << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    // Seed für rand()-Aufruf
    srand(time(NULL));
    // Parameter
    int s;
    if(argc < 2) {
        std::wcout << L"Bitte geben Sie einen Parameter fuer die Schwierigkeit an (0 = leicht, 1 = mittel, 2 = schwer)" << std::endl;
        s = 0;
    }
    else {
        s = std::stoi(argv[1]);
    }
    if(s < 0 || s > 2) {
        std::wcout << L"Falscher Parameter" << std::endl;
        return 0;
    }
    else {
        SCHWIERIGKEIT = s;
    }
    // Methoden werden hintereinander aufgerufen
    auto feld = eingabe();
    // Wörter werden sortiert
    std::sort(std::begin(woerter), std::end(woerter), compare_length());
    // Feld wird geleert
    loeschen(feld);
    // Wörter werden zum Feld hinzugefügt
    kombinationFinden(feld);
    if(fehler == true){
        return 0;
    };
    // Restliche Punkte werden befüllt
    fuellen(feld);
    // Ausgabe
    ausgabe(feld);
    return 0;
}
