#include <algorithm>
#include <iostream>
#include <vector>

void print_shifts(const std::vector<int>& move_possible, const std::vector<std::vector<int>>& shift, int cars_amount, char first_car)
{
    for(int i = 0; i < cars_amount; ++i) {
        std::cout << static_cast<char>(first_car + i) << "\t" << move_possible[i] << ":\t";
        for(int y = 0; y < cars_amount; ++y) {
            std::cout << shift[i][y] << " ";
        }
        std::cout << std::endl;
    }
}

// merge source into target
void update_row_dir(const std::vector<int>& source, std::vector<int>& target)
{
    for(int i = 0; i < target.size(); ++i) {
        target[i] = std::max(target[i], source[i]);
    }
}
// merge reduced source into target
void update_row_red(const std::vector<int>& source, std::vector<int>& target)
{
    for(int i = 0; i < target.size(); ++i) {
        target[i] = std::max(target[i], source[i] - 1);
    }
}

void load_possible_moves(const std::vector<std::pair<int, char>>& cross_cars, int cars_amount, std::vector<std::vector<int>>& shift, std::vector<int>& move_possible, bool go_left)
{
    int start = go_left ? 0 : cross_cars.size() - 1;
    int end   = go_left ? cross_cars.size() : -1;
    for(int c = start; c != end; go_left ? ++c : --c) {
        int first, second, next, next_but_one;
        if(go_left) {
            // location of cross_car
            // this pace gets primarily moved
            first = cross_cars[c].first;
            // secondary move
            second = cross_cars[c].first + 1;
            // places left or right of cross_car
            next         = first - 1;
            next_but_one = first - 2;
        }
        else {
            first        = cross_cars[c].first + 1;
            second       = cross_cars[c].first;
            next         = first + 1;
            next_but_one = first + 2;
        }
        // check if can be moved twice
        if(next_but_one >= 0 &&
           next_but_one < cars_amount &&
           move_possible[next] == 2 &&
           move_possible[next_but_one] >= 1) {
            // can be moved twice
            move_possible[first]  = 2;
            move_possible[second] = 2;
            // this car obviously has to be moved
            shift[first][first]   = 2;
            shift[first][second]  = 2;
            shift[second][first]  = 2;
            shift[second][second] = 2;

            // this one always has to be moved two paces
            update_row_dir(shift[next], shift[first]);
            update_row_dir(shift[next], shift[second]);
            // only reduce when one too much
            if(move_possible[next_but_one] == 2) {
                // right one only has to move once
                // only half movement required
                update_row_red(shift[next_but_one], shift[first]);
                update_row_red(shift[next_but_one], shift[second]);
            }
            else {
                // full movement required
                update_row_dir(shift[next_but_one], shift[first]);
                update_row_dir(shift[next_but_one], shift[second]);
            }
        }
        // can be moved once at least?
        else if(next >= 0 &&
                next < cars_amount &&
                move_possible[next] >= 1) {
            // can be moved once
            move_possible[first]  = 1;
            move_possible[second] = 1;

            // obviously has to be moved once
            shift[first][first]   = 1;
            shift[first][second]  = 1;
            shift[second][first]  = 1;
            shift[second][second] = 1;

            if(move_possible[next] == 2) {
                update_row_red(shift[next], shift[first]);
                update_row_red(shift[next], shift[second]);
            }
            else {
                update_row_dir(shift[next], shift[first]);
                update_row_dir(shift[next], shift[second]);
            }
        }
        // can't be moved
        else {
            move_possible[first]  = 0;
            move_possible[second] = 0;
        }
    }
}

// when is_first -> car has to be moved twice
// return true when can be moved in that direction
bool get_required_shifts(int moves_possible, std::vector<int>& shift, bool is_first, std::vector<int>& shift_req)
{
    // can be moved once and only have to move once <- i is second part of cross car
    if(moves_possible == 1 && !is_first) {
        update_row_dir(shift, shift_req);
        return true;
    }
    // can be moved twice but only has to be moved once
    else if(moves_possible == 2 && !is_first) {
        update_row_red(shift, shift_req);
        return true;
    }
    // can be moved twice and has to be moved twice
    else if(moves_possible == 2 && is_first) {
        update_row_dir(shift, shift_req);
        return true;
    }
    return false;
}

