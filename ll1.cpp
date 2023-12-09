#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;


class GRAMMAR_TABLE {
public:
    map< string, vector< string > > generative;
    vector< string >                N;
    vector< string >                T;
    string                          S;
};

class PREDICT_TABLE {
public:
    map< string, map< string, string > > table;
    vector< string >                     N;
    vector< string >                     T;
    string                               S;
};

class SyntaxAnalyzer {
public:
    PREDICT_TABLE              create_predict_table ( GRAMMAR_TABLE G, map< string, vector< string > > first_table, map< string, vector< string > > follow_table );
    void                       print_predict_table ( PREDICT_TABLE predict_table );
    vector< vector< string > > predict_analyze ( string input_string, PREDICT_TABLE M );
    void                       print_analyze_table ( vector< vector< string > > analyze_table );
    void                       error ( string error_string ) { cout << error_string << endl; };
};


int startWith ( string str, string pattern ) { return str.find ( pattern ) == 0 ? 1 : 0; }

void init ( GRAMMAR_TABLE& G, map< string, vector< string > >& first_table, map< string, vector< string > >& follow_table ) {
    vector< string > E_vector = { "TG" };
    G.generative.insert ( pair< string, vector< string > > ( "E", E_vector ) );
    vector< string > G_vector = { "+TG", "-TG", "ε" };
    G.generative.insert ( pair< string, vector< string > > ( "G", G_vector ) );
    vector< string > T_vector = { "FU" };
    G.generative.insert ( pair< string, vector< string > > ( "T", T_vector ) );
    vector< string > U_vector = { "*FU", "/FU", "ε" };
    G.generative.insert ( pair< string, vector< string > > ( "U", U_vector ) );
    vector< string > F_vector = { "(E)", "num" };
    G.generative.insert ( pair< string, vector< string > > ( "F", F_vector ) );
    G.N = { "E", "G", "T", "U", "F" };
    G.T = { "+", "-", "*", "/", "(", ")", "num", "$" };
    G.S = "E";
    vector< string > E_first_vector = { "(", "num" };
    first_table.insert ( pair< string, vector< string > > ( "E", E_first_vector ) );
    vector< string > G_first_vector = { "+", "-", "ε" };
    first_table.insert ( pair< string, vector< string > > ( "G", G_first_vector ) );
    vector< string > T_first_vector = { "(", "num" };
    first_table.insert ( pair< string, vector< string > > ( "T", T_first_vector ) );
    vector< string > U_first_vector = { "*", "/", "ε" };
    first_table.insert ( pair< string, vector< string > > ( "U", U_first_vector ) );
    vector< string > F_first_vector = { "(", "num" };
    first_table.insert ( pair< string, vector< string > > ( "F", F_first_vector ) );
    vector< string > E_follow_vector = { "$", ")" };
    follow_table.insert ( pair< string, vector< string > > ( "E", E_follow_vector ) );
    vector< string > G_follow_vector = { "$", ")" };
    follow_table.insert ( pair< string, vector< string > > ( "G", G_follow_vector ) );
    vector< string > T_follow_vector = { "+", "-", "$", ")" };
    follow_table.insert ( pair< string, vector< string > > ( "T", T_follow_vector ) );
    vector< string > U_follow_vector = { "+", "-", "$", ")" };
    follow_table.insert ( pair< string, vector< string > > ( "U", U_follow_vector ) );
    vector< string > F_follow_vector = { "*", "/", "+", "-", "$", ")" };
    follow_table.insert ( pair< string, vector< string > > ( "F", F_follow_vector ) );
}

