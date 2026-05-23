#include <string>
#include "CodeGenator.h"

void CodeGenerator::generateData(const std::vector<SymbolRow> &symbols){
    for(SymbolRow row: symbols){
        if(row.arrSize > 0){
            std::string init = "";
            for(int i = 0; i < row.arrSize; i++){
                init += "0";
                if(i != row.arrSize - 1) init += ",";
            }
            data.push_back(row.symbol + ": " + init);
        }else{
            /* NOTE
            Vou inicilizar tudo como 0
            mesmo que a variável esteja sendo inicianalizada
            E.g 'let a: ent = 1'
            .data
                a: 0
            .text
                LDI 1
                STO a*/  
            data.push_back(row.symbol + ": 0");
        }
    }
}

void CodeGenerator::store(std::string var){
    text.push_back("STO " + var);
}

void CodeGenerator::load(std::string var){
    text.push_back("LD " + var);
}

void CodeGenerator::loadi(int value){
    text.push_back("LDI " + std::to_string(value));
}

void CodeGenerator::add(std::string var){
    text.push_back("ADD " + var);
}

void CodeGenerator::addi(int value){
    text.push_back("ADDI " + std::to_string(value));
}

void CodeGenerator::sub(std::string var){
    text.push_back("SUB " + var);
}

void CodeGenerator::subi(int value){
    text.push_back("SUBI " + std::to_string(value));
}

std::string CodeGenerator::generate(){
    std::string output;

    output += ".data\n";
    for(std::string line: data){
        output += "\t" + line + "\n";
    }

    output += "\n.text\n";

    for (std::string line : text)
        output += "\t" + line + "\n";

    return output;
}