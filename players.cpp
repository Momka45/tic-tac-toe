#include "players.h"

Player::Player() {}

std::pair<int, int> Player::get_step(const Field &, bool) {}


std::pair<int, int> HumanPlayerClassical::get_step(const Field &fld, bool current_player) {
    auto field = fld.get_field();
    std::cout << "User №" << current_player + 1 << " make your step(example of step - a1): ";
    while (true) {
        std::string step;
        std::cin >> step;
        if (step.length() > 2 || !isdigit(step[1]) || !isalpha(step[0])) {
            std::cout << "Invalid step!" << std::endl;
            continue;
        } else {
            int y = step[0] - 'a';
            int x = step[1] - '0' - 1;
            if (y > 2 || x > 2 || field[x][y] != ' ') {
                std::cout << "Invalid step!" << std::endl;
                continue;
            }
            return std::make_pair(x, y);
        }
    }

}

std::pair<int, int> HumanPlayerNcurses::get_step(const Field &fld, bool current_player) {
    auto field = fld.get_field();
    clear();
    int yMax, xMax;
    getmaxx(stdscr);
    getmaxyx(stdscr, yMax, xMax);
    WINDOW *menuwin = newwin(7, 7, (yMax - 7) / 2, (xMax - 7) / 2);
    keypad(menuwin, true);
    refresh();
    wrefresh(menuwin);

    int option = 0;
    for(int i = 0; i < 3; i++)
        for(int j =0; j < 3; j++)
            if(field[i][j] == ' ') {
                option = i*3+j;
                break;
            }
    while (true) {
        box(menuwin, 0, 0);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (i * 3 + j == option) {
                    wattron(menuwin, A_STANDOUT);
                }
                mvwprintw(menuwin, i * 2 + 1, j * 2 + 1, "%c", field[i][j]);
                if (i * 3 + j == option) {
                    wattroff(menuwin, A_STANDOUT);
                }
                if (j != 2) mvwprintw(menuwin, i * 2 + 1, (j + 1) * 2, "|");
            }
            if (i != 2) mvwprintw(menuwin, (i + 1) * 2, 1, "-----");
        }
        wrefresh(menuwin);
        int tmp = option;
        switch (wgetch(menuwin)) {
            case KEY_UP:
                option = (6 + option) % 9;
                for (int j = 0; j < 3 && field[option / 3][option % 3] != ' '; j++) {
                    for (int i = 0; i < 3 && field[option / 3][option % 3] != ' '; i++) {
                        option = (6 + option) % 9;
                    }
                    if (field[option / 3][option % 3] == ' ' && (option / 3 != tmp / 3 || tmp % 3 != option % 3)) break;
                    option = ++option % 9;
                }
                if (field[option / 3][option % 3] != ' ') option = tmp;
                break;
            case KEY_DOWN:
                option = (3 + option) % 9;
                for (int j = 0; j < 3 && field[option / 3][option % 3] != ' '; j++) {
                    for (int i = 0; i < 3 && field[option / 3][option % 3] != ' '; i++) {
                        option = (3 + option) % 9;
                    }
                    if (field[option / 3][option % 3] == ' ' && (option / 3 != tmp / 3 || tmp % 3 != option % 3)) break;
                    option = ++option % 9;
                }
                if (field[option / 3][option % 3] != ' ') option = tmp;
                break;
            case KEY_LEFT:
                option = (9 + --option) % 9;
                for (int j = 0; j < 3 && field[option / 3][option % 3] != ' '; j++) {
                    for (int i = 0; i < 3 && field[option / 3][option % 3] != ' '; i++) {
                        option = (9 + --option) % 9;
                    }
                    if (field[option / 3][option % 3] == ' ' && (option / 3 != tmp / 3 || tmp % 3 != option % 3)) break;
                    option = (3 +option) % 9;
                }
                if (field[option / 3][option % 3] != ' ') option = tmp;
                break;
            case KEY_RIGHT:
                option = ++option % 9;
                for (int j = 0; j < 3 && field[option / 3][option % 3] != ' '; j++) {
                    for (int i = 0; i < 3 && field[option / 3][option % 3] != ' '; i++) {
                        option = ++option % 9;
                    }
                    if (field[option / 3][option % 3] == ' ' && (option / 3 != tmp / 3 || tmp % 3 != option % 3)) break;
                    option = (3 +option) % 9;
                }
                if (field[option / 3][option % 3] != ' ') option = tmp;
                break;
            case 10:
                return std::make_pair(option / 3, option % 3);
        }
    }
};

std::pair<int, int> LowSkillBotPlayer::get_step(const Field &fl, bool) {
    auto field = fl.get_field();
    std::pair<int, int> res = std::make_pair(0, 0);
    int cnt = 0;
    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field.size(); ++j) {
            if (field[i][j] == ' ') {
                cnt++;
                res.first = i;
                res.second = j;
            }
        }
    }
    if (cnt == 1) return res;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, 10);
    while (true) {
        int rnd = dist(mt);
        int x = rnd % 3;
        int y = rnd / 3;
        if (field[x][y] == ' ')
            return std::make_pair(x, y);
    }
}

std::pair<int, int> HighSkillBotPlayer::get_step(const Field &fl, bool current_player) {
    auto field = fl.get_field();
    std::map<std::pair<int, int>, int> ranks;
    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field.size(); ++j) {
            if (field[i][j] != ' ') continue;
            field[i][j] = current_player ? player_2_sign : player_1_sign;
            Field fld = Field(field);
            ranks[std::make_pair(i, j)] = win_rate(fld, !current_player, current_player, 1);
            field[i][j] = ' ';
        }
    }
    std::pair<int, int> res = std::make_pair(0, 0);
    if (ranks.size() == 1) return ranks.begin()->first;
    int max = -300000;
    for (auto it = ranks.begin(); it != ranks.end(); ++it) {
        if (it->second > max) {
            res.first = it->first.first;
            res.second = it->first.second;
            max = it->second;
        }
    }
    return res;
}

int win_rate(const Field &fl, bool current_player, bool st_player, int depth) {
    char wn = fl.is_winner();
    if (wn != 0) {
        if (!st_player && wn == player_1_sign) return 100 / depth;
        else if (st_player && wn == player_2_sign) return 100 / depth;
        else return -100 / depth;
    }
    if (fl.is_full()) return 0;

    auto field = fl.get_field();
    std::map<std::pair<int, int>, int> ranks;
    for (int i = 0; i < field.size(); ++i) {
        for (int j = 0; j < field.size(); ++j) {
            if (field[i][j] != ' ') continue;
            field[i][j] = !current_player ? player_1_sign : player_2_sign;
            Field fld = Field(field);
            ranks[std::make_pair(i, j)] = win_rate(fld, !current_player, st_player, depth + 1);
            field[i][j] = ' ';
        }
    }
//    for (auto it = ranks.begin(); it != ranks.end(); ++it) sum += it->second;
    if (current_player == st_player) {
        int max = std::numeric_limits<int>::min();
        for (auto it = ranks.begin(); it != ranks.end(); ++it) {
            if (it->second > max) {
                max = it->second;
            }
        }
        return max;
    } else {
        int min = std::numeric_limits<int>::max();
        for (auto it = ranks.begin(); it != ranks.end(); ++it) {
            if (it->second < min) {
                min = it->second;
            }
        }
        return min;
    }
}