PREDICT_TABLE SyntaxAnalyzer::create_predict_table ( GRAMMAR_TABLE G, map< string, vector< string > > first_table, map< string, vector< string > > follow_table ) {
    PREDICT_TABLE M;
    M.table.insert ( pair< string, map< string, string > > ( "E", map< string, string > () ) );
    M.table.insert ( pair< string, map< string, string > > ( "G", map< string, string > () ) );
    M.table.insert ( pair< string, map< string, string > > ( "T", map< string, string > () ) );
    M.table.insert ( pair< string, map< string, string > > ( "U", map< string, string > () ) );
    M.table.insert ( pair< string, map< string, string > > ( "F", map< string, string > () ) );
    for ( auto iter = G.generative.begin (); iter != G.generative.end (); iter++ ) {
        string A = iter->first;
        for ( auto iter2 = iter->second.begin (); iter2 != iter->second.end (); iter2++ ) {
            string alpha = *iter2;
            if ( alpha[ 0 ] >= 'A' && alpha[ 0 ] <= 'Z' ) {
                vector< string > first_vector = first_table.find ( string ( 1, alpha[ 0 ] ) )->second;
                for ( auto a = first_vector.begin (); a != first_vector.end (); a++ ) {
                    if ( *a != "ε" ) {
                        M.table.find ( A )->second.insert ( pair< string, string > ( *a, alpha ) );
                    }
                }
                if ( find ( first_vector.begin (), first_vector.end (), "ε" ) != first_vector.end () ) {
                    vector< string > follow_vector = follow_table.find ( A )->second;
                    for ( auto b = follow_vector.begin (); b != follow_vector.end (); b++ ) {
                        if ( *b != "ε" ) {
                            M.table.find ( A )->second.insert ( pair< string, string > ( *b, alpha ) );
                        }
                    }
                }
            } else {
                string alpha_first;
                if ( startWith ( alpha, "num" ) ) {
                    alpha_first = "num";
                } else if ( startWith ( alpha, "ε" ) ) {
                    alpha_first = "ε";
                } else {
                    alpha_first = alpha[ 0 ];
                }
                if ( alpha_first != "ε" ) {
                    M.table.find ( A )->second.insert ( pair< string, string > ( alpha_first, alpha ) );
                }
                if ( alpha_first == "ε" ) {
                    vector< string > follow_vector = follow_table.find ( A )->second;
                    for ( auto b = follow_vector.begin (); b != follow_vector.end (); b++ ) {
                        if ( *b != "ε" ) {
                            M.table.find ( A )->second.insert ( pair< string, string > ( *b, alpha ) );
                        }
                    }
                }
            }
        }
    }
    for ( auto iter = G.N.begin (); iter != G.N.end (); iter++ ) {
        string A = *iter;
        for ( auto iter2 = G.T.begin (); iter2 != G.T.end (); iter2++ ) {
            string a = *iter2;
            if ( M.table.find ( A )->second.find ( a ) == M.table.find ( A )->second.end () ) {
                M.table.find ( A )->second.insert ( pair< string, string > ( a, "error" ) );
            }
        }
    }
    M.N = G.N;
    M.T = G.T;
    M.S = G.S;
    return M;
}

void SyntaxAnalyzer::print_predict_table ( PREDICT_TABLE M ) {
    cout << "PREDICT_TABLE:" << endl;
    cout << "\t";
    for ( auto iter : M.table.begin ()->second ) {
        cout << iter.first << "\t\t";
    }
    cout << endl;
    for ( auto iter = M.table.begin (); iter != M.table.end (); iter++ ) {
        cout << iter->first << "\t";
        for ( auto iter2 = iter->second.begin (); iter2 != iter->second.end (); iter2++ ) {
            string output_str;
            if ( iter2->second == "error" ) {
                output_str = "error";
            } else if ( iter2->second == "ε" ) {
                output_str = iter->first + "-><epsilon>";
            } else {
                output_str = iter->first + "->" + iter2->second;
            }
            if ( output_str.length () >= 8 ) {
                cout << output_str << "\t";
            } else {
                cout << output_str << "\t\t";
            }
        }
        cout << endl;
    }
}

