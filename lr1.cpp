#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;
/*
0	E' -> E
1	E -> E+T
2	E -> E-T
3	E -> T
4	T -> T*F
5	T -> T/F
6	T -> F
7	F -> (E)
8	F -> num
*/
map< pair< string, string >, int > generative_index;

void init_index () {
    generative_index.insert ( make_pair ( make_pair ( "E'", "E" ), 0 ) );
    generative_index.insert ( make_pair ( make_pair ( "E", "E+T" ), 1 ) );
    generative_index.insert ( make_pair ( make_pair ( "E", "E-T" ), 2 ) );
    generative_index.insert ( make_pair ( make_pair ( "E", "T" ), 3 ) );
    generative_index.insert ( make_pair ( make_pair ( "T", "T*F" ), 4 ) );
    generative_index.insert ( make_pair ( make_pair ( "T", "T/F" ), 5 ) );
    generative_index.insert ( make_pair ( make_pair ( "T", "F" ), 6 ) );
    generative_index.insert ( make_pair ( make_pair ( "F", "(E)" ), 7 ) );
    generative_index.insert ( make_pair ( make_pair ( "F", "n" ), 8 ) );
}

typedef map< string, map< string, string > > analyse_table;

class SyntaxAnalyzer {
public:
    void loadTable ( string file_name );
    void initFormula () { _formula = { { "E'", "E" }, { "E", "E+T" }, { "E", "E-T" }, { "E", "T" }, { "T", "T*F" }, { "T", "T/F" }, { "T", "F" }, { "F", "(E)" }, { "F", "n" } }; }
    void analyse ( string str );
    int  startWith ( string str, string start ) { return str.find ( start ) == 0; }

private:
    analyse_table                    _table;
    vector< pair< string, string > > _formula;
    void                             _printStack ( vector< string > stack, vector< string > state_stack, string str, string action ) { cout << action << endl; };
};

void SyntaxAnalyzer::loadTable ( string file_name ) {
    /*
    state,),-,n,/,+,(,*,$,E,T,F,
I0,0,0,s5,0,0,s4,0,0,1,2,3,
I1,0,s7,0,0,s6,0,0,acc,0,0,0,
I2,0,r3,0,s9,r3,0,s8,r3,0,0,0,
I3,0,r6,0,r6,r6,0,r6,r6,0,0,0,
I4,0,0,s14,0,0,s13,0,0,10,11,12,
I5,0,r8,0,r8,r8,0,r8,r8,0,0,0,
I6,0,0,s5,0,0,s4,0,0,0,15,3,
I7,0,0,s5,0,0,s4,0,0,0,16,3,
I8,0,0,s5,0,0,s4,0,0,0,0,17,
I9,0,0,s5,0,0,s4,0,0,0,0,18,
I10,s19,s21,0,0,s20,0,0,0,0,0,0,
I11,r3,r3,0,s23,r3,0,s22,0,0,0,0,
I12,r6,r6,0,r6,r6,0,r6,0,0,0,0,
I13,0,0,s14,0,0,s13,0,0,24,11,12,
I14,r8,r8,0,r8,r8,0,r8,0,0,0,0,
I15,0,r1,0,s9,r1,0,s8,r1,0,0,0,
I16,0,r2,0,s9,r2,0,s8,r2,0,0,0,
I17,0,r4,0,r4,r4,0,r4,r4,0,0,0,
I18,0,r5,0,r5,r5,0,r5,r5,0,0,0,
I19,0,r7,0,r7,r7,0,r7,r7,0,0,0,
I20,0,0,s14,0,0,s13,0,0,0,25,12,
I21,0,0,s14,0,0,s13,0,0,0,26,12,
I22,0,0,s14,0,0,s13,0,0,0,0,27,
I23,0,0,s14,0,0,s13,0,0,0,0,28,
I24,s29,s21,0,0,s20,0,0,0,0,0,0,
I25,r1,r1,0,s23,r1,0,s22,0,0,0,0,
I26,r2,r2,0,s23,r2,0,s22,0,0,0,0,
I27,r4,r4,0,r4,r4,0,r4,0,0,0,0,
I28,r5,r5,0,r5,r5,0,r5,0,0,0,0,
I29,r7,r7,0,r7,r7,0,r7,0,0,0,0,
    */
    vector< vector< string > > input = { { "0", "0", "s5", "0", "0", "s4", "0", "0", "1", "2", "3" },      { "0", "s7", "0", "0", "s6", "0", "0", "acc", "0", "0", "0" },
                                         { "0", "r3", "0", "s9", "r3", "0", "s8", "r3", "0", "0", "0" },   { "0", "r6", "0", "r6", "r6", "0", "r6", "r6", "0", "0", "0" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "10", "11", "12" }, { "0", "r8", "0", "r8", "r8", "0", "r8", "r8", "0", "0", "0" },
                                         { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "15", "3" },     { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "16", "3" },
                                         { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "0", "17" },     { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "0", "18" },
                                         { "s19", "s21", "0", "0", "s20", "0", "0", "0", "0", "0", "0" },  { "r3", "r3", "0", "s23", "r3", "0", "s22", "0", "0", "0", "0" },
                                         { "r6", "r6", "0", "r6", "r6", "0", "r6", "0", "0", "0", "0" },   { "0", "0", "s14", "0", "0", "s13", "0", "0", "24", "11", "12" },
                                         { "r8", "r8", "0", "r8", "r8", "0", "r8", "0", "0", "0", "0" },   { "0", "r1", "0", "s9", "r1", "0", "s8", "r1", "0", "0", "0" },
                                         { "0", "r2", "0", "s9", "r2", "0", "s8", "r2", "0", "0", "0" },   { "0", "r4", "0", "r4", "r4", "0", "r4", "r4", "0", "0", "0" },
                                         { "0", "r5", "0", "r5", "r5", "0", "r5", "r5", "0", "0", "0" },   { "0", "r7", "0", "r7", "r7", "0", "r7", "r7", "0", "0", "0" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "25", "12" },  { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "26", "12" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "0", "27" },   { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "0", "28" },
                                         { "s29", "s21", "0", "0", "s20", "0", "0", "0", "0", "0", "0" },  { "r1", "r1", "0", "s23", "r1", "0", "s22", "0", "0", "0", "0" },
                                         { "r2", "r2", "0", "s23", "r2", "0", "s22", "0", "0", "0", "0" }, { "r4", "r4", "0", "r4", "r4", "0", "r4", "0", "0", "0", "0" },
                                         { "r5", "r5", "0", "r5", "r5", "0", "r5", "0", "0", "0", "0" },   { "r7", "r7", "0", "r7", "r7", "0", "r7", "0", "0", "0", "0" } };
    ifstream                   file ( file_name );
    if ( !file.is_open () ) {
        cout << "Error: file " << file_name << " not found" << endl;
        return;
    }
    analyse_table    table;
    string           line;
    vector< string > tokens = { ")", "-", "n", "/", "+", "(", "*", "$", "E", "T", "F" };
    getline ( file, line );
    string word;

    while ( getline ( file, line ) ) {
        string word;
        int    count = 0;
        string state;
        for ( int i = 0; i < line.size (); i++ ) {
            if ( line[ i ] == ',' ) {
                if ( count == 0 ) {
                    table.insert ( make_pair ( word, map< string, string > () ) );
                    state = word;
                    count++;
                } else if ( word != string ( "0" ) ) {
                    table[ state ].insert ( make_pair ( tokens[ count - 1 ], word ) );
                    count++;
                } else {
                    count++;
                }
                word.clear ();
            } else {
                word += line[ i ];
            }
        }
    }
    file.close ();
    _table = table;
}


