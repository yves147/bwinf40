#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#define inf std::numeric_limits<float>::max() / 2

constexpr int DAYS    = 5;
constexpr int DAY_LEN = 6 * 60;

void populate_tables(
    const std::vector<std::pair<int, float>>& hotels,
    std::vector<std::vector<float>>&          min_ratings,
    std::vector<std::vector<int>>&            last_hotel)
{
    // the "first" (virtual) hotel never has the worst rating
    min_ratings[0][0] = inf;
    // go through all but "first" hotels
    for(auto cur = hotels.begin() + 1; cur != hotels.end(); ++cur) {
        int cur_idx = cur - hotels.begin();
        // first hotel `cur` can be reached from
        auto prev = std::lower_bound(hotels.begin(), hotels.end(),
                                     std::make_pair(cur->first - DAY_LEN, .0f));
        for(; prev != cur; ++prev) {
            int prev_idx = prev - hotels.begin();
            // go through past days when prev can be reached
            // call ride off after `DAYS` days
            for(int prev_day = 0, cur_day = 1; prev_day < DAYS; ++prev_day, ++cur_day) {
                // if the hotel `prev` can't be reached in `prev_day` days
                if(min_ratings[prev_idx][prev_day] == -1)
                    continue;
                float new_min = std::min(min_ratings[prev_idx][prev_day], cur->second);
                // update when better
                if(new_min > min_ratings[cur_idx][cur_day]) {
                    min_ratings[cur_idx][cur_day] = new_min;
                    last_hotel[cur_idx][cur_day]  = prev_idx;
                }
            }
        }
    }
}

int get_best_day(const std::vector<std::vector<float>>& min_ratings, int n)
{
    int   best_day {-1};
    float best_min_rating {-1};
    for(int i = 0; i < DAYS + 1; ++i) {
        if(min_ratings[n + 1][i] > best_min_rating) {
            best_min_rating = min_ratings[n + 1][i];
            best_day        = i;
        }
    }
    return best_day;
}

void construct_path(
    const std::vector<std::vector<int>>& last_hotel,
    int                                  best_day,
    int                                  n,
    std::vector<int>&                    path)
{
    int cur_idx = last_hotel[n + 1][best_day];
    for(int cur_day = best_day - 1; cur_day; --cur_day) {
        path.push_back(cur_idx);
        cur_idx = last_hotel[cur_idx][cur_day];
    }
    std::reverse(path.begin(), path.end());
}

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

    // min_ratings[x][y] -> min rating till hotel x requiring y days to get to
    // -1 -> impossible
    std::vector<std::vector<float>> min_ratings(n + 2, std::vector<float>(DAYS + 1, -1));
    std::vector<std::vector<int>>   last_hotel(n + 2, std::vector<int>(DAYS + 1, -1));

    populate_tables(hotels, min_ratings, last_hotel);

    int best_day = get_best_day(min_ratings, n);

    // reconstruct path
    std::vector<int> path;
    if(best_day != -1)
        construct_path(last_hotel, best_day, n, path);

    // print table
    std::cout << "   \t      \t  min rating at day:" << std::endl;
    std::cout << "idx\trating\t  0\t1\t2\t3\t4\t5" << std::endl;
    std::cout << "===\t======\t  =\t=\t=\t=\t=\t=" << std::endl;
    for(int hotel_id = 0; hotel_id < n + 2; ++hotel_id) {
        std::cout << hotel_id << "\t";
        if(hotels[hotel_id].second == inf)
            std::cout << "inf\t";
        else
            std::cout << hotels[hotel_id].second << "\t";
        if(std::find(path.begin(), path.end(), hotel_id) != path.end())
            std::cout << "+ ";
        else
            std::cout << "  ";
        for(float x: min_ratings[hotel_id]) {
            if(x == -1)
                std::cout << " \t";
            else if(x == inf)
                std::cout << "inf\t";
            else
                std::cout << x << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // print path
    if(best_day == -1) {
        std::cout << "impossible" << std::endl;
    }
    else {
        std::cout << "these hotels should be used with min rating " << min_ratings[n + 1][best_day] << ":" << std::endl;
        std::cout << "idx\tlocation\trating\tmin rating till here" << std::endl;
        std::cout << "===\t========\t======\t====================" << std::endl;
        for(int day = 1; day < DAYS; ++day) {
            int idx = path[day - 1];
            std::cout << idx << "\t" << hotels[idx].first << "\t\t" << hotels[idx].second << "\t" << min_ratings[idx][day] << std::endl;
        }
        std::cout << std::endl;
    }
}
