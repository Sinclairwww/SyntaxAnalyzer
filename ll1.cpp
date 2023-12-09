#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;
typedef pair< string, vector< string > > Node;

int isStartWith ( string str, string pattern ) { return str.find ( pattern ) == 0; }

class GRAMMAR_TABLE {
public:
    map< string, vector< string > > generative;
    map< string, vector< string > > first;
    map< string, vector< string > > follow;
    vector< string >                N_set;
    vector< string >                T_set;
    string                          S;
    GRAMMAR_TABLE () {
        set_generative ( "E", { "TA" } );
        set_generative ( "A", { "+TA", "-TA", "ε" } );
        set_generative ( "T", { "FB" } );
        set_generative ( "B", { "*FB", "/FB", "ε" } );
        set_generative ( "F", { "(E)", "num" } );

        N_set = { "E", "A", "T", "B", "F" };
        T_set = { "+", "-", "*", "/", "(", ")", "num", "$" };
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
    void set_generative ( string left, vector< string > rights ) { generative.insert ( Node ( left, rights ) ); };
    void set_first ( string left, vector< string > rights ) { first.insert ( Node ( left, rights ) ); };
    void set_follow ( string left, vector< string > rights ) { follow.insert ( Node ( left, rights ) ); };
};

class PREDICT_TABLE {
public:
    map< string, map< string, string > > table;
    vector< string >                     N_set;
    vector< string >                     T_set;
    string                               S;
    PREDICT_TABLE () {
        table.insert ( pair< string, map< string, string > > ( "E", map< string, string > () ) );
        table.insert ( pair< string, map< string, string > > ( "A", map< string, string > () ) );
        table.insert ( pair< string, map< string, string > > ( "T", map< string, string > () ) );
        table.insert ( pair< string, map< string, string > > ( "B", map< string, string > () ) );
        table.insert ( pair< string, map< string, string > > ( "F", map< string, string > () ) );
    };
};

class SyntaxAnalyzer {
public:
    PREDICT_TABLE predict_table;

    void create_predict_table ( GRAMMAR_TABLE G ) {
        // 遍历生成式集合
        for ( auto generative_it = G.generative.begin (); generative_it != G.generative.end (); generative_it++ ) {
            string           left = generative_it->first;
            vector< string > right_vec = generative_it->second;
            // 遍历生成式的右部
            for ( auto right_it = right_vec.begin (); right_it != right_vec.end (); right_it++ ) {
                string right = *right_it;
                // 如果右部的第一个字符是非终结符
                if ( right[ 0 ] >= 'A' && right[ 0 ] <= 'Z' ) {
                    string           N = string ( 1, right[ 0 ] );              // 取出该非终结符
                    vector< string > first_set = G.first.find ( N )->second;    // 取出对应first集
                    // 将该非终结符的first集中的每一个终结符加入到预测分析表中
                    for ( auto it_first = first_set.begin (); it_first != first_set.end (); it_first++ ) {
                        if ( *it_first != "ε" ) {
                            predict_table.table.find ( left )->second.insert ( pair< string, string > ( *it_first, right ) );
                        }
                    }
                    // 如果该非终结符的first集中包含ε，则将该非终结符的follow集中的每一个终结符加入到预测分析表中
                    if ( find ( first_set.begin (), first_set.end (), "ε" ) != first_set.end () ) {
                        vector< string > follow_set = G.follow.find ( left )->second;
                        for ( auto follow_it = follow_set.begin (); follow_it != follow_set.end (); follow_it++ ) {
                            if ( *follow_it != "ε" ) {
                                predict_table.table.find ( left )->second.insert ( pair< string, string > ( *follow_it, right ) );
                            }
                        }
                    }
                }    // 如果右部的第一个字符是终结符
                else {
                    string alpha_first;
                    if ( isStartWith ( right, "num" ) ) {
                        alpha_first = "num";
                    } else if ( isStartWith ( right, "ε" ) ) {
                        alpha_first = "ε";
                    } else {
                        alpha_first = right[ 0 ];
                    }
                    if ( alpha_first != "ε" ) {
                        predict_table.table.find ( left )->second.insert ( pair< string, string > ( alpha_first, right ) );
                    } else {
                        // 如果该非终结符的first集中包含ε，则将该非终结符的follow集中的每一个终结符加入到预测分析表中
                        vector< string > follow_set = G.follow.find ( left )->second;
                        for ( auto follow_it = follow_set.begin (); follow_it != follow_set.end (); follow_it++ ) {
                            if ( *follow_it != "ε" ) {
                                predict_table.table.find ( left )->second.insert ( pair< string, string > ( *follow_it, right ) );
                            }
                        }
                    }
                }
            }
        }

        for ( auto N_it = G.N_set.begin (); N_it != G.N_set.end (); N_it++ ) {
            string N = *N_it;
            for ( auto T_it = G.T_set.begin (); T_it != G.T_set.end (); T_it++ ) {
                string T = *T_it;
                if ( predict_table.table.find ( N )->second.find ( T ) == predict_table.table.find ( N )->second.end () ) {
                    predict_table.table.find ( N )->second.insert ( pair< string, string > ( T, "error" ) );
                }
            }
        }
        predict_table.N_set = G.N_set;
        predict_table.T_set = G.T_set;
        predict_table.S = G.S;
    }

    void print_predict_table () {
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

    vector< vector< string > > predict_analyze ( string input_string ) {
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
            if ( find ( predict_table.T_set.begin (), predict_table.T_set.end (), X ) != predict_table.T_set.end () ) {
                if ( isStartWith ( input_string.substr ( ip ), X ) ) {
                    ip += X.size ();
                    stack.pop_back ();
                } else {
                    error ( "Except2 " + X );
                    return analyze_table;
                }
            } else if ( find ( predict_table.N_set.begin (), predict_table.N_set.end (), X ) != predict_table.N_set.end () ) {
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

    void print_analyze_table ( vector< vector< string > > analyze_table ) {
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

// 将输入字符串中的所有连续的数字（包括小数）替换为字符串 "num"
string processString ( string inputString ) {
    string newString;
    for ( int i = 0; i < inputString.size (); i++ ) {
        if ( isdigit ( inputString[ i ] ) || inputString[ i ] == '.' ) {
            newString += "num";
        } else {
            newString += inputString[ i ];
        }
    }
    return newString;
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

    string input_string = processString ( input () );

    vector< vector< string > > analyse_table = analyser.predict_analyze ( input_string );
    analyser.print_analyze_table ( analyse_table );
    system ( "pause" );
    return 0;
}