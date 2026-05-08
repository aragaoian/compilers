#include "Semantico.h"
#include "Constants.h"

#include <iostream>

void Semantico::executeAction(int action, const Token *token)
{
    std::cout << "Ação: " << action << ", Token: "  << token->getId() 
              << ", Lexema: " << token->getLexeme() << std::endl;

    switch (action)
    {
    // id
    case 1:
        stack.push(token->getLexeme().c_str());
        break;

    // tipo do dado
    case 2:
        stack.push(token->getLexeme().c_str());
        break;

    case 3:
        break;

    case 4:
        break;
    
    case 5:
        break;
    
    default:
        break;
    }
}

