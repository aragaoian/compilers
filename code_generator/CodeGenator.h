#include "SymbolsTableManager.h"
#include <string>
#include <unordered_map>
#include <vector>

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

enum class BipSpecialRegister { IN_PORT, OUT_PORT, INDR };

class CodeGenerator {
  private:
    std::vector<std::string> data;
    std::vector<std::string> text;
    // NOTE
    // true -> sendo usado
    // false -> não está sendo usado
    std::unordered_map<std::string, bool> tempPointers = {
        {TEMP1, false}, {TEMP2, false}, {TEMP3, false}};

    void generateData(const std::vector<SymbolRow> &symbols);

  public:
    CodeGenerator();
    std::string generate();
    std::string generateWithSymbols(const std::vector<SymbolRow> &symbols);
    void clear();
    void newLine();

    std::string getFreeTemp();
    void freeTemp(const std::string &temp);

    void store(std::string var);
    void load(std::string var);
    void loadi(int value);
    void loadVector(const std::string &var);
    void storeVector(const std::string &var);

    void add(std::string var);
    void addi(int value);
    void sub(std::string var);
    void subi(int value);
};
