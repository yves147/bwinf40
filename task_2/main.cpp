#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#define inf std::numeric_limits<float>::max() / 2

constexpr int DAYS    = 5;
constexpr int DAY_LEN = 6 * 60;

int main()
{
    int n, t;
    std::cin >> n >> t;
    // location, rating
    std::vector<std::pair<int, float>> hotels(n + 2);
    // first "hotel" is start
    hotels[0] = {0, inf};
    // last "hotel" is end
    hotels[n + 1] = {t, inf};
    for(int i = 1; i <= n; ++i) {
        std::cin >> hotels[i].first >> hotels[i].second;
    }

    // [x][y] -> min rating till hotel x requiring y days to get to
    // inf -> impossible
    std::vector<std::vector<float>> min_ratings(n + 2, std::vector<float>(DAYS + 1, -1));
    std::vector<std::vector<int>>   last_hotel(n + 2, std::vector<int>(DAYS + 1, -1));
    min_ratings[0][0] = inf;

    for(auto cur = hotels.begin() + 1; cur != hotels.end(); ++cur) {
        int cur_idx = cur - hotels.begin();
        for(auto prev = std::lower_bound(hotels.begin(), hotels.end(), std::make_pair(cur->first - DAY_LEN, .0f)); prev != cur; ++prev) {
            int prev_idx = prev - hotels.begin();
            for(int prev_day = 0; prev_day < DAYS; ++prev_day) {
                // invalid won't be propagated
                if(min_ratings[prev_idx][prev_day] == -1)
                    continue;
                int   cur_day = prev_day + 1;
                float new_min = std::min(min_ratings[prev_idx][prev_day], cur->second);
                // is better?
                if(new_min > min_ratings[cur_idx][cur_day]) {
                    min_ratings[cur_idx][cur_day] = new_min;
                    last_hotel[cur_idx][cur_day]  = prev_idx;
                }
            }
        }
    }

    // for(auto y: min_ratings) {
    //     for(auto x: y) {
    //         std::cout << x << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int   best_day {-1};
    float best_min_rating {-1};
    for(int i = 0; i < DAYS + 1; ++i) {
        if(min_ratings[n + 1][i] > best_min_rating) {
            best_min_rating = min_ratings[n + 1][i];
            best_day        = i;
        }
    }
    if(best_day == -1) {
        std::cout << "impossible" << std::endl;
    }
    else {
        std::cout << "possible with min rating: " << best_min_rating << std::endl;
        // reconstruct path
        std::vector<int> path;
        int              cur_idx = last_hotel[n + 1][best_day];
        for(int cur_day = best_day - 1; cur_day; --cur_day) {
            path.push_back(cur_idx);
            cur_idx = last_hotel[cur_idx][cur_day];
        }
        std::reverse(path.begin(), path.end());
        std::cout << "these hotels should be used (displayed as 'index:location-rating'): ";
        for(auto idx: path) {
            std::cout << idx << ":" << hotels[idx].first << "-" << hotels[idx].second << "\t";
        }
        std::cout << std::endl;
    }
}
