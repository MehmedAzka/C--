#include <iostream>
#include <vector>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <conio.h>
#include <random> // untuk std::default_random_engine dan std::shuffle

using namespace std;

const int GRID_SIZE = 15; // ukuran papan
vector<vector<string>> playerBoard(GRID_SIZE, vector<string>(GRID_SIZE, "[ ]"));
vector<vector<string>> enemyBoard(GRID_SIZE, vector<string>(GRID_SIZE, "( )"));

int difficultyLevel = 1;
int roundCounter = 0; // Menyimpan jumlah ronde yang telah dilalui
int playerScore = 0;  // Variabel untuk skor pemain
int enemyScore = 0;   // Variabel untuk skor musuh

void placeShips(vector<vector<string>>& board, const string& shipSymbol, vector<pair<int, int>> positions) {
    int shipsPlaced = 0;
    const int maxShips = 100; // Ubah jika ingin jumlah kapal berbeda

    for (const auto& pos : positions) {
        int x = pos.first;
        int y = pos.second;

        if (board[x][y] == "[ ]" || board[x][y] == "( )") {
            board[x][y] = shipSymbol;
            shipsPlaced++;

            if (shipsPlaced == maxShips) { // Batasi hingga maxShips
                break;
            }
        }
    }
}

bool makeMove(vector<vector<string>>& board, int x, int y, const string& shipSymbol, const string& hitSymbol, const string& missSymbol) {
    if (board[x][y] == shipSymbol) {
        board[x][y] = hitSymbol; // hit
        return true;
    }
    else if (board[x][y] == "[ ]" || board[x][y] == "( )") {
        board[x][y] = missSymbol; // miss
        return false;
    }
    return false;
}

bool isGameOver(const vector<vector<string>>& board, const string& shipSymbol) {
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (board[i][j] == shipSymbol)
                return false;
    return true;
}

bool isValidSingleNumber(const string& input, int min, int max) {
    try {
        int number = stoi(input);
        return number >= min && number <= max;
    }
    catch (const invalid_argument& e) {
        return false;
    }
    catch (const out_of_range& e) {
        return false;
    }
}

void displayBoard(const vector<vector<string>>& board) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++)
            cout << board[i][j] << ' ';
        cout << endl;
    }
}

void showLoadingSpinner(int duration) {
    const char spinner[] = { '|', '/', '-', '\\' };
    int spinnerIndex = 0;
    auto start = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() < duration) {
        cout << "\r" << spinner[spinnerIndex] << flush;
        spinnerIndex = (spinnerIndex + 1) % 4;
        std::this_thread::sleep_for(chrono::milliseconds(150)); // Tetap menggunakan std::
    }
    cout << "\r " << flush; // Clear the spinner
}

void displayScores() {
    cout << "\nScore Player: " << playerScore << " | Score Musuh: " << enemyScore << endl;
}


void enemyMoveVeryEasy() {
    int x, y;
    do {
        x = rand() % GRID_SIZE;
        y = rand() % GRID_SIZE;
    } while (playerBoard[x][y] == "[o]" || playerBoard[x][y] == "[X]"); // Pastikan musuh tidak menyerang posisi [o] atau [X]

    if (makeMove(playerBoard, x, y, "[S]", "[X]", "[o]")) {
        enemyScore++;
        cout << "+-----------------------------------+\n";
        cout << "|           Musuh Menyerang         |\n";
        cout << "+-----------------------------------+\n";
        showLoadingSpinner(1);
        cout << "\nHit pada (" << y + 1 << ", " << x + 1 << ")!" << endl << endl;
        showLoadingSpinner(4);
        system("cls");
    }
    else {
        cout << "+-----------------------------------+\n";
        cout << "|           Musuh Menyerang         |\n";
        cout << "+-----------------------------------+\n";
        showLoadingSpinner(1);
        cout << "\nMiss pada (" << y + 1 << ", " << x + 1 << ")!" << endl << endl;
        showLoadingSpinner(4);
        system("cls");
    }
}

