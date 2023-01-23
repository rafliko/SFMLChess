#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string getBestMove(int skill, string position, int movetime, string engine);
void move(string mv, char color, string& turn, sf::Sound& sound, sf::SoundBuffer& mvSound, sf::SoundBuffer& capSound, sf::SoundBuffer& sigSound);
void specialMoves(string move, char color);
void intToMove(int x1, int y1, int x2, int y2, string& mv);
void moveToInt(string mv, int& x1, int& y1, int& x2, int& y2);

char board[8][8] =
{
    {'r','p','0','0','0','0','P','R'},
    {'n','p','0','0','0','0','P','N'},
    {'b','p','0','0','0','0','P','B'},
    {'q','p','0','0','0','0','P','Q'},
    {'k','p','0','0','0','0','P','K'},
    {'b','p','0','0','0','0','P','B'},
    {'n','p','0','0','0','0','P','N'},
    {'r','p','0','0','0','0','P','R'}
};

char playercolor = 'w';
char aicolor = 'b';

int skill = 0;
int movetime = 1500;
string position = "startpos moves";
string engine = "stockfish.exe";

const int scale = 80;

int main()
{
    int mx = -1;
    int my = -1;
    int newmx = -1;
    int newmy = -1;
    string turn = "player";

    cout << "Player color (w/b): " << endl;
    cin >> playercolor;
    cout << "Skill level (0-20): " << endl;
    cin >> skill;
    cout << "position startpos moves ";

    if ((playercolor != 'w' && playercolor != 'b') || (skill > 20 || skill < 0)) return 1;

    if(playercolor == 'b')
    {
        aicolor = 'w';
        turn = "ai";
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                if (islower(board[x][y])) board[x][y] = toupper(board[x][y]);
                else if (isupper(board[x][y])) board[x][y] = tolower(board[x][y]);

                if (board[x][y] == 'k') board[x][y] = 'q';
                else if (board[x][y] == 'q') board[x][y] = 'k';
                else if (board[x][y] == 'K') board[x][y] = 'Q';
                else if (board[x][y] == 'Q') board[x][y] = 'K';
            }
        }
    }

    sf::SoundBuffer moveSoundBuffer;
    moveSoundBuffer.loadFromFile("sound/move.ogg");
    sf::SoundBuffer captureSoundBuffer;
    captureSoundBuffer.loadFromFile("sound/capture.ogg");
    sf::SoundBuffer signalSoundBuffer;
    signalSoundBuffer.loadFromFile("sound/signal.ogg");
    sf::Sound sound;
    sf::RenderWindow window(sf::VideoMode(scale*8, scale*8), "SFML chess", sf::Style::Close);
    sf::Texture tex;
    tex.loadFromFile("img/tex.png");
    tex.setSmooth(true);
    sf::Sprite s[2][7];
    for (int c = 0; c < 2; c++)
    {
        for (int p = 0; p < 7; p++)
        {
            s[c][p].setTexture(tex);
            s[c][p].setTextureRect(sf::IntRect(128 * p, 128 * c, 128, 128));
            s[c][p].setScale(sf::Vector2f(scale/128.0f, scale/128.0f));
        }
    }

    sound.setBuffer(signalSoundBuffer);
    sound.play();

    while (window.isOpen())
    {
        // Events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if (mx > -1 && my > -1)
                    {
                        newmx = event.mouseButton.x / scale;
                        newmy = event.mouseButton.y / scale;
                        if (mx!=newmx || my!=newmy)
                        {
                            // Player move
                            if (turn=="player")
                            {
                                string mv = "";
                                intToMove(mx, my, newmx, newmy, mv);
                                move(mv, playercolor, turn, sound, moveSoundBuffer, captureSoundBuffer, signalSoundBuffer);
                            }
                        }
                        mx = -1;
                        my = -1;
                        newmx = -1;
                        newmy = -1;
                    }
                    else
                    {
                        mx = event.mouseButton.x / scale;
                        my = event.mouseButton.y / scale;
                        if (board[mx][my] == '0')
                        {
                            mx = -1;
                            my = -1;
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    mx = -1;
                    my = -1;
                    newmx = -1;
                    newmy = -1;
                }
            }
        }

        // Drawing
        window.clear();
        int boardColor = 1;
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                s[boardColor][6].setColor(sf::Color::White);
                s[boardColor][6].setPosition(x * scale, y * scale);
                if (x == mx && y == my) s[boardColor][6].setColor(sf::Color::Green);
                window.draw(s[boardColor][6]);
                boardColor ^= 1;

                int pieceColor = 0;
                if (isupper(board[x][y])) pieceColor = 1;

                switch (tolower(board[x][y]))
                {
                case 'p':
                    s[pieceColor][0].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][0]);
                    break;
                case 'n':
                    s[pieceColor][1].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][1]);
                    break;
                case 'b':
                    s[pieceColor][2].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][2]);
                    break;
                case 'r':
                    s[pieceColor][3].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][3]);
                    break;
                case 'q':
                    s[pieceColor][4].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][4]);
                    break;
                case 'k':
                    s[pieceColor][5].setPosition(x * scale, y * scale);
                    window.draw(s[pieceColor][5]);
                    break;
                default:
                    break;
                }
            }
            boardColor ^= 1;
        }
        window.display();

        // AI move
        if (turn == "ai")
        {
            string mv = getBestMove(skill, position, movetime, engine);
            move(mv, aicolor, turn, sound, moveSoundBuffer, captureSoundBuffer, signalSoundBuffer);
        }
    }

    return 0;
}

