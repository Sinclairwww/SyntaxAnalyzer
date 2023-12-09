#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;


class GRAMMAR_TABLE {
public:
    map< string, vector< string > > generative;
    map< string, vector< string > > first;
    map< string, vector< string > > follow;
    vector< string >                N;
    vector< string >                T;
    string                          S;
    GRAMMAR_TABLE () {
        set_generative ( "E", { "TA" } );
        set_generative ( "A", { "+TA", "-TA", "ε" } );
        set_generative ( "T", { "FB" } );
        set_generative ( "B", { "*FB", "/FB", "ε" } );
        set_generative ( "F", { "(E)", "num" } );

        N = { "E", "A", "T", "B", "F" };
        T = { "+", "-", "*", "/", "(", ")", "num", "$" };
        S = "E";

        set_first ( "E", { "(", "num" } );
        set_first ( "A", { "+", "-", "ε" } );
        set_first ( "T", { "(", "num" } );
        set_first ( "B", { "*", "/", "ε" } );
        set_first ( "F", { "(", "num" } );

        set_follow ( "E", { "$", ")" } );
        set_follow ( "A", { "$", ")" } );
        set_follow ( "T", { "+", "-", "$", ")" } );
        set_follow ( "B", { "+", "-", "$", ")" } );
        set_follow ( "F", { "*", "/", "+", "-", "$", ")" } );
    };
    void set_generative ( string left, vector< string > rights ) { generative.insert ( pair< string, vector< string > > ( left, rights ) ); };
    void set_first ( string left, vector< string > rights ) { first.insert ( pair< string, vector< string > > ( left, rights ) ); };
    void set_follow ( string left, vector< string > rights ) { follow.insert ( pair< string, vector< string > > ( left, rights ) ); };
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
    PREDICT_TABLE predict_table;
    PREDICT_TABLE SyntaxAnalyzer::create_predict_table ( GRAMMAR_TABLE G ) {
        PREDICT_TABLE predict_table;
        predict_table.table.insert ( pair< string, map< string, string > > ( "E", map< string, string > () ) );
        predict_table.table.insert ( pair< string, map< string, string > > ( "A", map< string, string > () ) );
        predict_table.table.insert ( pair< string, map< string, string > > ( "T", map< string, string > () ) );
        predict_table.table.insert ( pair< string, map< string, string > > ( "B", map< string, string > () ) );
        predict_table.table.insert ( pair< string, map< string, string > > ( "F", map< string, string > () ) );
        for ( auto iter = G.generative.begin (); iter != G.generative.end (); iter++ ) {
            string A = iter->first;
            for ( auto iter2 = iter->second.begin (); iter2 != iter->second.end (); iter2++ ) {
                string alpha = *iter2;
                if ( alpha[ 0 ] >= 'A' && alpha[ 0 ] <= 'Z' ) {
                    vector< string > first_vector = G.first.find ( string ( 1, alpha[ 0 ] ) )->second;
                    for ( auto a = first_vector.begin (); a != first_vector.end (); a++ ) {
                        if ( *a != "ε" ) {
                            predict_table.table.find ( A )->second.insert ( pair< string, string > ( *a, alpha ) );
                        }
                    }
                    if ( find ( first_vector.begin (), first_vector.end (), "ε" ) != first_vector.end () ) {
                        vector< string > follow_vector = G.follow.find ( A )->second;
                        for ( auto b = follow_vector.begin (); b != follow_vector.end (); b++ ) {
                            if ( *b != "ε" ) {
                                predict_table.table.find ( A )->second.insert ( pair< string, string > ( *b, alpha ) );
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
                        predict_table.table.find ( A )->second.insert ( pair< string, string > ( alpha_first, alpha ) );
                    }
                    if ( alpha_first == "ε" ) {
                        vector< string > follow_vector = G.follow.find ( A )->second;
                        for ( auto b = follow_vector.begin (); b != follow_vector.end (); b++ ) {
                            if ( *b != "ε" ) {
                                predict_table.table.find ( A )->second.insert ( pair< string, string > ( *b, alpha ) );
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
                if ( predict_table.table.find ( A )->second.find ( a ) == predict_table.table.find ( A )->second.end () ) {
                    predict_table.table.find ( A )->second.insert ( pair< string, string > ( a, "error" ) );
                }
            }
        }
        predict_table.N = G.N;
        predict_table.T = G.T;
        predict_table.S = G.S;
        return predict_table;
    }

    void SyntaxAnalyzer::print_predict_table () {
        cout << "PREDICT_TABLE:" << endl;
        cout << "\t";
        for ( auto iter : predict_table.table.begin ()->second ) {
            cout << iter.first << "\t\t";
        }
        cout << endl;
        for ( auto iter = predict_table.table.begin (); iter != predict_table.table.end (); iter++ ) {
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
    vector< vector< string > > SyntaxAnalyzer::predict_analyze ( string input_string ) {
        vector< vector< string > > analyze_table;
        int                        ip = 0;
        input_string += "$";
        vector< string > stack;
        stack.push_back ( "$" );
        stack.push_back ( predict_table.S );
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
            if ( find ( predict_table.T.begin (), predict_table.T.end (), X ) != predict_table.T.end () ) {
                if ( startWith ( input_string.substr ( ip ), X ) ) {
                    ip += X.size ();
                    stack.pop_back ();
                } else {
                    error ( "Except2 " + X );
                    return analyze_table;
                }
            } else if ( find ( predict_table.N.begin (), predict_table.N.end (), X ) != predict_table.N.end () ) {
                string input_string_first;
                if ( input_string[ ip ] == 'n' && input_string[ ip + 1 ] == 'u' && input_string[ ip + 2 ] == 'm' ) {
                    input_string_first = "num";
                } else {
                    input_string_first = input_string.substr ( ip, 1 );
                }
                if ( predict_table.table.find ( X ) == predict_table.table.end ()
                     || predict_table.table.find ( X )->second.find ( input_string_first ) == predict_table.table.find ( X )->second.end () ) {
                    error ( "Except3 " + X );
                    return analyze_table;
                }
                string Y = predict_table.table.find ( X )->second.find ( input_string_first )->second;
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
    void error ( string error_string ) { cout << error_string << endl; };
};


int startWith ( string str, string pattern ) { return str.find ( pattern ) == 0 ? 1 : 0; }


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

string input () {
    cout << "Please input string:" << endl;
    string input_string;
    cin >> input_string;
    return input_string;
}

int main () {
    SyntaxAnalyzer analyser;
    GRAMMAR_TABLE  G;
    analyser.create_predict_table ( G );
    analyser.print_predict_table ();

    string input_string = process ( input () );

    vector< vector< string > > analyse_table = analyser.predict_analyze ( input_string );
    analyser.print_analyze_table ( analyse_table );
    system ( "pause" );
    return 0;
}