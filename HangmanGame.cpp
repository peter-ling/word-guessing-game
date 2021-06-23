/*
* This file contains a c++ game run in terminal. This game is very similar to hangman, 
* where a user can guess characters of a word/phrase and eventually make a guess before 
* they run out of wrong guesses.
*
* Creating this game helped me harness my skills in c++ while learning how to utilize input/output and
* reading/extracting data from files, It stregnthened my understanding in using data types such as vectors
* and unordered sets
* 
* Peter Ling, 2021
*/

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;
using std::string;
using std::vector; 

/**
 * @brief  function that, given a char, checks if it is in the word, and if so, updates hiddenword (unless no longer in curAlpha)
 * @param  word: the string that the user is trying to guess 
 * @param  &hiddenWord: the string that shows what characters have currently been guessed. passed by reference so function can update it 
 * @param  ch: the character that is being checked for 
 * @param  &curAlpha: an unordered_set of characters representing which lettes have not yet been guessed 
 * @retval true if ch was found in word, false if not 
 */

bool checkWordUpdate(string word, string &hiddenWord, char ch, unordered_set<char> &curAlpha){
    if (curAlpha.find(ch) == curAlpha.end()){  // having this here avoids the O(N) operation by the while loop
        return false;
    }
    bool found; 
    int curPos = word.find(ch);
    while (curPos != string::npos){
        hiddenWord[curPos] = word[curPos]; 
        found = true; 
        curPos = word.find(ch, curPos + 1); 
    }
    if (found) return true; 
    return false;
}

/**
 * @brief  handles the user input for recieivng a character when guessing a char of the hiddenWord
 * @note   
 * @retval the guessed character in lowercase 
 */
char getLetterGuess(){

    bool fail;
    string guess; 

    do {
        fail = false;
        cout << "Guess a letter (or press 0 to guess the word): "; 
        getline(cin, guess);
        cout << "\n";
        if (guess[0] == '0') return '0';
        
        if (cin.fail() || (!isalpha(guess[0]) && guess[0] != 0) || guess.size() != 1){
            fail = true; 
            cin.clear();
            cout << "Improper input--must input an alphabetical character." << endl;
        }
    } while (fail);
    cin.ignore();
    return tolower(guess[0]);  // return that character in a lowercase form so it matches the characters in the word 
}

/**
 * @brief handles the user input for receiving a string when they want to guess the word 
 * @retval the string put in all lowercase to match the format of the words in the .txt files 
 */
string getWordGuess(){
    bool fail; 
    string guess; 
    do {
        fail = false; 
        cout << "Guess the word: ";
        getline(cin, guess);

        if (cin.fail()){
            fail = true; 
            cin.clear(); 
            //TODO would i need cin.clear and cin.ignore here 
            cout << "Improper input--must be a word." << endl; 
        }
    } while (fail);

    transform(guess.begin(), guess.end(), guess.begin(), ::tolower);  // put the guess in all lowercase so it matches the word 
    cin.ignore(); 
    return guess; 
}

/**
 * @brief  Funciton to initialize the alphabet unordered_set
 * @note   Will be used to keep track of which letters the user has not yet guessed. 
 *         Had to do it with the .insert() function because my compiler would not support initilizer list
 * @retval an unordered_set containing each letter of the alphabet
 */
unordered_set<char> initCurAlpha(){
    unordered_set<char> curAlpha; 
    curAlpha.insert('a');
    curAlpha.insert('b');
    curAlpha.insert('c');
    curAlpha.insert('d');
    curAlpha.insert('e');
    curAlpha.insert('f');
    curAlpha.insert('g');
    curAlpha.insert('h');
    curAlpha.insert('i');
    curAlpha.insert('j');
    curAlpha.insert('k');
    curAlpha.insert('l');
    curAlpha.insert('m');
    curAlpha.insert('n');
    curAlpha.insert('o');
    curAlpha.insert('p');
    curAlpha.insert('q');
    curAlpha.insert('r');
    curAlpha.insert('s');
    curAlpha.insert('t');
    curAlpha.insert('u');
    curAlpha.insert('v');
    curAlpha.insert('w');
    curAlpha.insert('x');
    curAlpha.insert('y');
    curAlpha.insert('z');

    return curAlpha; 
}

/**
 * @brief  given a word, initialize the hiddenWord that shows which letters the user has not yet found 
 * @param  word: the word/phrase to be recreated 
 * @retval this new string of the hiddenWord
 */
