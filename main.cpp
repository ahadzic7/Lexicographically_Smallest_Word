#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>


const char alphabet[] = {'a', 'b', 'c', 'd', 'e'};

typedef std::pair<int, std::string> State;

void readData(std::vector<std::list<State>> &graph, std::vector<bool> &final, int nStates, int alphabetSize) {
    std::string line;
    for(int i = 0; i < nStates; i++) {
        getline (std::cin, line);

        std::stringstream ss;
        ss << line;

        //std::vector<std::vector<int>> tab(alphabetSize, std::vector<int>());

        int id;
        ss >> id >> std::ws;

        char type;
        ss >> type >> std::ws;

        if(type == 'F')
            final[id] = true;

        //transitions
        std::string temp;
        ss >> temp >> std::ws;
        std::string character(temp);

        for(int j = 0; j < alphabetSize; j++) {
            while(!ss.eof()) {
                int transition;
                ss >> temp >> std::ws;
                if(!(std::stringstream(temp) >> transition))
                    break;
                graph[id].emplace_back(transition, character);
                //tab[j].push_back(transition);
            }
            character = temp;
        }

//        std::cout << type << " " << id << ") ";
//        for(int j = 0; j < alphabetSize; j++) {
//            std::cout << alphabet[j] << " ";
//            for(int el: tab[j]) {
//                std::cout << el << " ";
//            }
//        }
//        std::cout << std::endl;
    }
}

bool substringCheck(const std::string &word, const std::string &substring) {
    if(word.size() < substring.size())
        return false;

    for(int i = 0; i < word.size(); i++) {
        if(word[i] == substring[0]) {
            bool found(true);
            for(int j = 0; j < substring.size(); j++) {
                if(word[i + j] != substring[j])
                    found = false;
            }
            if(found)
                return true;
        }
    }
    return false;
}

int suffixSearch(const std::string &newWord, const std::string &substring) {
    int size(newWord.size());
    int move(0);
    if(newWord.size() > substring.size()) {
        size = substring.size();
        move = newWord.size() - size;
    }

    for (int j = size, offset = 0; j >= 1; j--, offset++) {
        bool ssfound(true);
        for (int k = 0; k < j; k++) {
            char x = newWord[move + k + offset];
            char y = substring[k];

            if (x != y) {
                ssfound = false;
                break;
            }
        }

        if (ssfound) {
            return j;
        }
    }
    return -1;
}

std::string lexMinWord(std::vector<std::list<State>> &graph, std::vector<bool> &final, const int nStates, const std::string &substring) {
    std::vector<std::string> acceptedWords;
    std::vector<int> oldCurrentState(1), newCurrentState;
    std::vector<std::string> oldWords(1, ""), newWords;
    std::vector<std::vector<bool>> oldVisited, newVisited;
    std::vector<bool> oldHasSubstring(1, false), newHasSubstring;

    std::vector<std::string> prefixes(nStates);

    std::vector<bool> visited(nStates, false);
    visited[0] = true;
    oldVisited.emplace_back(visited);

    int it = 0;
    while(true) {
        //if(it == 10) break;
        it++;
        std::cout << it << std::endl;

        std::vector<bool>join(nStates, false);

        bool visiting(false);


        for(int i = 0; i < oldWords.size(); i++) {
            int currentState(oldCurrentState[i]);
            std::string currentWord(std::move(oldWords[i]));
            std::vector<bool> currentVisited(std::move(oldVisited[i]));
            bool hasSubstring(oldHasSubstring[i]);

            for(const State &newState: graph[currentState]) {
                std::string newWord(currentWord + newState.second);


                if(prefixes[newState.first].size() == newWord.size()) {
                    if(newWord >= prefixes[newState.first]) {
                        int suffixLength(suffixSearch(newWord, substring));
                        if(suffixLength == -1) {
                            //no suffix the word is useless
                            continue;
                        }
                        else if(suffixLength == substring.size()) {
                            //the word has the substring
                        }
                        else {
                            //check if the next state is possibility to continue
                            bool found(false);
                            for(const State &s: graph[newState.first]) {
                                if(substring[suffixLength + 1] == s.second[0]) {
                                    found = true;
                                }
                            }
                            if(!found) {
                                //word skipped bcuz the pattern breaks in the next iteration
                                continue;
                            }
                        }
                    }
                }
                else {
                    prefixes[newState.first] = newWord;
                }

                std::cout << newWord << "\n";

                if(!currentVisited[newState.first]) {
                    visiting = true;
                    std::vector<bool> tmpVisited(currentVisited);
                    tmpVisited[newState.first] = true;

                    if(final[newState.first]) {
                        acceptedWords.emplace_back(newWord);
                        if(substringCheck(newWord, substring)) {
                            return newWord;
                        }
                    }

                    newCurrentState.emplace_back(newState.first);
                    newWords.emplace_back(newWord);
                    newVisited.emplace_back(tmpVisited);
                    newHasSubstring.emplace_back(false);
                }
                else {
                    //checking if we need to enter a cycle
                    if(!hasSubstring) {
                        int suffixLength(suffixSearch(newWord, substring));
                        if(suffixLength == -1) {
                            //no suffix do not enter
                            continue;
                        }
                        else if(suffixLength == substring.size()) {
                            //substring found at the end
                            if(final[newState.first]) {
                                acceptedWords.emplace_back(newWord);
                                return newWord;
                            }
                            //mark as has substring
                            newHasSubstring.emplace_back(true);
                        }
                        else
                            newHasSubstring.emplace_back(false);
                    }
                    else {
                        //already has the substring need to continue searching
                        if(final[newState.first]) {
                            acceptedWords.emplace_back(newWord);
                            return newWord;
                        }
                        newHasSubstring.emplace_back(true);
                    }
                    //new word is possible put it in
                    visiting = true;

                    newCurrentState.emplace_back(newState.first);
                    newWords.emplace_back(newWord);
                    newVisited.emplace_back(currentVisited);

                }
            }

        }

        if(!visiting)
            break;

        std::swap(oldCurrentState, newCurrentState);
        std::swap(oldWords, newWords);
        std::swap(oldVisited, newVisited);
        std::swap(oldHasSubstring, newHasSubstring);

        newCurrentState.resize(0);
        newWords.resize(0);
        newVisited.resize(0);
        newHasSubstring.resize(0);

    }

    int x = 0;
    if(acceptedWords.empty())
        return "fuck";
    return acceptedWords[acceptedWords.size() - 1];
}

void solution() {
    std::ios::sync_with_stdio(false);

    int nStates, alphabetSize;
    std::cin >> nStates >> alphabetSize;
    std::cin >> std::ws;

    std::vector<std::list<State>> graph(nStates);
    std::vector<bool> final(nStates, false);

    readData(graph, final, nStates, alphabetSize);

    std::string subs;
    std::cin >> subs;

    std::string s (lexMinWord(graph, final, nStates, subs));

    std::cout << s;

    std::ios::sync_with_stdio(true);
}

int main() {
    clock_t start, end;
    start = clock();

    solution();

    end = clock();

    std::cout << "\nExecution time: " << (end - start) / 1000.;

    return 0;
}
