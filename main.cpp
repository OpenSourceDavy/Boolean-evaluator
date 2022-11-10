#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include<string.h>

using namespace std;

typedef struct AST *pNODE;
struct AST {
    string info;
    pNODE children[2];
};

struct tokRslt {
    bool success;
    vector<string> syms;
    vector<string> vars;
    string msg;
};

struct Input {
    string msg;
    string syms;
    string vars;
};

struct parseRslt {
    bool success;
    AST ast;
};

struct TPERslt {
    bool val;
    string msg;
};


void prinTree(AST T);
pNODE cons(string s, pNODE c1, pNODE c2);
tokRslt tokenize(string s);
parseRslt parse(vector<string> V);

pNODE checkUnbreakable(vector<string> V, int start, int stop);
pNODE checkNegation(vector<string> V, int start, int stop);
pNODE checkConjunction(vector<string> V, int start, int stop);
pNODE checkDisjunction(vector<string> V, int start, int stop);

pNODE checkBooleanExpression(vector<string> V, int start, int stop);
;
bool eval(AST T);
TPERslt TPE(string s);


void prinTree(AST T) {
    if (T.children[0] == NULL) {
        cout << T.info;
        return;
    }

    cout << "(" << T.info << " ";
    prinTree(*(T.children[0]));
    cout << " ";

    if (T.children[1] != NULL) {
        prinTree(*(T.children[1]));
    }

    cout << ")";
}

pNODE cons(string s, pNODE c1, pNODE c2) {
    pNODE ret = new AST;
    ret->info = s;
    ret->children[0] = c1;
    ret->children[1] = c2;
    return ret;
}
//void Tokenize1(const string& str, vector<string>& tokens, const string& delimiters)
//{
//
//    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
//
//    string::size_type pos     = str.find_first_of(delimiters, lastPos);
//    while (string::npos != pos || string::npos != lastPos)
//    {
//
//        tokens.push_back(str.substr(lastPos, pos - lastPos));
//
//        lastPos = str.find_first_not_of(delimiters, pos);
//
//        pos = str.find_first_of(delimiters, lastPos);
//    }
//}

auto split(const string& str, const string& delim)
{
    vector<string> vs;
    size_t pos {};

    for (size_t fd = 0; (fd = str.find(delim, pos)) != std::string::npos; pos = fd + delim.size())
        vs.emplace_back(str.data() + pos, str.data() + fd);

    vs.emplace_back(str.data() + pos, str.data() + str.size());
    return vs;
}

Input seperate(string s){

    auto vs {split(s, ";")};
    string::size_type idx;
    idx = s.find(";");


    Input input;
    input.syms = vs[0];
    input.msg ="success";
    if (idx==string::npos){
        input.msg = "false";
    }
    if (vs.size()>1 ) {
        input.vars = vs[1];
    }
    if (vs.size()>2 ) {
        input.msg = "false";
    }
    return input;
}

tokRslt tokenizevar(string s)
{
    tokRslt tokenize;
    tokenize.success = true;


    if (tokenize.success)
    {
        tokenize.success = true;


        for (size_t i = 0; i < s.length(); i++)
        {

            if (s[i] == ' ')
                continue;
            if (s[i]=='1'||s[i]=='0'){
                int j = i+1;
                while(s[j]&&s[j]==' '){
                    j+=1;
                }
                if (s[j]&&s[j]!=','){
                    tokenize.success = false;
                    break;
                }
            }
            if (s[i] == '!'|| s[i] == '/'|| s[i] == '%'|| s[i] == '^'|| s[i] == '&'|| s[i] == '#'|| s[i] == '@'|| s[i] == '|'|| s[i] == '{'|| s[i] == '}'|| s[i] == '['|| s[i] == ']'|| s[i] == '?'|| s[i] == '='|| s[i] == '~'|| s[i] == '`'|| s[i] == '.'|| s[i+1] == '_'|| s[i+1] == '"'|| s[i+1] == '<'|| s[i+1] == '>') {
                tokenize.success = false;
                break;
            }

            if (s[i] != '1' && s[i] != ',' && s[i] != ':' && s[i] != ' '&& s[i] != '0' )
            {
                int lenth = 1;
                int position = i;

                while(s[i+1] && s[i+1] != ':' && s[i+1] != ' ') {
                    lenth += 1;
                    i+=1;
                }
                tokenize.syms.push_back(s.substr(position, lenth));
                tokenize.vars.push_back(s.substr(position, lenth));
                int j =i+1;
                while(s[j]==' '){
                    j +=1;
                }
                if (s[j]!=':'){
                    tokenize.success = false;
                    break;
                }
            }


            else
            {
                tokenize.syms.push_back(s.substr(i, 1));
            }
        }
    }


    if (tokenize.success == false) {

        tokenize.syms.clear();
    }

    return tokenize;
}