bool better_shift(const std::vector<int>& a, const std::vector<int>& b)
{
    int count_a {0}, count_b {0};
    int moves_a {0}, moves_b {0};
    for(int i = 0; i < a.size(); ++i) {
        if(a[i]) {
            ++count_a;
            moves_a += a[i];
        }
        if(b[i]) {
            ++count_b;
            moves_b += b[i];
        }
    }
    if(count_a < count_b)
        return true;
    if(count_a > count_b)
        return false;
    if(moves_a < moves_b)
        return true;
    if(moves_a > moves_b)
        return false;
    return true;
}

int main()
{
    // read input
    char first_car, last_car;
    std::cin >> first_car >> last_car;
    int cars_amount = last_car - first_car + 1;
    // turns location into car name
    // name, left part of cross car?
    std::vector<std::pair<char, bool>> cross_car_lookup(cars_amount);

    int cross_car_amount;
    std::cin >> cross_car_amount;
    // location, name
    std::vector<std::pair<int, char>> cross_cars(cross_car_amount);
    for(int i = 0; i < cross_car_amount; ++i) {
        // paces of car
        int  left, right;
        char name;
        std::cin >> name >> left;
        right                   = left + 1;
        cross_cars[i].first     = left;
        cross_cars[i].second    = name;
        cross_car_lookup[left]  = {name, true};
        cross_car_lookup[right] = {name, false};
    }
    std::sort(cross_cars.begin(), cross_cars.end());

    // 2 -> can be moved two paces
    // 1 -> can only be moved once
    // 0 -> unable to be moved at all
    // initially all cars can be moved as far as possible
    std::vector<int> move_left_possible(cars_amount, 2);
    std::vector<int> move_right_possible(cars_amount, 2);

    // 0 -> doesn't have to be moved
    // 1 -> has to be moved once
    // 2 -> has to be moved twice
    // initially no cars have to be moved
    std::vector<std::vector<int>> left_shift(cars_amount, std::vector<int>(cars_amount, 0));
    std::vector<std::vector<int>> right_shift(cars_amount, std::vector<int>(cars_amount, 0));

    load_possible_moves(cross_cars, cars_amount, left_shift, move_left_possible, true);
    load_possible_moves(cross_cars, cars_amount, right_shift, move_right_possible, false);

    // std::cout << std::endl;
    // std::cout << "left:" << std::endl;
    // print_shifts(move_left_possible, left_shift, cars_amount, first_car);
    // std::cout << std::endl;
    // std::cout << "right:" << std::endl;
    // print_shifts(move_right_possible, right_shift, cars_amount, first_car);

    // use produced tables
    for(int i = 0; i < cars_amount; ++i) {
        // move left
        std::vector<int> left_shift_req(cars_amount);
        bool             left_ok = get_required_shifts(move_left_possible[i], left_shift[i], cross_car_lookup[i].second, left_shift_req);
        // move right
        std::vector<int> right_shift_req(cars_amount);
        bool             right_ok = get_required_shifts(move_right_possible[i], right_shift[i], !cross_car_lookup[i].second, right_shift_req);

        // std::cout << std::endl;
        // for(auto e: left_shift_req) {
        //     std::cout << e << " ";
        // }
        // std::cout << std::endl;
        // for(auto e: right_shift_req) {
        //     std::cout << e << " ";
        // }
        // std::cout << std::endl;

        std::cout << static_cast<char>(first_car + i) << ": ";
        // select better move
        // either left is better than right or right is impossible
        if(left_ok && (better_shift(left_shift_req, right_shift_req) || !right_ok)) {
            for(int x = 0; x < left_shift_req.size(); ++x)
                if(left_shift_req[x]) {
                    std::cout << cross_car_lookup[x].first << " " << left_shift_req[x] << " links, ";
                    // move to pace after current car
                    ++x;
                }
        }
        else if(right_ok) {
            // go the other way around <- now moving right
            for(int x = right_shift_req.size() - 1; x; --x)
                if(right_shift_req[x]) {
                    std::cout << cross_car_lookup[x].first << " " << right_shift_req[x] << " rechts, ";
                    --x;
                }
        }
        else
            std::cout << "unmoeglich";
        std::cout << std::endl;
    }
}
