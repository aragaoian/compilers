#include "gals_code/Lexico.h"
#include "gals_code/Sintatico.h"
#include "gals_code/Semantico.h"
#include "gals_code/LexicalError.h"
#include "gals_code/SyntacticError.h"
#include "gals_code/SemanticError.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

int main()
{
	string input = "let a : ent;";

	Lexico scanner;
	Sintatico parser;
	Semantico semantic;

	try
	{
		scanner.setInput(input.c_str());
		parser.parse(&scanner, &semantic);
		cout << "Compilação sem erros." << endl;
		return 0;
	}
	catch (const LexicalError &e)
	{
		cerr << "Erro léxico: " << e.getPosition() 
				  << e.getMessage() << endl;
	}
	catch (const SyntacticError &e)
	{
		cerr << "Erro sintático: " << e.getPosition()
				  << e.getMessage() << endl;
	}
	catch (const SemanticError &e)
	{
		cerr << "Erro semântico: " << e.getPosition()
				  << e.getMessage() << endl;
	}

	return 1;
}