void SyntaxAnalyzer::analyse ( string str ) {
    int              ip = 0;
    vector< string > state_stack;
    vector< string > stack;
    state_stack.push_back ( "I0" );
    str.push_back ( '$' );
    while ( true ) {
        int    ipAdd = 0;
        string state = state_stack.back ();
        string a;
        if ( startWith ( str.substr ( ip ), "n" ) ) {
            a = "n";
            ipAdd += 1;
        } else {
            a = str[ ip ];
            ipAdd = 1;
        }
        if ( _table.find ( state ) == _table.end () ) {
            cout << "Error: state " << state << " not found" << endl;
            return;
        }
        if ( _table[ state ].find ( a ) == _table[ state ].end () ) {
            cout << "Error: Unexpected token " << a << " in state " << state << ", skip..." << endl;
            ip += ipAdd;
            continue;
            return;
        }
        string next_state = _table[ state ][ a ];
        if ( startWith ( next_state, "s" ) || startWith ( next_state, "S" ) ) {
            _printStack ( stack, state_stack, str.substr ( ip ), "shift" );
            stack.push_back ( a );
            state_stack.push_back ( "I" + next_state.substr ( 1 ) );
            ip += ipAdd;
        } else if ( startWith ( next_state, "r" ) ) {
            int counti = 0;
            int r = stoi ( next_state.substr ( 1 ) );
            int index = generative_index.find ( _formula[ r - 1 ] )->second;
            _printStack ( stack, state_stack, str.substr ( ip ), to_string ( index ) );
            counti = _formula[ r - 1 ].second.size ();
            for ( int i = 0; i < counti; i++ ) {
                stack.pop_back ();
                state_stack.pop_back ();
            }
            stack.push_back ( _formula[ r - 1 ].first );
            state_stack.push_back ( "I" + _table[ state_stack.back () ][ _formula[ r - 1 ].first ] );
        } else if ( next_state == string ( "acc" ) ) {
            _printStack ( stack, state_stack, str.substr ( ip ), "accept" );
            return;
        } else {
            cout << "Error: Unexpected state " << next_state << endl;
            return;
        }
    }
}

string process ( string str ) {
    string newStr;
    int    isNum = 0;
    for ( int i = 0; i < str.size (); i++ ) {
        if ( str[ i ] == 'n' ) {
            if ( !isNum ) {
                isNum = 1;
                newStr += "n";
            }
            continue;
        } else {
            isNum = 0;
            newStr += str[ i ];
        }
    }
    return newStr;
}

int main () {
    init_index ();
    SyntaxAnalyzer analyzer;
    analyzer.loadTable ( "lr1.csv" );
    string str;
    cin >> str;
    str = process ( str );
    analyzer.analyse ( str );
    return 0;
}


// 改csv的读入