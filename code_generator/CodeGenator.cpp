#include "CodeGenator.h"
#include "../enums/Operators.h"
#include <string>

CodeGenerator::CodeGenerator() = default;

void CodeGenerator::generateData(const std::vector<SymbolRow> &symbols) {
    for (SymbolRow row : symbols) {
        if (row.arrSize > 0) {
            if (row.isInitialized && row.initialValue != "0") {
                data.push_back(row.symbol + ": " + row.initialValue);
                continue;
            }

            std::string initialValues = "";
            for (int i = 0; i < row.arrSize; i++) {
                initialValues += "0";
                if (i != row.arrSize - 1) {
                    initialValues += ",";
                }
            }
            data.push_back(row.symbol + ": " + initialValues);
        } else {
            data.push_back(row.symbol + ": " + row.initialValue);
        }
    }
}

void CodeGenerator::store(std::string var) { text.push_back("STO " + var); }

void CodeGenerator::load(std::string var) { text.push_back("LD " + var); }

void CodeGenerator::loadi(int value) { text.push_back("LDI " + std::to_string(value)); }

void CodeGenerator::loadVector(const std::string &var) { text.push_back("LDV " + var); }

void CodeGenerator::storeVector(const std::string &var) { text.push_back("STOV " + var); }

void CodeGenerator::add(std::string var) { text.push_back("ADD " + var); }

void CodeGenerator::addi(int value) { text.push_back("ADDI " + std::to_string(value)); }

void CodeGenerator::sub(std::string var) { text.push_back("SUB " + var); }

void CodeGenerator::subi(int value) { text.push_back("SUBI " + std::to_string(value)); }

std::string CodeGenerator::getFreeTemp() {
    const std::string temps[] = {TEMP1, TEMP2, TEMP3};
    for (const std::string &temp : temps) {
        if (!tempPointers[temp]) {
            tempPointers[temp] = true;
            return temp;
        }
    }

    return "";
}

void CodeGenerator::freeTemp(const std::string &temp) {
    auto it = tempPointers.find(temp);
    if (it != tempPointers.end()) {
        it->second = false;
    }
}

void CodeGenerator::label(std::string l){
    text.push_back(l + ":");
}

void CodeGenerator::branching(Operators op, std::string label) {
    std::string instruction;
    if (Operators::GREATER == op){
        instruction = "BLE";
    }else if (Operators::GREATER_EQ == op){
        instruction = "BLT";
    }else if (Operators::LESSER == op){
        instruction = "BGE";
    }else if (Operators::LESSER_EQ == op){
        instruction = "BGT";
    }else if (Operators::EQUAL == op){
        instruction = "BNE";
    }else if (Operators::DIFFERENT == op){
        instruction = "BEQ";
    }else {
        instruction = "JMP";
    }

    text.push_back(instruction + " " + label);
}

std::string CodeGenerator::generate() {
    std::string output;

    output += ".data\n";
    for (std::string line : data) {
        output += "\t" + line + "\n";
    }

    output += "\n.text\n";

    for (std::string line : text) {
        if (line == "") {
            output += "\n";
            continue;
        }
        output += "\t" + line + "\n";
    }

    return output;
}

std::string CodeGenerator::generateWithSymbols(const std::vector<SymbolRow> &symbols) {
    data.clear();
    generateData(symbols);
    return generate();
}

void CodeGenerator::clear() {
    data.clear();
    text.clear();
    tempPointers = {{TEMP1, false}, {TEMP2, false}, {TEMP3, false}};
}

void CodeGenerator::newLine() { text.push_back(""); }
