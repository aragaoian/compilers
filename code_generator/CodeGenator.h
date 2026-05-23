#include <vector>
#include <string>
#include "SymbolsTableManager.h"

const std::string TEMP1 = "1000";
const std::string TEMP2 = "1001";
const std::string TEMP3 = "1002";

enum class BipInstruction {
    // Transferência e Memória
    STO,
    LD,
    LDI,
    // Vetores
    LDV,
    STOV,
    // Aritmética
    ADD,
    ADDI,
    SUB,
    SUBI,
    // Controle
    HLT,
    CALL,
    RETURN,
    // Desvio
    BEQ,
    BNE,
    BGT,
    BGE,
    BLT,
    BLE,
    JMP,
    // Lógica
    AND,
    ANDI,
    OR,
    ORI,
    NOT,
    SLL,
    SRL
};

enum class BipSpecialRegister {
    IN_PORT,
    OUT_PORT,
    INDR
};

class CodeGenerator {
public:
    std::string generate();
private:
    CodeGenerator();
    std::vector<std::string> data;
    std::vector<std::string> text;
    std::vector<bool> tempPointers = {false, false, false};

    void generateData(const std::vector<SymbolRow> &symbols);

    void store(std::string var);
    void load(std::string var);
    void loadi(int value);

    void add(std::string var);
    void addi(int value);
    void sub(std::string var);
    void subi(int value);
};