void enemyMoveNormal() {
    static int lastX = -1, lastY = -1;

    // Jika ada posisi terakhir yang di-hit
    if (lastX != -1 && lastY != -1) {
        // Cek apakah posisi terakhir di-hit
        if (playerBoard[lastX][lastY] == "[X]") {
            // Cek posisi di sekitarnya
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    // Hanya memeriksa posisi di atas, bawah, kiri, kanan
                    if (abs(dx) + abs(dy) == 1) {
                        int x = lastX + dx;
                        int y = lastY + dy;

                        // Memastikan posisi dalam batas dan tidak di [o] atau [X]
                        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                            // Jika posisi adalah kapal
                            if (playerBoard[x][y] == "[S]") {
                                cout << "+-----------------------------------+\n";
                                cout << "|           Musuh Menyerang         |\n";
                                cout << "+-----------------------------------+\n";
                                makeMove(playerBoard, x, y, "[S]", "[X]", "[o]");
                                showLoadingSpinner(1);
                                cout << "\nHit pada (" << y + 1 << ", " << x + 1 << ")!" << endl << endl;
                                lastX = x; // Update posisi terakhir
                                lastY = y;
                                showLoadingSpinner(4);
                                system("cls");
                                return; // Hentikan setelah menyerang satu kali
                            }
                        }
                    }
                }
            }
        }
    }

    // Jika tidak ada hit sebelumnya, serang posisi kapal yang masih ada
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (playerBoard[i][j] == "[S]") {
                cout << "+-----------------------------------+\n";
                cout << "|           Musuh Menyerang         |\n";
                cout << "+-----------------------------------+\n";
                makeMove(playerBoard, i, j, "[S]", "[X]", "[o]");
                enemyScore++;
                showLoadingSpinner(1);
                cout << "\nHit pada (" << j + 1 << ", " << i + 1 << ")!" << endl << endl;
                lastX = i; // Update posisi terakhir
                lastY = j;
                showLoadingSpinner(4);
                system("cls");
                return; // Hentikan setelah menyerang satu kali
            }
        }
    }

    // Jika tidak ada kapal yang tersisa, serang acak
    enemyMoveVeryEasy();
}

void enemyMoveHard() {
    bool targetFound = false; // Menyimpan status apakah [S] ditemukan
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (playerBoard[i][j] == "[S]") {
                targetFound = true; // Tandai bahwa target ditemukan
                cout << "+-----------------------------------+\n";
                cout << "|           Musuh Menyerang         |\n";
                cout << "+-----------------------------------+\n";
                makeMove(playerBoard, i, j, "[S]", "[X]", "[o]"); // Serang posisi kapal
                enemyScore++;
                showLoadingSpinner(1);
                cout << "\nHit pada (" << j + 1 << ", " << i + 1 << ")!" << endl << endl;
                showLoadingSpinner(4);
                system("cls");
                break; // Lanjut ke posisi berikutnya setelah serangan
            }
        }
        if (targetFound) break; // Hentikan loop jika sudah ada hit pada [S]
    }
    if (!targetFound) {
        cout << "Tidak ada target [S] yang ditemukan.\n";
    }
}

void enemyMove() {
    roundCounter++; // Increment counter setiap giliran musuh
    switch (difficultyLevel) {
    case 1: // Easy
        enemyMoveVeryEasy();
        break;
    case 2: // Normal
        // Set hitRound untuk 2-3 ronde
        if (roundCounter % (2 + rand() % 2) == 0) { // Musuh menyerang setiap 2-3 ronde
            enemyMoveVeryEasy();
        }
        else {
            enemyMoveNormal();
        }
        break;
    case 3: // Very Hard
        enemyMoveHard();
        break;
    }
}

void openingPage() {
    cout << "=====================================" << endl;
    cout << "       WELCOME TO BATTLESHIP!        " << endl;
    cout << "=====================================" << endl << endl;
    cout << "Selamat datang di permainan Battleship!" << endl;
    cout << "Tujuan permainan ini adalah untuk menemukan dan" << endl;
    cout << "menghancurkan semua kapal musuh sebelum mereka" << endl;
    cout << "menghancurkan kapal Anda!" << endl << endl;
    cout << "Press any key to continue...";

    getch(); // Tunggu pemain menekan tombol apa saja 
    system("cls");
}

void showHowToPlay() {
    cout << endl << "=== Cara Bermain ===" << endl;
    cout << "1. Pemain dan musuh akan menempatkan kapal di papan secara acak." << endl;
    cout << "2. Giliran akan bergantian antara pemain dan musuh." << endl;
    cout << "3. Masukkan koordinat (x y) untuk menembak musuh. ex: 2 4" << endl;
    cout << "4. Jika mengenai kapal, akan ada pesan 'Hit!', jika tidak maka 'Miss'." << endl;
    cout << "5. [S] adalah Kapal milik mu, (S) adalah Kapal milik musuh. Hancurkan mereka!" << endl;
    cout << "6. Kapal mu yang hancur akan ditandai dengan simbol [X]" << endl;
    cout << "7. Kapal musuh yang hancur akan ditandai dengan simbol (X)" << endl;
    cout << "8. Permainan berakhir jika semua kapal salah satu pihak hancur." << endl << endl;
    cout << "Press any key to continue...";

    getch(); // Tunggu pemain menekan tombol apa saja
    system("cls");
}

