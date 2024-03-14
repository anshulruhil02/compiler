#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

void printParse(deque<string> const & input_sequence, deque<string> const & parseSequence);

int main() {
    int count = 0;
    string line, lhs, rhs;
    map<int,pair<string, string>> cfg;
    map<pair<int, string>, int> transitions;
    map<pair<int, string>, int> reductions;
    deque<string> input_sequence;
    deque<string> parseSequence;
    deque<int> stateStack;
    deque<string> symbolStack;
    int shiftCounter = 0;
    while(getline(cin, line) && line != ".INPUT"){
        if(line == ".CFG") continue;
        istringstream iss(line);
        iss>>lhs;
        getline(iss, rhs);
        cfg[count] = make_pair(lhs, rhs);
        count++;
    }
    
    while(getline(cin, line) && line != ".TRANSITIONS"){
        if(line == ".INPUT") continue;
        istringstream iss(line);
        string symbol;
        while(iss>>symbol) input_sequence.push_back(symbol);
    }

    while(getline(cin, line) && line != ".REDUCTIONS"){
        if(line == ".TRANSITIONS") continue;
        istringstream iss(line);
        int currentState, nextState;
        string transitionSymbol;
        iss>>currentState>>transitionSymbol>>nextState;
        transitions[make_pair(currentState, transitionSymbol)] = nextState;
    }

    while(getline(cin, line) && line != ".END"){
        if(line == ".REDUCTIONS") continue;
        istringstream iss(line);
        int reductionState, productionRuleNo;
        string symbol;
        iss>>reductionState>>productionRuleNo>>symbol;
        reductions[make_pair(reductionState, symbol)] = productionRuleNo;
    }
    
    stateStack.push_back(0);
    int currentState = stateStack.back();
    printParse(input_sequence, parseSequence);
    //cout<<"State : "<<currentState<<endl;
    while (!input_sequence.empty()) {
        string currentSymbol = input_sequence.front();  
        shiftCounter++;
        // check for reduction
        bool isReducable;
        isReducable = false;
        int currentState = stateStack.back();
        
        do
        {   
            auto reduceIt = reductions.find(make_pair(currentState, currentSymbol));
            if(reduceIt != reductions.end()) isReducable = true;
            else break;
            //cout<<"I am Reducable "<<currentState<<endl;
            int productionRuleNo = reduceIt->second;
            string lookupSymbol = reduceIt->first.second;

            // Get the production rule from the CFG
            auto rule = cfg[productionRuleNo];
            string lhs = rule.first;  // This is the non-terminal on the left-hand side of the production
            string rhs = rule.second;
            //cout<<productionRuleNo<<":"<<lhs<< " -> "<<rhs<<endl;
            if(rhs == " .EMPTY") {
                symbolStack.push_back(lhs);
                parseSequence.push_back(lhs);
            }else{
                string word;
                stringstream rhsBreakup(rhs);
                //cout<<"Popping symbol: ";
                while(rhsBreakup >> word){
                    //cout<<symbolStack.back()<<" ";
                    symbolStack.pop_back();
                    parseSequence.pop_back();
                    stateStack.pop_back();
                }
                //cout<<endl;
                symbolStack.push_back(lhs);
                parseSequence.push_back(lhs);
                //cout<<"pushed symbol: "<<symbolStack.back()<<endl;
            }
            currentState = stateStack.back();
            auto transitionIt = transitions.find(make_pair(currentState, lhs));
            //cout<<"Reduced back to State: "<<currentState<<endl;
            if(transitionIt != transitions.end()) {
                stateStack.push_back(transitionIt->second);
                currentState = stateStack.back();
                //cout<<"State : "<<currentState<<endl;
                isReducable = true;
            }else {
                cerr << "ERROR: No valid transition after reduction." << endl;
                exit(EXIT_FAILURE);
            }
            printParse(input_sequence, parseSequence);        
        }while (isReducable);
        input_sequence.pop_front();
        symbolStack.push_back(currentSymbol);
        parseSequence.push_back(currentSymbol);
        printParse(input_sequence, parseSequence);
        if (transitions.find(make_pair(stateStack.back(), currentSymbol)) == transitions.end()) {
            cerr << "ERROR at "<<shiftCounter<< endl;
            break;
        }
        auto transitionIt = transitions.find(make_pair(stateStack.back(), currentSymbol));
        stateStack.push_back(transitionIt->second);
        currentState = stateStack.back();
        //cout<<"State : "<<currentState<<endl;
    }

    if(symbolStack.back() == "EOF") cout<<cfg[0].first<<" ."<<endl;
}  

void printParse(const deque<string>& input_sequence, const deque<string>& parseSequence) {
    for (const auto& element : parseSequence) cout << element << " ";
    cout << ". ";
    for (const auto& element : input_sequence) cout << element << " ";
    cout<<endl;
}
