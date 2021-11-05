#include <algorithm>
#include <iostream>
#include <vector>

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

void load_possible_moves(const std::vector<std::pair<int, char>>& q_cars, int cars_amount, std::vector<std::vector<int>>& shift, std::vector<int>& move_possible, bool go_left)
{
    for(auto q_car: q_cars) {
        int first, second, next, next_but_one;
        if(go_left) {
            // location of q_car
            first = q_car.first;
            // secondary move
            second = q_car.first + 1;
            // places left or right of q_car
            next         = second - 1;
            next_but_one = second - 2;
        }
        else {
            first        = q_car.first + 1;
            second       = q_car.first;
            next         = second + 1;
            next_but_one = second + 2;
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
        else if(next_but_one >= 0 &&
                next_but_one < cars_amount &&
                move_possible[next] >= 1) {
            // can be moved once
            move_possible[first]  = 1;
            move_possible[second] = 0;

            shift[first][first]  = 1;
            shift[first][second] = 1;
            // doesn't actually matter <- can't be moved anyways
            // shift[second][first]  = 0;
            // shift[second][second] = 0;

            if(move_possible[next] == 2) {
                update_row_red(shift[next], shift[first]);
                // doesn't matter
                // update_row_red(shift[next], shift[second]);
            }
            else {
                update_row_dir(shift[next], shift[first]);
                // doesn't matter
                // update_row_dir(shift[next], shift[second]);
            }
        }
        // can't be moved
        else {
            move_possible[first]  = 0;
            move_possible[second] = 0;
        }
    }
}

int main()
{
    // read input
    char first_car, last_car;
    std::cin >> first_car >> last_car;
    int cars_amount = last_car - first_car + 1;

    int q_cars_amount;
    std::cin >> q_cars_amount;
    // location, name
    std::vector<std::pair<int, char>> q_cars(q_cars_amount);
    for(int i = 0; i < q_cars_amount; ++i) {
        std::cin >> q_cars[i].second >> q_cars[i].first;
    }
    std::sort(q_cars.begin(), q_cars.end());

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

    load_possible_moves(q_cars, cars_amount, left_shift, move_left_possible, true);
    load_possible_moves(q_cars, cars_amount, right_shift, move_right_possible, false);

    std::cout << std::endl;
    std::cout << "left:" << std::endl;
    for(int i = 0; i < cars_amount; ++i) {
        std::cout << static_cast<char>(first_car + i) << "\t" << move_left_possible[i] << ":\t";
        for(int y = 0; y < cars_amount; ++y) {
            std::cout << left_shift[i][y] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "right:" << std::endl;
    for(int i = 0; i < cars_amount; ++i) {
        std::cout << static_cast<char>(first_car + i) << "\t" << move_right_possible[i] << ":\t";
        for(int y = 0; y < cars_amount; ++y) {
            std::cout << right_shift[i][y] << " ";
        }
        std::cout << std::endl;
    }
}