string initHiddenWord(string word){
    int wordLength = word.length(); 
    string hiddenWord;
    for (int i = 0; i < wordLength; i++){

        if (word[i] == ' ' || word[i] == '-' || word[i] == '.'){  // user will not have to guess these characters 
            hiddenWord += word[i];
        } else {
            hiddenWord += '/'; 
        }
    }
    return hiddenWord;
}
/**
 * @brief  Welcome the user to the game and receive their choice for word category 
 * @retval an int which will later be coorespoinding to a text file 
 */
int introInitVector(){
    cout << "Welcome to the word guessing game!\nThe goal is to find out the hidden phrase/word by guessing the letters." << endl;
    cout << "First, select a category!" << endl; 
    int category = 0; 
    int numCategories = 4; 
    bool fail; 
    cout << "U.S. Cities: Press 1\nU.S. States: Press 2\nSports Vocabulary: Press 3\nSports Teams: Press 4" << endl; 

    do {
        fail = false; 
        cout << "Enter category choice here: "; 
        cin >> category; 

        if (cin.fail() || category < 1 || category > numCategories){
            cin.clear(); 
            cin.ignore();  
            cout << "Improper input--please select a number from 1-" << numCategories << "." << endl;
            fail = true;  
        }
    } while (fail);

    cin.ignore();  // clear out the buffer 
    return category; 
}

/**
 * @brief  function to initizlize the number of misses the user can have by asking them what 'level' they want to play on
 * @retval an int representing the number of missed attempts allowed for this round
 */
int initMisses(){
    
    cout << "Select what level you want to play on! " << endl; 
    int level = 0; 
    const int NUM_LEVELS = 3; 
    const int EASY_MISSES = 10; 
    bool fail;
    cout << "Easy (10 misses): Press 1\nMedium (6 misses): Press 2\nHard (4 Misses): Press 3" << endl;  
    // try and do something with ${EASY_MISSES}

    do {
        fail = false; 
        cout << "Level selection: "; 
        cin >> level; 

        if (cin.fail() || level < 1 || level > NUM_LEVELS){
            fail = true; 
            cin.clear(); 
            cout << "Improper input--please input a number associated with one of the levels" << endl; 
        }
        cin.ignore(1, '\n');
    } while (fail);

    switch (level){
        case 1:
            return 10;  // try to get rid of these magic numbers 
        
        case 2: 
            return 6;
        
        case 3: 
            return 4;
    }
    return -1;  
}

/**
 * @brief  initialize the map containing numbers paired with text file names 
 * @note   
 * @retval return the map that was initialized 
 */
map<int, string> initFileSelections(){
    map<int, string> fileSelections;
    fileSelections.insert( {1, "uscities.txt"} );
    fileSelections.insert( {2, "usstates.txt"} );
    fileSelections.insert( {3, "sports.txt"} );
    fileSelections.insert( {4, "sportsteams.txt"} );

    return fileSelections; 
}

int main(){
    map<int, string> fileSelections = initFileSelections(); 
    int selection = introInitVector();
    ifstream in(fileSelections[selection]); 
    vector<string> words; 

    if (!in){
        cout << "Error opening the file." << endl; 
        exit(1); 
    }
    string curWord; 

    while (getline(in, curWord)){
        transform(curWord.begin(), curWord.end(), curWord.begin(), ::tolower);
        words.push_back(curWord);
    }

    in.close(); 
    unordered_set<char> curAlpha = initCurAlpha(); 
    srand(time(0));
    int randIndex = rand() % words.size(); 
    string word = words[rand() % words.size()];  
    string hiddenWord = initHiddenWord(word);

    int missesRemaining = initMisses(); 
    bool found;  
    while (hiddenWord != word && missesRemaining > 0){
        cout << "\nWrong guesses remaining: "; 
        cout << missesRemaining << endl; 
        cout << hiddenWord << endl; 
        char guess = getLetterGuess(); 
        found = checkWordUpdate(word, hiddenWord, guess, curAlpha);  

        if (found){
            cout << "Yes! The letter '" << guess << "' is in the word." << endl; 
        } else {
            if (guess == '0'){  // wants to guess the word 
                if (word == getWordGuess()){  // having issues
                    break;
                } else {
                    cout << "Incorrect guess." << endl;
                    missesRemaining--;
                }
            } else if (curAlpha.find(guess) == curAlpha.end()){
                cout << "The character '" << guess << "' has already been guessed. Make another selection." << endl;
            } else {
                missesRemaining--; 
                cout << "The letter '" << guess << "' is not in the hidden word. " << endl; 
                }
        }
        curAlpha.erase(guess);        
    }

    if (missesRemaining != 0){  // user has won
        cout << "Congrats! You found the hidden word: '" << word << "'!" << endl; 
        cout << "Thank you for playing! Hope you play again soon!" << endl; 

    } else {
        cout << "You lose. You were not able to guess the word '" << word << "'. Play again soon!'" << endl; 
    }

}