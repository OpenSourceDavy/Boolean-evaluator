#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <regex>

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
    unordered_map<string,string> varmap;
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

    unordered_map<string,string> varmap;
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
                int z = j+1;
                while(s[z]==' '){
                    z +=1;
                }
                auto iterator = tokenize.varmap.find(s.substr(position, lenth));
                if(iterator != tokenize.varmap.end()) {
                    if ("0"==iterator->second){

                        if (s[z]=='1'){
                            tokenize.success = false;
                            tokenize.msg ="contra";
                            break;
                        }
                    }
                    if ("1"==iterator->second){
                        if (s[z]=='0'){
                            tokenize.success = false;
                            tokenize.msg ="contra";
                            break;
                        }
                    }
                }
                tokenize.varmap[s.substr(position, lenth)] = s[z];
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
        if (t.msg== "contra"){
            re.success = false;
            re.msg ="Error: contradicting assignment";
            return re;
        }
        re.success = false;
        re.msg = "Error: invalid input";
        return re;
    }
    re.success = true;
    re.varmap = t.varmap;

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

            if (s[i] == '!'|| s[i] == '/'|| s[i] == '%'|| s[i] == '^'|| s[i] == '&'|| s[i] == '#'|| s[i] == '@'|| s[i] == '|'|| s[i] == '{'|| s[i] == '}'|| s[i] == '['|| s[i] == ']'|| s[i] == '?'|| s[i] == '='|| s[i] == '~'|| s[i] == '`'|| s[i] == '.'|| s[i+1] == '_'|| s[i+1] == '"'|| s[i+1] == ','|| s[i+1] == '<'|| s[i+1] == '>') {
                tokenize.success = false;
                break;
            }
            if (s[i]=='-'){
                int j = i+1;
                while(s[j]==' '){
                    j += 1;
                }
                if (!s[j]){
                    tokenize.success = false;
                    break;
                }
            }
            if (s[i] != '1' && s[i] != '0' && s[i] != '*' && s[i] != '+' && s[i] != '-'&&
                s[i] != '(' && s[i] != ')' && s[i] != ' ')
            {
                regex varreg("[A-Za-z]");
                string c ;
                c.append(1,s[i]);
                bool match = regex_match(c,varreg);
                if (!match){
                    tokenize.success = false;

                    break;
                }
                int lenth = 1;
                int position = i;

                while(s[i+1] && s[i+1] != '*' && s[i+1] != '+' && s[i+1] != '-'&&
                      s[i+1] != '(' && s[i+1] != ')' && s[i+1] != ' ') {
                    string c1 ;
                    c1.append(1,s[i+1]);
                    bool nummatch = regex_match(c1,varreg);
                    if (!nummatch){
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
                auto iterator = varre.varmap.find(t.vars[i]);
                if(iterator != varre.varmap.end()) {
                    t.vars[i] = iterator->second;
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
//  prinTree(expression.ast);
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
    pNODE expression1;


        if (V[start] == "-")
        {

            expression1 = checkNegation(V, start + 1, stop);
            if (expression1 != NULL)
                return cons(V[start], expression1, NULL);
        }


    expression1 = checkUnbreakable(V, start, stop);
    if (expression1 != NULL)
        return expression1;
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