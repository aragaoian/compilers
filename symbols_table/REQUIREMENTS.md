# Requisitos

### (2 pontos) Inserir identificadores na tabela de símbolos com os respectivos tipo, modalidade (variável, vetor, parâmetro ou função) e escopo
1. Inserir escalar [X]
2. Inserir vetor [X]
3. Inserir função [X]
4. Inserir parâmetro [X]
5. Inserção múltipla de escalar [X]
6. Inserção múltipla de vetor [X]
7. Identificar expressões [X]
8. Atribuição [] -> não funciona ainda

### (2 pontos) Verificar se os identificadores estão declarados no escopo em que são usados no programa, sem vazar para escopos proibidos
1. Fazer função de validação de escopo [X]

### (1 pontos) Garantir a unicidade dos identificadores em um escopo
1. Fazer função de unicidade [X]

### (1 ponto) Avisar se os identificadores são declarados e não usados
1. Fazer função de declaredButNotUsed [X]

### (1 ponto) Avisar (não restringir, apenas avisar) se os identificadores estão sendo usados sem estar inicializados (uso de lixo de memória)
1. Criar função para reuniar isso 'mem garbage' [X]

### (1 ponto) Acrescentar na interface da IDE um componente para visualização da tabela
1. Criação da tabela semântica pelo qt [X]

### (2 pontos) Verificar a compatibilidade de tipos nas expressões (por exemplo, evitar multiplicação de strings) e nas atribuições.

> [!WARNING]
> A validação da compatiblidade do retorno ainda não funciona

#### Tabela de compatibilidade - Operações

> [!NOTE] 
> Para a coluna de 'Resultado', somente SUCESSO e AVISO são considerados

| Tipos (A,B) | Sucesso | Aviso | Erro | Resultado |
|-------------|---------|-------|------|-----------|
| int,int     |   +-*/  |     |      | int       |
| int,float   |   +-*/      |       |      | float          |
| int,char    |         |       | +-*/     | -           |
| int,string  |         |       | +-*/     |  -         |
| float,int   |  +-*/       |       |      |  float       |
| float,float |  +-*/       |       |      |  float         |
| float,char  |         |       | +-*/     |  -        |
| float,string|         |       | +-*/    |   -        |
| char,int    |         |       | +-*/     |   -        |
| char,float  |         |       | +-*/     |   -        |
| char,char   |         |       | +-*/     |   -        |
| char,string |   +      |       | -*/     |   string        |
| string,int  |   +      |       | -*/     |   string      |
| string,float|   +      |       | -*/     |   string       |
| string,char |   +      |       | -*/     |   string        |
| string,string|  +     |       | -*/     |   string        |


#### Tabela de compatibilidade - Atribuições

| Tipos (A,B) | Sucesso | Aviso | Erro |
|-------------|---------|-------|------|
| int,int     |    X     |       |      |
| int,float   |         |   X (perda de precisão)    |      |
| int,char    |         |       |   X   |
| int,string  |         |       |   X   |
| float,int   |    X     |       |      |
| float,float |    X     |       |      |
| float,char  |         |       |   X   |
| float,string|         |       |   X   |
| char,int    |     |     X (pega o primeiro dígito)  |      |
| char,float  |        |       |   X   |
| char,char   |    X    |       |      |
| char,string |    X (pega o primeiro)     |       |      |
| string,int  |    X (vira string)     |       |      |
| string,float|    X (vira string)    |       |      |
| string,char |    X (vira string)     |       |      |
| string,string|   X     |       |      |

> [!NOTE] 
> Aqui também combre os casos de compatiblidade de funções (e.g função de tipo VOID com retorno de int)