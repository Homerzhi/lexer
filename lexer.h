#ifndef LEXER  //lexer.cpp
#define LEXER
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>  //setw
 
using namespace std;


//             0    , 1    ,  2,   3,           4,           5,        6,       7,         8    ,   9   ,   10                11          12      
enum INPUTCOL{ LETTER, DIGIT, DOT, UNDERSCORE, OPENRACKET, STAR, CLOSERACKET, PUNCTUATION, NEWLINE, SPACE, IDENTIFICATION, KEYWORD, SEPERATOR, OPERATOR, UNKNOWN = (-1) };

 

class Lexer{
private:
  struct Node{ string token; string lexeme; Node *next; };
  Node *front,*back;
  int  **M;
  int count;
  string tk, lx;

  
public:
  Lexer(){ 
    front = NULL;  back = NULL;  
    count = 0;
    M = new int*[16];
    for (int i = 0; i < 16; i++){
      M[i] = new int[10];
    }
    int N[16][10] = {
      /* col 0, 1,  2, 3,  4, 5, 6, 7, 8, 9 // 0=letter, 1=digit, 2=dot, 3=_ , 4=[, 5=*, 6=], 7=punct, */
      /*0*/{ 1, 2, 3, 15, 4, 5, 5, 5, 0, 0 },
      /*1*/{ 6, 15, 15, 7, 0, 0, 0, 0, 0, 0 },  //state 1=identification
      /*2*/{ 15, 8, 3, 15, 0, 0, 15, 0, 0, 0 },  //state 2=integer
      /*3*/{ 15, 9, 15, 15, 0, 0, 0, 0, 0, 0 },
      /*4*/{ 15, 15, 15, 15, 15, 10, 15, 15, 0, 0 },  //state 4=punctuation
      /*5*/{ 0, 0, 0, 0, 0, 0, 0, 11, 0, 0 },  //state 5=identification
      /*6*/{ 6, 15, 15, 7, 0, 0, 15, 0, 0, 0 },
      /*7*/{ 6, 15, 15, 7, 0, 15, 15, 15, 0, 0 },  //state 7=int
      /*8*/{ 15, 8, 3, 15, 0, 0, 15, 0, 0, 0 },  //state 8=Real
      /*9*/{ 15, 9, 15, 15, 0, 0, 15, 0, 0, 0 },   //state 9= punctuation  0, 0, 0, 0, 9 will handle more than 2 punctuations, 
      /*10*/{ 12, 12, 12, 12, 12, 13, 12, 12, 12, 12 },
      /*11*/{ 0, 0, 0, 0, 0, 0, 0, 11, 0, 0 },
      /*12*/{ 12, 12, 12, 12, 12, 13, 12, 12, 12, 12 },
      /*13*/{ 12, 12, 12, 12, 12, 13, 14, 12, 0, 12 },
      /*14*/{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      /*15*/{ 15, 15, 0, 15, 0, 15, 15, 15, 0, 0 }
      //                      0, 0, 0, 0, 0 only allows at most 2 punctuations together.
    };
    for (int i = 0; i < 16; i++){
      for (int j = 0; j < 10; j++){
        M[i][j] = N[i][j];
      }
    }

  }

  string token(){ return tk; }
  string lexeme(){ return lx; }
  int isSeperatorOperator(string str){
    if (str == ":=" || str == "==" || str == "/=" || str == ">" || str == "<"
      || str == "=>" || str == "<="
      || str == "+" || str == "-" || str == "*" || str == "/")
      return OPERATOR;
    else if (str == "$$" || str == "(" || str == ")" || str == ":"
      || str == "{" || str == "}" || str == ";" || str == ",")
      return SEPERATOR;
    return UNKNOWN;
  }

  //check if string is keyword OR ID
  int isKeywordID(string str){
    if (str == "function" || str == "integer" || str == "boolean" || str == "real"
      || str == "endif" || str == "if"
      || str == "else" || str == "return" || str == "printf" || str == "scanf"
      || str == "while"
      || str == "true" || str == "false")
      return KEYWORD;
    return IDENTIFICATION;
  }

  //return input column
  int returnCol(char c){
    if (isalpha(c)) return LETTER;
    else if (isdigit(c)) return DIGIT;
    else if (c == '.') return DOT;
    else if (c == '_') return UNDERSCORE;
    else if (c == 10) return NEWLINE;
    else if (c == '[') return OPENRACKET;
    else if (c == '*') return STAR;
    else if (c == ']') return CLOSERACKET;
    else if (c == 32) return SPACE;
    else return PUNCTUATION;
  }

  void display(){
    Node *p = front;
    while (p != NULL){
      count++;
      cout <<left<<setw(3)<<count<< left << setw(16) << p->token << "--->    " << left << setw(16 - p->token.size()) << setfill(' ') << p->lexeme << endl;
      p = p->next;
    }
    cout << "NULL\n";
  }
  
  void insert(string x, string y){
    insert(front, back, x, y);
  }
  void insert(Node *&f, Node *&b, string x, string y){
    Node *p;
    p = new(Node); p->token = x; p->lexeme = y; p->next = NULL;

    if (f == NULL){ f = p; b = p; }           //add node in front     
    else{ b->next = p; b = p; }               //add to back
  }
  string popfront(){
    return popfront(front, back);
  }
  string popfront(Node *&f, Node *&b){
    string s;
    if (f == NULL){   return "eof"; }
    else if (f->next == NULL){
      s = f->token; 
      tk = f->token; lx = f->lexeme;  
      f = NULL; b = NULL;
      return s;
    }
    else {
      Node *p; p = new(Node); p = f; 
      tk = f->token; lx = f->lexeme;
      f = f->next;
      s=p->token; delete p;
      return s;
     
    }
    
  }

  //use fsm to check int,real, ID/keyword, seperator/operator/unknown.
  void analyze(string filename) {  /*, string outputFilename){ */
 
    ifstream myfile;
    myfile.open(filename.c_str());
    if (myfile.is_open()){ cout << "input file is open\n"; }
    else cout << "input file open error\n";
    char c;

    int start = 0, state = 1, lastState = 1;
    string tempstr = "", tokenName;

   /* ofstream f;
    f.open(outputFilename.c_str(), std::ios_base::out | std::ios_base::trunc);
     
    if (f.is_open()){ cout << "output file is open\n"; }
    else cout << "output file open error\n";
    f << left << setw(16) << "token" << setw(16 - 5) << setfill(' ') << "lexeme" << endl << endl;
      */
    //get each char from file
    while (myfile.get(c)) {
      //take care of upper case if needed.
      if (isupper(c)){ c = tolower(c); }

      //handle eof  
      if (c == 13){ state = 0; }
      else  state = M[start][returnCol(c)];


      //state=0, means reach the next token 
      if (state == 0){
        start = 0;    //this is q, start state.
        if (tempstr != ""){

          tokenName = tokenType(lastState, tempstr);
          insert(tokenName, tempstr);
          //tempstr = "==" + tempstr + "==";
          //cout << left << setw(16) << tokenName << left << setw(26 - tokenName.size()) << tempstr << endl;
          //save to file
          //f << left << setw(16) << tokenName << setw(26 - tokenName.size()) << setfill(' ') << tempstr << endl;
        }
        tempstr = "";
      }

      //token continous, and save c to temp string
      else{
        tempstr += c;
        lastState = state;  //lastState keep track when state=0
        start = state;      //update start which is state OR row.
      }
    }
         
          
          
    myfile.close();
   // f.close();
  }

 
  //return token type
  string tokenType(int lastState, string tempstr){
    //to check if keyword or not
    if (lastState == 1 || lastState == 6){
      if (isKeywordID(tempstr) == KEYWORD) return "Keyword";
      else return "Identification";
    }
    //to check if seperator,operator or unknown
    else if (lastState == 5 || lastState == 11){
      //return 12=seperator, 13=operator, -1=unknown
      if (isSeperatorOperator(tempstr) == SEPERATOR) return "Seperator";
      else  if (isSeperatorOperator(tempstr) == OPERATOR) return "Operator";
      else {
        // cout << tempstr << " has syntax error\n";
        return "error"; //exit(1);
      }
    }
    else if (lastState == 2 || lastState == 8){
      return "Integer";
    }
    else if (lastState == 9) return "Real";
    else if (lastState == 14) return "Comment";
    else return "Unknown";
  }

}; //end of class

/*  
rules:
Integer = D+ 
real    = D*.D+
ID      = L|{L(L|_)*L}
punct   = P+
comment = [* (anything|*)* *]

*/



#endif
