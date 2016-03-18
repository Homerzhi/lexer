 
#include <iostream>
#include <iomanip>  //setw
#include <string>
#include <cctype>
#include "lexer.h"

int main(){

  string filename = "C:\\Users\\z\\Downloads\\mine\\Fullerton\\2016_spring\\CPSC_323-02_13460_compilers\\323_project2\\code.txt ";
 
  Lexer L;
   L.analyze(filename);
  L.display();
 
 
  system("pause");
  return 0;
}
