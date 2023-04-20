#include <iostream>
#include <chrono>
#include <cmath>

#if _WIN32

#include <windows.h>

#endif


using namespace std;


class Screen {
public:
    Screen() {
#ifdef _WIN32
        _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
        hideCursor();
        saveCursorPosition();
    }


    void clear() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                _screen[i][j] = 'x';
            }
        }
    }


    void draw(char value, float x, float y) {
        int xPos = lround(x);
        int yPos = lround(y);
        if (xPos < width && yPos < height) {
            _screen[yPos][xPos] = value;
        }
    }


    void flush() {
        restoreCursorPosition();


        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (_screen[i][j] != 'x') {
                    setRedFont();
                } else {
                    setWhiteFont();
                }
                cout << _screen[i][j];
            }
            cout << "\n";
        }
    }


private:
    void hideCursor() {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(_hConsole, &cursorInfo);
        cursorInfo.dwSize = 1;
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(_hConsole, &cursorInfo);
#else
        cout << "\33[?25l";//disable cursor
#endif


    }


    void saveCursorPosition() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        GetConsoleScreenBufferInfo(_hConsole, &coninfo);
        _prevPos = coninfo.dwCursorPosition;
#else
        cout << "\33[s";
#endif

    }


    void restoreCursorPosition() {
#ifdef _WIN32
        SetConsoleCursorPosition(_hConsole, _prevPos);
#else
        cout << "\338";
#endif
    }


    void setRedFont() {
#ifdef _WIN32
        WORD attributes = FOREGROUND_RED;
        SetConsoleTextAttribute(_hConsole, attributes);
#else
        cout << "\33[31m";//red color
#endif
    }


    void setWhiteFont() {
#ifdef _WIN32
        WORD attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        SetConsoleTextAttribute(_hConsole, attributes);
#else
        cout << "\33[37m";//white color
#endif
    }


    static constexpr int width{60};
    static constexpr int height{20};
    char _screen[height][width];
#ifdef _WIN32
    HANDLE _hConsole;
    COORD _prevPos;
#endif
};


struct Position {
    float x;
    float y;
};


using Pixel = char;


class Engine {
public:

    int repeat{};

    void update(chrono::milliseconds deltaTime) {
        if (position.x < 40.f && position.y == 0.f) {
            float a_distance = deltaTime.count() / 2000.f * 40.f;
            position.x += a_distance;
        } else if (position.x >= 40.f && position.y < 10.f) {
            float b_distance = deltaTime.count() / 3000.f * 10.f;
            position.y += b_distance;
        } else if (position.y > 0.f && position.x > 0.f) {
            float c_distance_x = deltaTime.count() / 1000.0f * 40.0f / sqrt(2);
            float c_distance_y = deltaTime.count() / 1000.0f * 10.0f / sqrt(2);
            position.x -= c_distance_x;
            position.y -= c_distance_y;
        } else {
            repeat++;
            position.x = 0.f;
            position.y = 0.f;
        }
    }


    void render(Screen &screen) {
        screen.draw(pixel, position.x, position.y);
    }

    Position position{0.0f, 0.0f};
private:
    Pixel pixel{1};

};


int main() {
    Engine engine;
    Screen screen;


    auto startTime = chrono::high_resolution_clock::now();
    while (engine.repeat <= 4) {

        auto now = chrono::high_resolution_clock::now();
        auto deltaTime = chrono::duration_cast<chrono::milliseconds>(now - startTime);
        if (deltaTime >= 20ms) {
            screen.clear();
            engine.update(deltaTime);
            engine.render(screen);
            screen.flush();
            startTime = now;

        }
    }


    return 0;
}