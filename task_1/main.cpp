#include <algorithm>
#include <iostream>
#include <vector>

// merge source into target
void update_row_dir(std::vector<int>& target, const std::vector<int>& source)
{
    for(int i = 0; i < target.size(); ++i) {
        target[i] = std::max(target[i], source[i]);
    }
}
// merge reduced source into target
void update_row_red(std::vector<int>& target, const std::vector<int>& source)
{
    for(int i = 0; i < target.size(); ++i) {
        target[i] = std::max(target[i], source[i] - 1);
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
    std::vector<int> possible_moves(cars_amount, 2);

    // 0 -> doesn't have to be moved
    // 1 -> has to be moved once
    // 2 -> has to be moved twice
    // initially no cars have to be moved
    std::vector<std::vector<int>> left_shift(cars_amount, std::vector<int>(cars_amount, 0));

    for(auto q_car: q_cars) {
        // location of q_car
        int first  = q_car.first;
        int second = first + 1;
        // places left of q_car
        int left1 = first - 1;
        int left2 = first - 2;
        // check if can be moved twice
        if(left2 >= 0 && possible_moves[left1] == 2 && possible_moves[left2] >= 1) {
            update_row_dir(left_shift[first], left_shift[left1]);
            if(possible_moves[left2] == 2) {
                // right one only has to move once
                // only half movement required
                update_row_red(left_shift[first], left_shift[left1]);
                update_row_red(left_shift[second], left_shift[left1]);
            }
            else {
                // full movement required
                update_row_dir(left_shift[first], left_shift[left2]);
                update_row_dir(left_shift[second], left_shift[left2]);
            }
            possible_moves[first]  = 2;
            possible_moves[second] = 2;
        }
        // can be moved once at least?
        else if(left1 >= 0 && possible_moves[left1] >= 1) {
            if(possible_moves[left1] == 2) {
                update_row_red(left_shift[first], left_shift[left1]);
                update_row_red(left_shift[second], left_shift[left1]);
            }
            else {
                update_row_dir(left_shift[first], left_shift[left1]);
                update_row_dir(left_shift[second], left_shift[left1]);
            }
            possible_moves[first]      = 1;
            possible_moves[second]     = 1;
            left_shift[second][first]  = 1;
            left_shift[second][second] = 1;
        }
        // can't be moved
        else {
            possible_moves[first]  = 0;
            possible_moves[second] = 0;
        }
    }
    std::cout << std::endl;
    for(int i = 0; i < cars_amount; ++i) {
        std::cout << i << "\t" << possible_moves[i] << ":\t";
        for(int y = 0; y < cars_amount; ++y) {
            std::cout << left_shift[i][y] << " ";
        }
        std::cout << std::endl;
    }
}
