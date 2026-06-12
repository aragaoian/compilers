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