vector< vector< string > > SyntaxAnalyzer::predict_analyze ( string input_string, PREDICT_TABLE M ) {
    vector< vector< string > > analyze_table;
    int                        ip = 0;
    input_string += "$";
    vector< string > stack;
    stack.push_back ( "$" );
    stack.push_back ( M.S );
    while ( stack.size () != 1 ) {
        vector< string > analyze_table_item;
        analyze_table.push_back ( analyze_table_item );
        string stack_str = "[\"";
        for ( auto iter = stack.begin (); iter != stack.end (); iter++ ) {
            stack_str += *iter + "\",\"";
        }
        stack_str.erase ( stack_str.length () - 2, 2 );
        stack_str += "]";
        analyze_table.back ().push_back ( stack_str );
        string input_str = input_string.substr ( ip );
        analyze_table.back ().push_back ( input_str );
        string X = stack.back ();
        if ( find ( M.T.begin (), M.T.end (), X ) != M.T.end () ) {
            if ( startWith ( input_string.substr ( ip ), X ) ) {
                ip += X.size ();
                stack.pop_back ();
            } else {
                error ( "Except2 " + X );
                return analyze_table;
            }
        } else if ( find ( M.N.begin (), M.N.end (), X ) != M.N.end () ) {
            string input_string_first;
            if ( input_string[ ip ] == 'n' && input_string[ ip + 1 ] == 'u' && input_string[ ip + 2 ] == 'm' ) {
                input_string_first = "num";
            } else {
                input_string_first = input_string.substr ( ip, 1 );
            }
            if ( M.table.find ( X ) == M.table.end () || M.table.find ( X )->second.find ( input_string_first ) == M.table.find ( X )->second.end () ) {
                error ( "Except3 " + X );
                return analyze_table;
            }
            string Y = M.table.find ( X )->second.find ( input_string_first )->second;
            if ( Y == "error" ) {
                error ( "Except4 " + X );
                return analyze_table;
            } else {
                stack.pop_back ();
                if ( Y != "ε" ) {
                    for ( auto iter = Y.rbegin (); iter != Y.rend (); iter++ ) {
                        string first;
                        if ( *iter == 'm' && *( iter + 1 ) == 'u' && *( iter + 2 ) == 'n' ) {
                            first = "num";
                            iter += 2;
                        } else {
                            first = string ( 1, *iter );
                        }
                        stack.push_back ( first );
                    }
                    analyze_table.back ().push_back ( X + "->" + Y );
                } else {
                    analyze_table.back ().push_back ( X + "-><epsilon>" );
                }
            }
        } else {
            error ( "Except5 " + X );
            return analyze_table;
        }
    }
    if ( input_string[ ip ] == '$' ) {
        cout << "Accepted" << endl;
        vector< string > analyze_table_item;
        analyze_table.push_back ( analyze_table_item );
        analyze_table.back ().push_back ( "[\"$\"]" );
        analyze_table.back ().push_back ( "$" );
    } else {
        error ( "Except6 $" );
    }
    return analyze_table;
}

void SyntaxAnalyzer::print_analyze_table ( vector< vector< string > > analyze_table ) {
    cout << "vector< vector< string > >:" << endl;
    for ( auto iter = analyze_table.begin (); iter != analyze_table.end (); iter++ ) {
        vector< string > analyze_table_item = *iter;
        for ( auto iter2 = analyze_table_item.begin (); iter2 != analyze_table_item.end (); iter2++ ) {
            cout << *iter2;
            if ( iter2 == analyze_table_item.begin () ) {
                for ( int i = 0; i < 6 - iter2->size () / 8; i++ ) {
                    cout << "\t";
                }
            } else {
                for ( int i = 0; i < 4 - iter2->size () / 8; i++ ) {
                    cout << "\t";
                }
            }
        }
        cout << endl;
    }
}

string process ( string str ) {
    string newStr;
    int    isNum = 0;
    for ( int i = 0; i < str.size (); i++ ) {
        if ( ( str[ i ] >= '0' && str[ i ] <= '9' ) || str[ i ] == '.' ) {
            if ( !isNum ) {
                isNum = 1;
                newStr += "num";
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
    SyntaxAnalyzer                  sa;
    GRAMMAR_TABLE                   G;
    map< string, vector< string > > first_table;
    map< string, vector< string > > follow_table;
    init ( G, first_table, follow_table );
    PREDICT_TABLE M = sa.create_predict_table ( G, first_table, follow_table );
    sa.print_predict_table ( M );
    cout << "Please input string:" << endl;
    string input_string;
    cin >> input_string;
    input_string = process ( input_string );
    vector< vector< string > > at = sa.predict_analyze ( input_string, M );
    sa.print_analyze_table ( at );
    system ( "pause" );
    return 0;
}