void displaySeparateBoards() {
    // Menampilkan papan lawan
    cout << "====================== Board Lawan ========================" << endl;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << enemyBoard[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;

    // Menampilkan papan pemain
    cout << "====================== Board Player =======================" << endl;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << playerBoard[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    openingPage();
    showHowToPlay();

    string input;
    int shipCount;

    showLoadingSpinner(1);
    // Pemilihan tingkat kesulitan dengan validasi
    while (true) {
        cout << endl;
        cout << "+----------+------------+----------+\n";
        cout << "| 1 = Easy | 2 = Normal | 3 = Hard |\n";
        cout << "+----------+------------+----------+\n";
        cout << "Tingkat kesulitan musuh: ";
        getline(cin, input);

        if (isValidSingleNumber(input, 1, 3)) {
            try {
                difficultyLevel = stoi(input); // Use the global variable
                system("cls");
                break;
            }
            catch (const out_of_range& e) {
                system("cls");
                showLoadingSpinner(1);
                cout << "Input tidak valid.\n";
            }
        }
        else {
            system("cls");
            showLoadingSpinner(1);
            cout << "Input tidak valid.\n";
        }
    }

    showLoadingSpinner(1); // Fungsi tunggu

    // Pemilihan jumlah kapal dengan validasi
    while (true) {
        cout << endl;
        cout << "Kapal yang akan dipasang (Maks = 100): ";
        getline(cin, input);

        // Mengecek input hanya angka antara 1 hingga 100
        if (isValidSingleNumber(input, 1, 100)) {
            try {
                shipCount = stoi(input);
                system("cls");
                break;
            }
            catch (const invalid_argument& e) {
                system("cls");
                showLoadingSpinner(1);
                cout << "Input tidak valid.\n";
            }
            catch (const out_of_range& e) {
                system("cls");
                showLoadingSpinner(1);
                cout << "Input tidak valid.\n";
            }
        }
        else {
            system("cls");
            showLoadingSpinner(1);
            cout << "Input tidak valid.\n";
        }
    }

    cout << "Tingkat Kesulitan : " << difficultyLevel << endl;
    cout << "Jumlah Kapal      : " << shipCount << "\n\n";

    showLoadingSpinner(3);
    system("cls");

    // Menyiapkan posisi acak tanpa tumpang tindih
    vector<pair<int, int>> positions;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            positions.push_back({ i, j });
        }
    }

    // Acak posisi dengan std::shuffle
    auto rng = std::default_random_engine(time(0));
    std::shuffle(positions.begin(), positions.end(), rng);

    // Membagi posisi yang tidak tumpang tindih untuk kapal pemain dan musuh
    vector<pair<int, int>> playerPositions(positions.begin(), positions.begin() + shipCount);
    vector<pair<int, int>> enemyPositions(positions.begin() + shipCount, positions.begin() + 2 * shipCount);

    placeShips(playerBoard, "[S]", playerPositions);
    placeShips(enemyBoard, "(S)", enemyPositions);

    // Tampilkan papan pemain dan musuh satu kali di awal
    displaySeparateBoards();

    // Lanjutkan permainan
    bool playerTurn = true;
    int x, y;

    while (true) {
        // Hanya perbarui tampilan papan setelah setiap giliran
        system("cls");
        displaySeparateBoards();
        displayScores();    

        if (playerTurn) {
            cout << "+-----------------------------------+\n";
            cout << "|        >> Giliran Player <<       |\n";
            cout << "+-----------------------------------+\n";
            cout << "Masukkan koordinat tembakan (x y): ";

            while (true) {
                string inputLine;
                getline(cin >> ws, inputLine); // Membaca seluruh baris input

                istringstream iss(inputLine);
                vector<int> coordinates;
                int coord;
                while (iss >> coord) {
                    coordinates.push_back(coord);
                }

                if (coordinates.size() != 2) {
                    cout << "Koordinat X dan Y (2 variabel): ";
                    continue;
                }

                y = coordinates[0];
                x = coordinates[1];

                if (x < 1 || x > GRID_SIZE || y < 1 || y > GRID_SIZE) {
                    cout << "Masukkan angka antara 1 hingga " << GRID_SIZE << " untuk koordinat (x y): ";
                }
                else {
                    break;
                }
            }
            x--; y--;

            if (makeMove(enemyBoard, x, y, "(S)", "(X)", "(o)")) {
                playerScore++;
                cout << "+-----------------------------------+\n";
                cout << "|               Hit!                |\n";
                cout << "+-----------------------------------+\n";
                showLoadingSpinner(2);
                system("cls");
            }
            else {
                cout << "+-----------------------------------+\n";
                cout << "|             Kamu Miss!            |\n";
                cout << "+-----------------------------------+\n";
                showLoadingSpinner(2);
                system("cls");
            }
            if (isGameOver(enemyBoard, "(S)")) {
                cout << "+-----------------------------------+\n";
                cout << "|            Player Menang!         |\n";
                cout << "+-----------------------------------+\n" << endl;
                displayScores();  // Tampilkan skor akhir
                break;
            }
        }
        else {
            enemyMove();

            if (isGameOver(playerBoard, "[S]")) {
                cout << "+-----------------------------------+\n";
                cout << "|            Musuh Menang!          |\n";
                cout << "+-----------------------------------+\n" << endl;
                displayScores();  // Tampilkan skor akhir
                break;
            }
        }

        playerTurn = !playerTurn;
    }

    cout << "Press any key to close...";
    getch(); // Tunggu pemain menekan tombol apa saja
    system("cls");

    return 0;
}