tokRslt checkvars(tokRslt t,string s){
    tokRslt re;
    if(!t.success){
        re.success = false;
        re.msg = "Error: invalid input";
        return re;
    }

    if(!t.vars.empty()){

        for (auto & var : t.vars){
            auto vs {split(s, var)};
            if (vs.size()>2) {
                string value;
                string value2;
                for (char q : vs[1]) {

                    if (q == ' ' || q == ':') {
                        continue;
                    } else if (q == '1' || q == '0') {


                        value = q;
                        break;
                    }
                }
                for (char q : vs[2]) {

                    if (q == ' ' || q == ':') {
                        continue;
                    } else if (q == '1' || q == '0') {

                        value2 = q;
                        break;
                    }
                }
                if (value != value2) {
                    re.success = false;
                    re.msg = "Error: contradicting assignment";

                    return re;
                }
            }
        }
        re.success = true;
        re.msg = "success";
        return re;
    };
    re.success = true;
    return re;
}


tokRslt tokenize(string s)
{
    tokRslt tokenize;
    tokenize.success = true;


    if (tokenize.success)
    {
        tokenize.success = true;


        for (size_t i = 0; i < s.length(); i++)
        {

            if (s[i] == ' ')
                continue;
//            if (s[i] =='(' ||s[i] ==')'||s[i] =='1'||s[i] =='0'){
//                if (s[i+1] !='+' &&s[i+1] !='-'&&s[i+1] !='*'){
//                    tokenize.success = false;
//                }
//            }
            if (s[i] == '!'|| s[i] == '/'|| s[i] == '%'|| s[i] == '^'|| s[i] == '&'|| s[i] == '#'|| s[i] == '@'|| s[i] == '|'|| s[i] == '{'|| s[i] == '}'|| s[i] == '['|| s[i] == ']'|| s[i] == '?'|| s[i] == '='|| s[i] == '~'|| s[i] == '`'|| s[i] == '.'|| s[i+1] == '_'|| s[i+1] == '"'|| s[i+1] == ','|| s[i+1] == '<'|| s[i+1] == '>') {
                tokenize.success = false;
                break;
            }

            if (s[i] != '1' && s[i] != '0' && s[i] != '*' && s[i] != '+' && s[i] != '-'&&
                s[i] != '(' && s[i] != ')' && s[i] != ' ')
            {
                int lenth = 1;
                int position = i;

                while(s[i+1] && s[i+1] != '*' && s[i+1] != '+' && s[i+1] != '-'&&
                      s[i+1] != '(' && s[i+1] != ')' && s[i+1] != ' ') {
                    if (s[i+1] == '!'|| s[i+1] == '/'|| s[i+1] == '%'|| s[i+1] == '^'|| s[i+1] == '&'|| s[i+1] == '#'|| s[i+1] == '@'|| s[i+1] == '|'|| s[i+1] == '{'|| s[i+1] == '}'|| s[i+1] == '['|| s[i+1] == ']'|| s[i+1] == '?'|| s[i+1] == '='|| s[i+1] == '~'|| s[i+1] == '`'|| s[i+1] == '.'|| s[i+1] == '_'|| s[i+1] == '"'|| s[i+1] == ','|| s[i+1] == '<'|| s[i+1] == '>') {

                        tokenize.success = false;
                        break;
                    }
                    lenth += 1;
                    i+=1;
                }
                tokenize.syms.push_back(s.substr(position, lenth));
                tokenize.vars.push_back(s.substr(position, lenth));

            }


            else
            {
                tokenize.syms.push_back(s.substr(i, 1));
            }
        }
    }


    if (tokenize.success == false) {

        tokenize.syms.clear();
    }

    return tokenize;
}