string getBestMove(int skill, string position, int movetime, string engine)
{
    try
    {
        string command = "(echo uci & \
                        echo setoption name Skill Level value " + to_string(skill) + " & \
                        echo position " + position + " & \
                        echo go movetime " + to_string(movetime) + " & \
                        timeout " + to_string(movetime / 1000) + ") \
                        | " + engine + " | find \"bestmove\" > move.tmp";

        if (system(command.c_str()) != 0) return "err";
        ifstream mv_file("move.tmp");
        string mv = "";
        getline(mv_file, mv);
        mv = mv.substr(9, 4);
        mv_file.close();
        remove("move.tmp");
        if (mv == "(non") return "checkmate";
        int x1, y1, x2, y2;
        moveToInt(mv, x1, y1, x2, y2);
        if (((y2 == 0 || y2 == 7) && board[x1][y1] == 'P') ||
            ((y2 == 0 || y2 == 7) && board[x1][y1] == 'p'))
        {
            mv += 'q';
        }
        if ((aicolor == 'b' && islower(board[x1][y1])) || (aicolor == 'w' && isupper(board[x1][y1]))) return mv;
        else return "err";
    }
    catch (exception ex)
    {
        return "err";
    }
}

void move(string mv, char color, string& turn, sf::Sound& sound, sf::SoundBuffer& mvSound, sf::SoundBuffer& capSound, sf::SoundBuffer& sigSound)
{
    string testMove = getBestMove(20, position + " " + mv, 100, engine);
    if (testMove == "err" && turn == "player") return;

    int x1, y1, x2, y2;
    moveToInt(mv, x1, y1, x2, y2);

    cout << mv << " ";
    position += " " + mv;
    specialMoves(mv, color);

    if (board[x2][y2] == '0')
    {
        sound.setBuffer(mvSound);
        sound.play();
    }
    else
    {
        sound.setBuffer(capSound);
        sound.play();
    }

    if (testMove == "checkmate")
    {
        turn = "none";
        if (color == 'w') cout << endl << "White wins!";
        else if (color == 'b') cout << endl << "Black wins!";
        sound.setBuffer(sigSound);
        sound.play();
    }
    else
    {
        if (turn == "ai") turn = "player";
        else if (turn == "player") turn = "ai";
    }

    board[x2][y2] = board[x1][y1];
    board[x1][y1] = '0';
}

void specialMoves(string move, char color)
{
    int x1, y1, x2, y2;
    moveToInt(move, x1, y1, x2, y2);

    char test1 = tolower(board[x1][y1]);
    char test2 = tolower(board[x2][y2]);

    if ((y2 == 0 || y2 == 7) && test1 == 'p') //promotion
    {
        board[x1][y1] = 'q';
    }
    else if ((move == "e8g8" || move == "e1g1") && test1 == 'k') //short castling
    {
        if (x1 == 4)
        {
            if(color=='b') board[x2 - 1][y2] = 'r';
            else if(color=='w') board[x2 - 1][y2] = 'R';
            board[x2 + 1][y2] = '0';
        }
        else if (x1 == 3)
        {
            if (color == 'b') board[x2 + 1][y2] = 'r';
            else if (color == 'w') board[x2 + 1][y2] = 'R';
            board[x2 - 1][y2] = '0';
        }
    }
    else if ((move == "e8c8" || move == "e1c1") && test1 == 'k') //long castling
    {
        if (x1 == 4)
        {
            if (color == 'b') board[x2 + 1][y2] = 'r';
            else if (color == 'w') board[x2 + 1][y2] = 'R';
            board[x2 - 2][y2] = '0';
        }
        else if (x1 == 3)
        {
            if (color == 'b') board[x2 - 1][y2] = 'r';
            else if (color == 'w') board[x2 - 1][y2] = 'R';
            board[x2 + 2][y2] = '0';
        }
    }
    else if ((x2 == x1 + 1 || x2 == x1 - 1) && test2 == '0' && test1 == 'p') //en passant
    {
        if (color == 'b')
        {
            if (board[x2][y2 - 1] == 'P') board[x2][y2 - 1] = '0';
            else if (board[x2][y2 + 1] == 'P') board[x2][y2 + 1] = '0';
        }
        else if (color == 'w')
        {
            if (board[x2][y2 - 1] == 'p') board[x2][y2 - 1] = '0';
            else if (board[x2][y2 + 1] == 'p') board[x2][y2 + 1] = '0';
        }
    }
}

void intToMove(int x1, int y1, int x2, int y2, string& mv)
{
    if (playercolor == 'w')
    {
        mv += x1 + 'a';
        mv += to_string(8 - y1);
        mv += x2 + 'a';
        mv += to_string(8 - y2);
    }
    else if (playercolor == 'b')
    {
        mv += 7 - x1 + 'a';
        mv += to_string(y1 + 1);
        mv += 7 - x2 + 'a';
        mv += to_string(y2 + 1);
    }
}

void moveToInt(string mv, int& x1, int& y1, int& x2, int& y2)
{
    if (playercolor == 'w')
    {
        x1 = mv[0] - 'a';
        y1 = 8 - (mv[1] - '0');
        x2 = mv[2] - 'a';
        y2 = 8 - (mv[3] - '0');
    }
    else if (playercolor == 'b')
    {
        x1 = 7 - (mv[0] - 'a');
        y1 = (mv[1] - '0') - 1;
        x2 = 7 - (mv[2] - 'a');
        y2 = (mv[3] - '0') - 1;
    }
}