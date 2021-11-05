#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

int main()
{
    std::ifstream cin("examples/hotels1.txt");

    int n, t;

    std::cin >> n;
    std::cin >> t;

    // Entfernung & Bewertung
    std::vector<std::pair<int, float>> hotels(n);

    for(int i = 0; i < n; ++i) {
        std::cin >> hotels[i].first >> hotels[i].second;
    }

    std::vector<std::pair<int, std::vector<int>>> hotelMoeglicheZiele(n);
    for(int i = 0; i < n; ++i) {
        std::vector<int> mz;
        //std::vector<std::pair<int, float>> t = ;
        auto             upb = std::upper_bound(hotels.begin(), hotels.end(), std::make_pair(hotels[i].first + 6 * 60, std::numeric_limits<float>::max()));

        for(auto j = upb - 1; j >= hotels.begin() + i; --j) {
            mz.push_back(j - hotels.begin());
        }

        hotelMoeglicheZiele[i] = std::make_pair(i, mz);
    }

    for(int k = 0; k < hotelMoeglicheZiele.size(); ++k){
        std::cout << hotelMoeglicheZiele[k].first << " ";
        for(int j = 0; j < hotelMoeglicheZiele[k].second.size(); ++j) std::cout << hotelMoeglicheZiele[k].second[j] << "-";
        std::cout << std::endl;
    }
}