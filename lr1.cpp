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
    void loadTable ();
    void initFormula () { _formula = { { "E'", "E" }, { "E", "E+T" }, { "E", "E-T" }, { "E", "T" }, { "T", "T*F" }, { "T", "T/F" }, { "T", "F" }, { "F", "(E)" }, { "F", "n" } }; }
    void analyse ( string str );
    int  startWith ( string str, string start ) { return str.find ( start ) == 0; }

private:
    analyse_table                    _table;
    vector< pair< string, string > > _formula;
};

void Error () {
    cout << "error" << endl;
    exit ( 0 );
}

void SyntaxAnalyzer::loadTable () {
    vector< vector< string > > input = { { "0", "0", "s5", "0", "0", "s4", "0", "0", "1", "2", "3", "30" }, { "0", "s7", "0", "0", "s6", "0", "0", "acc", "0", "0", "0" },
                                         { "0", "r3", "0", "s9", "r3", "0", "s8", "r3", "0", "0", "0" },    { "0", "r6", "0", "r6", "r6", "0", "r6", "r6", "0", "0", "0" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "10", "11", "12" },  { "0", "r8", "0", "r8", "r8", "0", "r8", "r8", "0", "0", "0" },
                                         { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "15", "3" },      { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "16", "3" },
                                         { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "0", "17" },      { "0", "0", "s5", "0", "0", "s4", "0", "0", "0", "0", "18" },
                                         { "s19", "s21", "0", "0", "s20", "0", "0", "0", "0", "0", "0" },   { "r3", "r3", "0", "s23", "r3", "0", "s22", "0", "0", "0", "0" },
                                         { "r6", "r6", "0", "r6", "r6", "0", "r6", "0", "0", "0", "0" },    { "0", "0", "s14", "0", "0", "s13", "0", "0", "24", "11", "12" },
                                         { "r8", "r8", "0", "r8", "r8", "0", "r8", "0", "0", "0", "0" },    { "0", "r1", "0", "s9", "r1", "0", "s8", "r1", "0", "0", "0" },
                                         { "0", "r2", "0", "s9", "r2", "0", "s8", "r2", "0", "0", "0" },    { "0", "r4", "0", "r4", "r4", "0", "r4", "r4", "0", "0", "0" },
                                         { "0", "r5", "0", "r5", "r5", "0", "r5", "r5", "0", "0", "0" },    { "0", "r7", "0", "r7", "r7", "0", "r7", "r7", "0", "0", "0" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "25", "12" },   { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "26", "12" },
                                         { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "0", "27" },    { "0", "0", "s14", "0", "0", "s13", "0", "0", "0", "0", "28" },
                                         { "s29", "s21", "0", "0", "s20", "0", "0", "0", "0", "0", "0" },   { "r1", "r1", "0", "s23", "r1", "0", "s22", "0", "0", "0", "0" },
                                         { "r2", "r2", "0", "s23", "r2", "0", "s22", "0", "0", "0", "0" },  { "r4", "r4", "0", "r4", "r4", "0", "r4", "0", "0", "0", "0" },
                                         { "r5", "r5", "0", "r5", "r5", "0", "r5", "0", "0", "0", "0" },    { "r7", "r7", "0", "r7", "r7", "0", "r7", "0", "0", "0", "0" },
                                         { "0", "0", "0", "0", "0", "0", "0", "acc", "0", "0", "0" } };

    analyse_table    table;
    string           line;
    vector< string > tokens = { ")", "-", "n", "/", "+", "(", "*", "$", "E", "T", "F", "E'" };

    for ( int i = 0; i < input.size (); i++ ) {
        int    col = 0;
        string state;
        for ( const auto& word : input[ i ] ) {
            if ( col == 0 ) {    // state
                state = "I" + to_string ( i );
                table.insert ( make_pair ( state, map< string, string > () ) );
            }
            if ( word != "0" ) {
                table[ state ].insert ( make_pair ( tokens[ col ], word ) );
            }
            col++;
        }
    }
    _table = table;
}


void SyntaxAnalyzer::analyse ( string str ) {
    int              ip = 0;
    int              r = 0;
    vector< string > state_stack;
    vector< string > stack;
    state_stack.push_back ( "I0" );
    str.push_back ( '$' );
    while ( true ) {
        int    ipAdd = 1;
        string state = state_stack.back ();
        string a;
        a = str[ ip ];
        // 看看有没有这个状态
        if ( _table.find ( state ) == _table.end () ) {
            Error ();
            // cout << "Error: state " << state << " not found" << endl;
            // cout << state_stack.back () << "\t" << _formula[ r - 1 ].first << "\t" << state << endl;
            // return;
        }
        // 看看该状态下有没有这个token
        if ( _table[ state ].find ( a ) == _table[ state ].end () ) {
            Error ();
            // cout << "Error: Unexpected token " << a << " in state " << state << ", skip..." << endl;
            // ip += ipAdd;
            // continue;
            // return;
        }
        string next_state = _table[ state ][ a ];
        // 移入
        if ( startWith ( next_state, "s" ) || startWith ( next_state, "S" ) ) {
            cout << "shift" << endl;
            stack.push_back ( a );
            state_stack.push_back ( "I" + next_state.substr ( 1 ) );
            ip += ipAdd;
        }    // 归约
        else if ( startWith ( next_state, "r" ) ) {
            int counti = 0;
            r = stoi ( next_state.substr ( 1 ) );
            int index = generative_index.find ( _formula[ r ] )->second;
            cout << to_string ( index ) << endl;
            counti = _formula[ r ].second.size ();
            for ( int i = 0; i < counti; i++ ) {
                stack.pop_back ();
                state_stack.pop_back ();
            }
            stack.push_back ( _formula[ r ].first );
            state_stack.push_back ( "I" + _table[ state_stack.back () ][ _formula[ r ].first ] );
        } else if ( next_state == string ( "acc" ) ) {
            cout << "accept" << endl;
            return;
        } else {
            Error ();
            // cout << "Error: Unexpected state " << next_state << endl;
            // return;
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
    analyzer.initFormula ();
    analyzer.loadTable ();
    string str;
    cin >> str;
    // str = "n-n*n";
    str = process ( str );
    analyzer.analyse ( str );
    return 0;
}


// 基本改完了,错误处理还需再完善