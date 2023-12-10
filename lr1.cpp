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
    void loadFormula ( string file_name );
    void analyse ( string str );
    int  startWith ( string str, string start ) { return str.find ( start ) == 0 ? 1 : 0; }

private:
    analyse_table                    _table;
    vector< pair< string, string > > _formula;
    void                             _printStack ( vector< string > stack, vector< string > state_stack, string str, string action ) { cout << action << endl; };
};

void SyntaxAnalyzer::loadTable ( string file_name ) {
    ifstream file ( file_name );
    if ( !file.is_open () ) {
        cout << "Error: file " << file_name << " not found" << endl;
        return;
    }
    analyse_table    table;
    string           line;
    vector< string > tokens;
    getline ( file, line );
    string word;
    for ( int i = 0; i < line.size (); i++ ) {
        if ( line[ i ] == ',' || line[ i ] == '\n' ) {
            string str2 = "state";
            if ( word.size () > 0 && word.compare ( str2 ) != 0 ) {
                tokens.push_back ( word );
                word.clear ();
            } else {
                word.clear ();
            }
        } else {
            word += line[ i ];
        }
    }
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

void SyntaxAnalyzer::loadFormula ( string file_name ) {
    ifstream file ( file_name );
    if ( !file.is_open () ) {
        cout << "Error: file " << file_name << " not found" << endl;
        return;
    }
    vector< pair< string, string > > formula;
    string                           line;
    while ( getline ( file, line ) ) {
        string word[ 2 ];
        int    count = 0;
        for ( int i = 0; i < line.size (); i++ ) {
            if ( line[ i ] == '-' && i + 1 < line.size () && line[ i + 1 ] == '>' ) {
                count++;
                i++;
            } else {
                word[ count ] += line[ i ];
            }
        }
        formula.push_back ( make_pair ( word[ 0 ], word[ 1 ] ) );
    }
    _formula = formula;
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
    analyzer.loadFormula ( "lr1.conf" );
    string str;
    cin >> str;
    str = process ( str );
    analyzer.analyse ( str );
    return 0;
}


// 改读入文件