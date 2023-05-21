#include <gtkmm.h>
#include <string>
using namespace std;


class Window : public Gtk::Window {
private:
    Gtk::Fixed fixed;
    Gtk::Button restart;
    Gtk::Button tools_change;
    Gtk::Label time;
    Gtk::Grid field;
    int time_second;
    int time_minute;
    int field_matrix[9][9][3]; // минное поле в виде массива. в каждой ячейке хранится значение, открыта ли, помечена ли
    bool game_start = false;
    bool game_win = false;
    bool game_lose = false;
    int dig_cell; // кл-во открытых клеток. Для победы нужно открыть 71 клетку.
    int period = 1; // позволяет в конце менять значение таймера на надпись и наоборот.
    bool tool = false; // какой инструмент используется. false - лопата, true - флажок.
public:
    Window() : Gtk::Window() {
        clear_field();
        dig_cell = 0;
        time_second = 0;
        time_minute = 0;


        add(fixed);
        fixed.add(restart);
        fixed.add(time);
        fixed.add(field);
        fixed.add(tools_change);


        field.set_column_spacing(5);
        field.set_row_spacing(5);
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                Gtk::Button *button = new Gtk::Button();
                button->set_size_request(50, 50);
                field.attach(*button, col, row, 1, 1);
                pair<int, int> coord;
                coord.first = row;
                coord.second = col;
                button->signal_clicked().connect( sigc::bind<pair<int,int>, Gtk::Button*>( sigc::mem_fun(*this, &Window::open_field), coord, button));
            }
        }
        fixed.move(field, 105, 120);


        restart.set_label("Перезапустить");
        fixed.move(restart, 300, 700);
        restart.set_size_request(100, 20);
        restart.signal_clicked().connect(sigc::mem_fun(*this, &Window::restart_game));


        fixed.move(tools_change,325, 50);
        tools_change.set_size_request(50, 50);
        tools_change.signal_clicked().connect(sigc::mem_fun(*this, &Window::tool_choice));
        tools_change.set_label("лопата");


        time.set_label("00:00");
        fixed.move(time, 300, 20);
        time.set_size_request(100, 20);
        Glib::signal_timeout().connect(sigc::mem_fun(*this, Window::on_time), 1000);


        Gdk::RGBA color_background("#cccccc");
        override_background_color(color_background);
        set_default_size(700, 800);
        set_title("Sapper");
        show_all();
    }

    void tool_choice(){//выбор инструмента
        if(tool){
            tool = false;
            tools_change.set_label("лопата");
        }else{
            tool = true;
            tools_change.set_label("флаг");
        }
    }
    void restart_game() {// перезапуск игры. Всё сводится к начальным значениям.
        time_second = 0;
        time_minute = 0;
        dig_cell = 0;
        time.set_label("00:00");
        clear_field();
        game_start = false;
        game_win = false;
        game_lose = false;
        int dig_cell = 0;
        for(int i = 0; i < 9; ++i){// позволяет очистить grid
            for(int j = 0; j < 9; ++j){
                Gtk::Widget* widget = field.get_child_at(i, j);
                Gtk::Button* button = dynamic_cast<Gtk::Button*>(widget);
                button->set_label("");
            }
        }
    }
    void open_field(pair<int, int> coord, Gtk::Button* button) {// обработка нажатия на ячейку поля.
        if (!game_start) {
            make_field(coord.first, coord.second);
            game_start = true;
        }
            if(!game_win and !game_lose and tool and field_matrix[coord.first][coord.second][1] != 1 and field_matrix[coord.first][coord.second][2] != 1){
                field_matrix[coord.first][coord.second][2] = 1;
                button->set_label("М");
            }else {
                if (!game_win and !game_lose and tool and field_matrix[coord.first][coord.second][2] == 1) {
                    field_matrix[coord.first][coord.second][2] = 0;
                    button->set_label("");
                }
            }
            if (!game_win and !game_lose and field_matrix[coord.first][coord.second][1] != 1 and field_matrix[coord.first][coord.second][2] == 0 and !tool) {
                button->set_label(to_string(field_matrix[coord.first][coord.second][0]));
                field_matrix[coord.first][coord.second][1] = 1;
                if(field_matrix[coord.first][coord.second][0] == 0) {
                    right_open_clear_field(coord.first, coord.second);
                    left_open_clear_field(coord.first, coord.second);
                }
                if (field_matrix[coord.first][coord.second][0] == -1){
                    game_lose = true;
                }
                if (dig_cell == 71) game_win = true;
            }
            count_open_field();
        }

    bool on_time() {

        if(game_start and !game_win and !game_lose) {
            time_second++;
            string minutes = "";
            string seconds = "";
            if (time_second == 60) {
                time_second = 0;
                time_minute += 1;
            }
            if (time_second < 10) {
                seconds += "0" + to_string(time_second);
            } else {
                seconds += to_string(time_second);
            }
            if (time_minute < 10) {
                minutes += "0" + to_string(time_minute);
            } else {
                minutes += to_string(time_minute);
            }
            time.set_label(minutes + ":" + seconds);
        }else{
            if(game_win){
                if(period == 1){
                    time.set_label("VICTORY!");
                    period = 0;
                }else{
                    string minutes = "";
                    string seconds = "";
                    if (time_second == 60) {
                        time_second = 0;
                        time_minute += 1;
                    }
                    if (time_second < 10) {
                        seconds += "0" + to_string(time_second);
                    } else {
                        seconds += to_string(time_second);
                    }
                    if (time_minute < 10) {
                        minutes += "0" + to_string(time_minute);
                    } else {
                        minutes += to_string(time_minute);
                    }
                    time.set_label(minutes + ":" + seconds);
                    period = 1;
                }
            }
            if(game_lose){
                if(period == 1){
                    time.set_label("GAME OVER!");
                    period = 0;
                }else{
                    string minutes = "";
                    string seconds = "";
                    if (time_second == 60) {
                        time_second = 0;
                        time_minute += 1;
                    }
                    if (time_second < 10) {
                        seconds += "0" + to_string(time_second);
                    } else {
                        seconds += to_string(time_second);
                    }
                    if (time_minute < 10) {
                        minutes += "0" + to_string(time_minute);
                    } else {
                        minutes += to_string(time_minute);
                    }
                    time.set_label(minutes + ":" + seconds);
                    period = 1;
                }
            }
        }
        return true;
    }

    void make_field(int i_chose, int j_chose){// создаёт поле
        int mine = 0;
        int random_cell_i;
        int random_cell_j;
        while(mine != 10){
            random_cell_i = rand() % 9;
            random_cell_j = rand() % 9;
            if(field_matrix[random_cell_i][random_cell_j][0] != -1 and random_cell_i != i_chose and random_cell_j != j_chose){//расставляются мины
                field_matrix[random_cell_i][random_cell_j][0] = -1;
                mine += 1;
            }
        }
        // ставится значение по углам
        if(field_matrix[0][0][0] != -1) field_matrix[0][0][0] = -(field_matrix[1][0][0] + field_matrix[1][1][0] + field_matrix[0][1][0]);
        if(field_matrix[8][0][0] != -1) field_matrix[8][0][0] = -(field_matrix[8][1][0] + field_matrix[7][1][0] + field_matrix[7][0][0]);
        if(field_matrix[0][8][0] != -1) field_matrix[0][8][0] = -(field_matrix[0][7][0] + field_matrix[1][8][0] + field_matrix[1][7][0]);
        if(field_matrix[8][8][0] != -1) field_matrix[8][8][0] = -(field_matrix[8][7][0] + field_matrix[7][8][0] + field_matrix[7][7][0]);
        for(int i = 1; i < 8; ++i) { // ставится значение по правому и левому краю
            if (field_matrix[i][0][0] != -1) {
                if (field_matrix[i - 1][1][0] == -1) field_matrix[i][0][0] += 1;
                if (field_matrix[i][1][0] == -1) field_matrix[i][0][0] += 1;
                if (field_matrix[i + 1][1][0] == -1) field_matrix[i][0][0] += 1;
                if (field_matrix[i - 1][0][0] == -1) field_matrix[i][0][0] += 1;
                if (field_matrix[i + 1][0][0] == -1) field_matrix[i][0][0] += 1;
            }
            if(field_matrix[i][8][0] != -1){
                if (field_matrix[i - 1][7][0] == -1) field_matrix[i][8][0] += 1;
                if (field_matrix[i + 1][7][0] == -1) field_matrix[i][8][0] += 1;
                if (field_matrix[i - 1][8][0] == -1) field_matrix[i][8][0] += 1;
                if (field_matrix[i + 1][8][0] == -1) field_matrix[i][8][0] += 1;
                if (field_matrix[i][7][0] == -1) field_matrix[i][8][0] += 1;
            }
        }
        for(int j = 1; j < 8; ++j) { // ставится значение по верхнему и нижнему краю.
            if (field_matrix[0][j][0] != -1) {
                if (field_matrix[1][j][0] == -1) field_matrix[0][j][0] += 1;
                if (field_matrix[1][j + 1][0] == -1) field_matrix[0][j][0] += 1;
                if (field_matrix[1][j - 1][0] == -1) field_matrix[0][j][0] += 1;
                if (field_matrix[0][j - 1][0] == -1) field_matrix[0][j][0] += 1;
                if (field_matrix[0][j  + 1][0] == -1) field_matrix[0][j][0] += 1;
            }
            if (field_matrix[8][j][0] != -1) {
                if (field_matrix[7][j][0] == -1) field_matrix[8][j][0] += 1;
                if (field_matrix[7][j + 1][0] == -1) field_matrix[8][j][0] += 1;
                if (field_matrix[7][j - 1][0] == -1) field_matrix[8][j][0] += 1;
                if (field_matrix[8][j + 1][0] == -1) field_matrix[8][j][0] += 1;
                if (field_matrix[8][j - 1][0] == -1) field_matrix[8][j][0] += 1;
            }
        }
        // расставляются значение в остальных ячейках
        for(int i = 1; i < 8; ++i){
            for(int j = 1; j < 8; ++j){
                if(field_matrix[i][j][0] != -1){
                    if(field_matrix[i - 1][j - 1][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i - 1][j][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i - 1][j + 1][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i][j - 1][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i][j + 1][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i + 1][j - 1][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i + 1][j][0] == -1) field_matrix[i][j][0] += 1;
                    if(field_matrix[i + 1][j + 1][0] == -1) field_matrix[i][j][0] += 1;
                }
            }
        }

    }

    void clear_field(){// очищает массив поля
        for(int i = 0; i < 9; ++i){
            for(int j = 0; j < 9; ++j){
                field_matrix[i][j][0] = 0;
                field_matrix[i][j][1] = 0;
                field_matrix[i][j][2] = 0;
            }
        }
    }

    void right_open_clear_field(int i, int j){ // позволять открыть нулевые клетки справа и снизу
        this->field_matrix[i][j][1] = 1;
        if(i < 8 and field_matrix[i + 1][j][0] == 0 and field_matrix[i + 1][j][1] == 0 and field_matrix[i + 1][j][2] == 0) {
            Gtk::Widget *widget = field.get_child_at(j, i + 1);
            Gtk::Button *button = dynamic_cast<Gtk::Button *>(widget);
            button->set_label("0");
            right_open_clear_field(i + 1, j);
        }
        if(j < 8 and field_matrix[i][j + 1][0] == 0 and field_matrix[i][j + 1][1] == 0 and field_matrix[i][j + 1][2] == 0){
            Gtk::Widget* widget = field.get_child_at(j + 1, i);
            Gtk::Button* button = dynamic_cast<Gtk::Button*>(widget);
            button->set_label("0");
            right_open_clear_field(i, j + 1);
        }
    }
    void left_open_clear_field(int i, int j){ // позволять открыть нулевые клетки слева и сверху
        this->field_matrix[i][j][1] = 1;
        if(i > 0 and field_matrix[i - 1][j][0] == 0 and field_matrix[i - 1][j][1] == 0 and field_matrix[i - 1][j][2] == 0){
                Gtk::Widget* widget = field.get_child_at(j, i - 1);
                Gtk::Button* button = dynamic_cast<Gtk::Button*>(widget);
                button->set_label("0");
                left_open_clear_field(i - 1, j);
            }
            if(j > 0 and field_matrix[i][j - 1][0] == 0 and field_matrix[i][j - 1][1] == 0 and field_matrix[i][j - 1][2] == 0){
                Gtk::Widget* widget = field.get_child_at(j - 1, i);
                Gtk::Button* button = dynamic_cast<Gtk::Button*>(widget);
                button->set_label("0");
                left_open_clear_field(i, j - 1);
            }
    }
    void count_open_field(){ // подсчёт открытых клеток
        dig_cell = 0;
        for(int i = 0; i < 9; ++i){
            for(int j = 0; j < 9; ++j){
                dig_cell += field_matrix[i][j][1];
            }
        }
    }
};

int main(int argc, char ** argv){
    auto app = Gtk::Application::create(argc, argv);
    Window wnd;
    return app->run(wnd);
}
