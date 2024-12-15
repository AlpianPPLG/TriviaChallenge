#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <chrono>

using namespace std;

struct Question {
    string question;
    vector<string> options;
    char correctAnswer;
    string difficulty; // Menambahkan tingkat kesulitan
    string hint; // Menambahkan hint
};

void displayLoading(const string& message) {
    cout << message << " ";
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << endl;
}

void displayQuestion(const Question& q, int questionNumber) {
    cout << "\nQuestion " << questionNumber << ": " << q.question << endl;
    for (size_t i = 0; i < q.options.size(); ++i) {
        cout << static_cast<char>('A' + i) << ". " << q.options[i] << endl;
    }
    cout << "Your answer (A/B/C/D) or H for hint: ";
}

bool isValidAnswer(char answer) {
    return answer == 'A' || answer == 'B' || answer == 'C' || answer == 'D' || answer == 'H';
}

void playQuiz(vector<Question> questions, int numQuestions, const string& playerName) {
    int score = 0;
    int questionNumber = 1;

    // Randomize question order
    random_shuffle(questions.begin(), questions.end());
    questions.resize(min(numQuestions, static_cast<int>(questions.size()))); // Limit questions

    for (const auto& q : questions) {
        displayQuestion(q, questionNumber);

        char userAnswer;
        bool answered = false;

        // Timer thread
        thread timerThread([&answered] {
            this_thread::sleep_for(chrono::seconds(10)); // Waktu terbatas 10 detik
            if (!answered) {
                cout << "\nTime's up! You didn't answer in time.\n";
            }
        });

        cin >> userAnswer;
        answered = true; // Menandakan jawaban sudah diberikan
        timerThread.detach(); // Menghentikan thread timer

        userAnswer = toupper(userAnswer);

        // Validate input
        while (!isValidAnswer(userAnswer)) {
            cout << "Invalid answer! Please choose A, B, C, D, or H for hint: ";
            cin >> userAnswer;
            userAnswer = toupper(userAnswer);
        }

        if (userAnswer == 'H' && answered) {
            cout << "Hint: " << q.hint << endl;
            cout << "Your answer (A/B/C/D): ";
            cin >> userAnswer;
            userAnswer = toupper(userAnswer);
        }

        if (userAnswer == q.correctAnswer) {
            cout << "Correct!\n";
            ++score;
        } else {
            cout << "Wrong! The correct answer was " << q.correctAnswer << ".\n";
        }

        ++questionNumber;
    }

    // Menampilkan skor berdasarkan jumlah soal yang dijawab benar
    cout << "\nQuiz finished! Your score: " << score << "/" << numQuestions 
         << " (" << static_cast<double>(score) / numQuestions * 100 << "% correct)\n";
    
    // Menyapa pemain dengan nama dan skor
    cout << "Congratulations, " << playerName << "! Your score is " << score << ".\n";
}

