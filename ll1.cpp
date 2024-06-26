#include <algorithm>    // find
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;
typedef pair< string, vector< string > > Node;

map< pair< string, string >, int > generative_index{};
void                               initIndex () {
    generative_index.insert ( { { "E", "TA" }, 1 } );
    generative_index.insert ( { { "A", "+TA" }, 2 } );
    generative_index.insert ( { { "A", "-TA" }, 3 } );
    generative_index.insert ( { { "A", "#" }, 4 } );
    generative_index.insert ( { { "T", "FB" }, 5 } );
    generative_index.insert ( { { "B", "*FB" }, 6 } );
    generative_index.insert ( { { "B", "/FB" }, 7 } );
    generative_index.insert ( { { "B", "#" }, 8 } );
    generative_index.insert ( { { "F", "(E)" }, 9 } );
    generative_index.insert ( { { "F", "n" }, 10 } );
}
/*1 E->TA 2 A->+ TA 3 A->- TA 4 A-># 5 T->FB 6 B->* FB 7 B->/ FB 8 B-># 9 F->( E ) 10 F->num */

int isStartWith ( string str, string pattern ) { return str.find ( pattern ) == 0; }

class Gramma_Info {
public:
    map< string, vector< string > > generative;
    map< string, vector< string > > first;
    map< string, vector< string > > follow;
    vector< string >                N_set;
    vector< string >                T_set;
    string                          S;
    Gramma_Info () {
        set_generative ( "E", { "TA" } );
        set_generative ( "A", { "+TA", "-TA", "#" } );
        set_generative ( "T", { "FB" } );
        set_generative ( "B", { "*FB", "/FB", "#" } );
        set_generative ( "F", { "(E)", "n" } );

        N_set = { "E", "A", "T", "B", "F" };
        T_set = { "+", "-", "*", "/", "(", ")", "n", "$" };
        S = "E";

        set_first ( "E", { "(", "n" } );
        set_first ( "A", { "+", "-", "#" } );
        set_first ( "T", { "(", "n" } );
        set_first ( "B", { "*", "/", "#" } );
        set_first ( "F", { "(", "n" } );

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
    PREDICT_TABLE              predict_table;
    vector< vector< string > > analyze_table;

    void create_predict_table ( Gramma_Info G ) {
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
                        if ( *it_first != "#" ) {
                            predict_table.table.find ( left )->second.insert ( pair< string, string > ( *it_first, right ) );
                        }
                    }
                    // 如果该非终结符的first集中包含ε，则将该非终结符的follow集中的每一个终结符加入到预测分析表中
                    if ( find ( first_set.begin (), first_set.end (), "#" ) != first_set.end () ) {
                        vector< string > follow_set = G.follow.find ( left )->second;
                        for ( auto follow_it = follow_set.begin (); follow_it != follow_set.end (); follow_it++ ) {
                            if ( *follow_it != "#" ) {
                                predict_table.table.find ( left )->second.insert ( pair< string, string > ( *follow_it, right ) );
                            }
                        }
                    }
                }    // 如果右部的第一个字符是终结符
                else {
                    string alpha_first;
                    if ( isStartWith ( right, "n" ) ) {
                        alpha_first = "n";
                    } else if ( isStartWith ( right, "#" ) ) {
                        alpha_first = "#";
                    } else {
                        alpha_first = right[ 0 ];
                    }
                    if ( alpha_first != "#" ) {
                        predict_table.table.find ( left )->second.insert ( pair< string, string > ( alpha_first, right ) );
                    } else {
                        // 如果该非终结符的first集中包含ε，则将该非终结符的follow集中的每一个终结符加入到预测分析表中
                        vector< string > follow_set = G.follow.find ( left )->second;
                        for ( auto follow_it = follow_set.begin (); follow_it != follow_set.end (); follow_it++ ) {
                            if ( *follow_it != "#" ) {
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
                // 使用error填空
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
                } else if ( iter2->second == "#" ) {
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


    string generate_stack_str ( const vector< string >& stack ) {
        string stack_str = "";
        for ( auto iter = stack.begin (); iter != stack.end (); iter++ ) {
            stack_str += *iter;
        }
        return stack_str;
    }

    bool isInN_set ( string str ) { return find ( predict_table.N_set.begin (), predict_table.N_set.end (), str ) != predict_table.N_set.end (); }

    bool isInT_set ( string str ) { return find ( predict_table.T_set.begin (), predict_table.T_set.end (), str ) != predict_table.T_set.end (); }

    void predict_analyze ( string input_string ) {
        int ip = 0;
        input_string += "$";

        vector< string > stack;
        stack.push_back ( "$" );
        stack.push_back ( predict_table.S );

        // 直到栈中只剩下$
        while ( stack.size () != 1 ) {
            string stack_str = generate_stack_str ( stack );    // 将栈中的内容转换为["A","B","C"]型的字符串
            string input_str = input_string.substr ( ip );      // 将输入串中当前位置到末尾的内容转换为字符串
            analyze_table.push_back ( { stack_str, input_str } );

            string top_item = stack.back ();
            // 如果栈顶元素是终结符
            if ( isInT_set ( top_item ) ) {
                // 如果栈顶元素与输入串中当前位置的内容相同,则将栈顶元素和输入串中当前位置的内容都弹出,并将输入串的位置后移一位
                if ( isStartWith ( input_string.substr ( ip ), top_item ) ) {
                    ip += top_item.size ();
                    stack.pop_back ();
                    analyze_table.back ().push_back ( "match" );
                } else {
                    error ( "Except2 " + top_item );
                    analyze_table.back ().push_back ( "error" );
                    return;
                }
            }    // 如果栈顶元素是非终结符
            else if ( isInN_set ( top_item ) ) {
                string input_string_first;
                // 如果输入串中当前位置的内容是num,则将input_string_first设置为num
                if ( input_string[ ip ] == 'n' ) {
                    input_string_first = "n";
                } else {    // 否则将input_string_first设置为输入串中当前位置的内容
                    input_string_first = input_string.substr ( ip, 1 );
                }
                // 如果预测分析表中没有栈顶元素和输入串中当前位置的内容对应的生成式,则报错
                if ( predict_table.table.find ( top_item ) == predict_table.table.end ()
                     || predict_table.table.find ( top_item )->second.find ( input_string_first ) == predict_table.table.find ( top_item )->second.end () ) {
                    error ( "Except3 " + top_item );
                    analyze_table.back ().push_back ( "error" );
                    return;
                }
                // 否则将栈顶元素弹出,并将对应的生成式的右部的每一个字符压入栈中
                string Y = predict_table.table.find ( top_item )->second.find ( input_string_first )->second;
                // 如果生成式的右部是error,则报错
                if ( Y == "error" ) {
                    error ( "Except4 " + top_item );
                    analyze_table.back ().push_back ( "error" );
                    return;
                }    // 否则将生成式的右部的每一个字符压入栈中
                else {
                    stack.pop_back ();

                    for ( auto iter = Y.rbegin (); iter != Y.rend (); iter++ ) {
                        string first;
                        if ( *iter == 'm' && *( iter + 1 ) == 'u' && *( iter + 2 ) == 'n' ) {
                            first = "n";
                            iter += 2;
                        } else {
                            first = string ( 1, *iter );
                        }
                        if ( first != "#" )
                            stack.push_back ( first );
                    }

                    string left = top_item;
                    string right = Y;
                    int    index = generative_index.find ( { left, right } )->second;
                    analyze_table.back ().push_back ( to_string ( index ) );
                }
            } else {
                error ( "Except5 " + top_item );
                analyze_table.back ().push_back ( "error" );
                return;
            }
        }
        if ( input_string[ ip ] == '$' ) {
            vector< string > analyze_table_item;
            analyze_table.push_back ( analyze_table_item );
            analyze_table.back ().push_back ( "$" );
            analyze_table.back ().push_back ( "$" );
            analyze_table.back ().push_back ( "accept" );
        } else {
            error ( "Except6 $" );
            analyze_table.back ().push_back ( "error" );
        }
        return;
    }

    void print_analyze_table () {
        for ( auto iter = analyze_table.begin (); iter != analyze_table.end (); iter++ ) {
            vector< string > analyze_table_item = *iter;
            for ( auto iter2 = analyze_table_item.begin (); iter2 != analyze_table_item.end (); iter2++ ) {
                cout << *iter2;
                if ( iter2 + 1 != analyze_table_item.end () ) {
                    cout << "\t";
                }
            }
            cout << endl;
        }
    }

    void error ( string error_string ){

    };
};

// 将输入字符串中的所有连续的数字（包括小数）替换为字符串 "n"
string processString ( string inputString ) {
    string newString;
    for ( int i = 0; i < inputString.size (); i++ ) {
        if ( inputString[ i ] == 'n' ) {
            newString += "n";
        } else {
            newString += inputString[ i ];
        }
    }
    return newString;
}

string input () {
    string input_string;
    cin >> input_string;
    return input_string;
}

int main () {
    SyntaxAnalyzer analyser;
    Gramma_Info    G;
    analyser.create_predict_table ( G );
    // analyser.print_predict_table ();

    string input_string = processString ( input () );
    initIndex ();
    analyser.predict_analyze ( input_string );
    analyser.print_analyze_table ();

    return 0;
}

/*
$E	n+n$	1
$AT	n+n$	5
$ABF	n+n$	10
$ABn	n+n$	match
$AB	+n$	8
$A	+n$	2
$AT+	+n$	match
$AT	n$	5
$ABF	n$	10
$ABn	n$	match
$AB	$	8
$A	$	4
$	$	accept
*/