tokRslt getvar(tokRslt t, string s, string msg, tokRslt varre){

    if (msg == "false"){
        t.success = false;

        t.msg = "Error: invalid input";
        return t;
    }
    if (t.success == false){

        t.msg = "Error: invalid input";
        return t;
    }
    if (varre.success== false){
        t.success = false;
        t.msg = varre.msg;
        return t;
    }
    if(!t.vars.empty()) {

        for (int i = 0; i <t.vars.size(); i++) {

            string::size_type idx;
            idx = s.find(t.vars[i]);

            if (idx == string::npos) {

                t.success = false;
                t.msg = "Error: incomplete assignment";
                return t;

            }

            else {

                 auto vs {split(s, t.vars[i])};

            if(vs.size()==1){
                vs.push_back(" ");
                swap(vs[1],vs[0]);
            }


                if (vs.size()>2) {
                    string value;
                    string value2;
                    for (char q : vs[1]) {

                        if (q == ' ' || q == ':') {
                            continue;
                        } else if (q == '1' || q == '0') {


                            value = q;
                            break;
                        }
                    }
                    for (char q : vs[2]) {

                        if (q == ' ' || q == ':') {
                            continue;
                        } else if (q == '1' || q == '0') {

                            value2 = q;
                            break;
                        }
                    }
                    if (value != value2) {
                        t.success = false;
                        t.msg = "Error: contradicting assignment";

                        return t;
                    }
                }
                idx = vs[1].find(":");
                if (idx == string::npos) {
                    t.success = false;
                    t.msg = "Error: invalid input";
                    return t;
                }
                for (size_t q = 0; q < vs[1].size(); q++) {

                    if (vs[1][q] == ' ' || vs[1][q] == ':') {
                        continue;
                    } else if (vs[1][q] == '1' || vs[1][q] == '0') {

                        t.vars[i] = vs[1][q];
                        break;
                    }
                }

            }
        }

        int j = 0;


        for (size_t i = 0; i < t.syms.size() ; i++) {
            if (t.syms[i] != "1" && t.syms[i] != "0"&& t.syms[i] != "+"&& t.syms[i] != "*"&& t.syms[i] != "-"&& t.syms[i] != "("&& t.syms[i] != ")") {

                swap(t.syms[i],t.vars[j]);

                j++;
            }
        }
    }
    return t;
}

parseRslt parse(vector<string> V)
{
    parseRslt expression;
    pNODE expression2;
    int open = 0;
    int close = 0;


    if (V.empty())
    {

        expression.success = false;
        expression.ast = *cons("EMPTY", NULL, NULL);
        return expression;
    }


    for (size_t i = 0; i < V.size() - 1; i++)
    {
        if (V[i] == "1" || V[i] == "0")
        {
            if (V[i + 1] == "*" || V[i + 1] == "+" || V[i + 1] == ")")
                continue;
            else
            {
                expression.success = false;
                expression.ast = *cons("EMPTY", NULL, NULL);
                return expression;
            }
        }
    }

    if (V[0] == "*" || V[0] == "+" )
    {
        expression.success = false;
        expression.ast = *cons("EMPTY", NULL, NULL);
        return expression;
    }


    for (auto & i : V)
    {
        if (i == "(")
            open++;
        if (i == ")")
            close++;
    }

    if (open != close)
    {
        expression.success = false;
        expression.ast = *cons("EMPTY", NULL, NULL);
        return expression;
    }


    expression2 = checkBooleanExpression(V, 0, V.size());

    if (expression2 == NULL)
        expression.success = false;
    else
    {
        expression.success = true;
        expression.ast = *expression2;
    }

    if (expression.success)
    {
//        prinTree(expression.ast);
        return expression;
    }
    else
    {
        return expression;
    }
}

pNODE checkConstant(vector<string> V, int start, int stop)
{
    pNODE expression;

    if (start != stop - 1 || start >= V.size())
    {
        return NULL;
    }

    if (V[start] == "1" || V[start] == "0")
    {
        return cons(V[start], NULL, NULL);
    }
    else
    {
        return cons("EMPTY", NULL, NULL);
    }
}