void displayPolicy() {
    cout << "=== Quiz Game Policy ===\n";
    cout << "1. All questions are designed to be educational and informative.\n";
    cout << "2. Players are encouraged to answer honestly and to the best of their ability.\n";
    cout << "3. Players have the option to use hints if they are unsure of an answer.\n";
    cout << "4. The game is intended for fun and learning purposes only.\n";
    cout << "5. Players can choose to play again or exit the game at any time.\n";
    cout << "=========================\n\n";
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    string playerName;
    cout << "Enter your name: ";
    getline(cin, playerName); // Input nama pemain

    // Display loading
    displayLoading("Loading game policy");

    // Display policy
    displayPolicy();

    // Questions categorized by topics
    unordered_map<string, vector<Question>> categorizedQuestions = {
        {"Geography", {
            {"What is the capital of France?", {"Paris", "London", "Berlin", "Madrid"}, 'A', "Easy", "It's also known as the City of Light."},
            {"Which country has the largest population?", {"China", "India", "USA", "Indonesia"}, 'A', "Easy", "It has over a billion people."},
            {"What is the longest river in the world?", {"Nile", "Amazon", "Yangtze", "Mississippi"}, 'B', "Medium", "It's in South America."}
        }},
        {"Science", {
            {"Which planet is known as the Red Planet?", {"Earth", "Mars", "Jupiter", "Venus"}, 'B', "Easy", "It's named after the Roman god of war."},
            {"What is the chemical symbol for water?", {"H2", "O2", "H2O", "HO2"}, 'C', "Easy", "It's composed of two hydrogen atoms and one oxygen atom."},
            {"What is the speed of light?", {"300,000 km/s", "150,000 km/s", "450,000 km/s", "600,000 km/s"}, 'A', "Hard", "It's the fastest speed in the universe."}
        }},
        {"Literature", {
            {"Who wrote 'Hamlet'?", {"Shakespeare", "Dickens", "Hemingway", "Orwell"}, 'A', "Medium", "He is one of the greatest playwrights."},
            {"What is the first book of the Bible?", {"Genesis", "Exodus", "Leviticus", "Numbers"}, 'A', "Easy", "It tells the creation story."},
            {"Who wrote 'Pride and Prejudice'?", {"Charlotte Bronte", "Jane Austen", "Emily Bronte", "George Eliot"}, 'B', "Medium", "She is known for her novels about the British gentry."}
        }},
        {"Math", {
            {"What is 2 + 2?", {"3", "4", "5", "6"}, 'B', "Easy", "It's the first even number."},
            {"What is the square root of 16?", {"2", "3", "4", "5"}, 'C', "Easy", "It's a whole number."},
            {"What is 10 / 2?", {"3", "4", "5", "6"}, 'C', "Easy", "It's half of ten."}
        }}
    };

    cout << "Welcome to the Quiz Game, " << playerName << "!\n";
    cout << "You will be asked a series of questions. Choose the correct answer (A/B/C/D) or H for a hint.\n\n";

    char playAgain;
    do {
        cout << "Choose a quiz mode:\n";
        cout << "1. By Category\n";
        cout << "2. Random from All Categories\n";
        cout << "Select an option (1/2): ";
        int choice;
        cin >> choice;

        vector<Question> allQuestions;
        
        // Collect all questions from all categories
        for (const auto& category : categorizedQuestions) {
            allQuestions.insert(allQuestions.end(), category.second.begin(), category.second.end());
        }

        // Choose number of questions
        int numQuestions;
        cout << "Enter the number of questions you want to attempt (max "
             << allQuestions.size() << "): ";
        cin >> numQuestions;

        // Validate number of questions
        if (numQuestions < 1 || numQuestions > allQuestions.size()) {
            cout << "Invalid number of questions! Please try again.\n";
            continue;
        }

        if (choice == 1) {
            // Choose category
            string selectedCategory;
            cout << "Available categories:\n";
            for (const auto& [category, _] : categorizedQuestions) {
                cout << "- " << category << endl;
            }
            cout << "Enter your preferred category: ";
            cin >> selectedCategory;

            // Check if the category is valid
            if (categorizedQuestions.find(selectedCategory) == categorizedQuestions.end()) {
                cout << "Invalid category! Please try again.\n";
                continue;
            }

            // Filter questions based on selected category
            vector<Question> filteredQuestions;
            for (const auto& q : categorizedQuestions[selectedCategory]) {
                filteredQuestions.push_back(q);
            }

            // Play the quiz with filtered questions
            displayLoading("Loading quiz");
            playQuiz(filteredQuestions, numQuestions, playerName);
        } else if (choice == 2) {
            // Play with random questions from all categories
            displayLoading("Loading quiz");
            playQuiz(allQuestions, numQuestions, playerName);
        } else {
            cout << "Invalid choice! Please try again.\n";
            continue;
        }

        cout << "\nDo you want to play again? (Y/N): ";
        cin >> playAgain;
        playAgain = toupper(playAgain);
    } while (playAgain == 'Y');

    cout << "Thank you for playing, " << playerName << "!\n";

    return 0;
}
