#include "../enums/BuildingStructure.h"
#include "../enums/Operators.h"
#include "SymbolsTableManager.h"
#include <string>
#include <unordered_map>
#include <vector>

const std::string TEMP1 = "1000";
const std::string TEMP2 = "1001";
const std::string TEMP3 = "1002";

class CodeGenerator {
  private:
    std::vector<std::string> data;
    std::vector<std::string> text;
    std::vector<std::string> deferredText;
    // NOTE
    // é pra ser usado no afterthought do for loop.
    // Atrasa a emissão do asm
    bool isDeferringText = false;
    // NOTE
    // true -> sendo usado
    // false -> não está sendo usado
    std::unordered_map<std::string, bool> tempPointers = {
        {TEMP1, false}, {TEMP2, false}, {TEMP3, false}};

    void generateData(const std::vector<SymbolRow> &symbols);
    void emitText(const std::string &line);

  public:
    CodeGenerator();
    std::string generate();
    std::string generateWithSymbols(const std::vector<SymbolRow> &symbols);
    void clear();
    void newLine();
    void beginDeferredText();
    void endDeferredText();
    void flushDeferredText();

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

    void label(std::string l);
    void branching(BuildingStructure bb, Operators op, std::string label);
};