pNODE checkUnbreakable(vector<string> V, int start, int stop)
{
    pNODE expression;

    if (V[start] == "(" && V[stop - 1] == ")")
    {
        expression = checkBooleanExpression(V, start + 1, stop - 1);
        if (expression != NULL)
            return expression;
    }

    expression = checkConstant(V, start, stop);
    if (expression != NULL)
        return expression;
    else
        return NULL;
}

pNODE checkNegation(vector<string> V, int start, int stop)
{
    pNODE expression;

    for (int i = start; i < stop; i++)
    {
        if (V[i] == "-")
        {
            expression = checkNegation(V, start + 1, stop);
            if (expression != NULL)
                return cons(V[i], expression, NULL);
        }
    }

    expression = checkUnbreakable(V, start, stop);
    if (expression != NULL)
        return expression;
    else
        return NULL;
}

pNODE checkConjunction(vector<string> V, int start, int stop)
{
    pNODE expression1, expression2;

    for (int i = start + 1; i < stop; i++)
    {
        if (V[i] == "*")
        {
            expression1 = checkConjunction(V, start, i);
            expression2 = checkNegation(V, i + 1, stop);
            if (expression1 != NULL && expression2 != NULL)
                return cons(V[i], expression1, expression2);
        }
    }

    expression1 = checkNegation(V, start, stop);

    if (expression1 != NULL)
        return expression1;
    else
        return NULL;
}

pNODE checkDisjunction(vector<string> V, int start, int stop)
{
    pNODE expression1, expression2;

    for (int i = start + 1; i < stop; i++)
    {
        if (V[i] == "+")
        {
            expression1 = checkDisjunction(V, start, i);
            expression2 = checkConjunction(V, i + 1, stop);
            if (expression1 != NULL && expression2 != NULL)
                return cons(V[i], expression1, expression2);
        }
    }

    expression1 = checkConjunction(V, start, stop);

    if (expression1 != NULL)
        return expression1;
    else
        return NULL;
}


pNODE checkBooleanExpression(vector<string> V, int start, int stop)
{
    pNODE expression1;


    expression1 = checkDisjunction(V, start, stop);

    if (expression1 != NULL)
        return expression1;
    else
        return NULL;
}

bool eval(AST T)
{
    bool left, right;


    if (T.children[0] == NULL)
    {
        if (T.info == "1")
            return true;
        else if (T.info == "0")
            return false;
        else
            return NULL;
    }


    else if (T.info == "-" && T.children[1] == NULL)
    {
        return !eval(*T.children[0]);
    }


    else
    {

        if (T.info == "*")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);

            if (left == true && right == true)
                return true;
            else
                return false;
        }

            //OR statements
        else if (T.info == "+")
        {
            left = eval(*T.children[0]);
            right = eval(*T.children[1]);

            if (left == false && right == false)
                return false;
            else
                return true;
        }


        return NULL;
    }
}


TPERslt TPE(string s)
{
    TPERslt result;
    Input input = seperate(s);

    tokRslt token1 = tokenize(input.syms);
    tokRslt tokvar = tokenizevar(input.vars);
    tokRslt varre = checkvars(tokvar, input.vars);
    tokRslt token = getvar(token1, input.vars, input.msg, varre);

    parseRslt expression = parse(token.syms);


    if (token.success)
    {

        if (expression.success)
        {
            result.msg = "success";
            result.val = eval(expression.ast);
        }


        else if (!expression.success)
        {

            result.msg = "Error: invalid input";
        }
    }

    else
    {
        result.msg = token.msg;
    }
    return result;
}

string evalResult(string s)
{
    TPERslt output;

    output = TPE(s);


    if (output.msg == "success")
    {

        if (output.val == true)
            return "1";
        else if (output.val == false)
            return "0";
        else
            return NULL;
    }


    else
        return output.msg;
}



int main()
{
    string user_input;

//    cout << "Enter a boolean expression:\n";
    while (true){
        if(getline(cin, user_input)) {
            if (user_input ==""){
                cout<<"Error: invalid input"<<endl;
                continue;
            }
            else if (user_input ==";"){
                cout<<"Error: invalid input"<<endl;
                continue;
            }
            cout << evalResult(user_input)<<endl;
            continue;
        }
        break;
    }

    return 0;
}