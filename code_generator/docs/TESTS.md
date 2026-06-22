# Testes para o BipOS

## 1. Declaração escalar com literais

**Entrada**

```txt
let a, b: ent = 1, 2;
```

PASSOU
```asm
.data
	a: 1
	b: 2

.text
```

## 2. Declaração inicializada com variável

**Entrada**

```txt
let a: ent = 7;
let a_copy: ent = a;
```

PASSOU
```asm
.data
	a: 7
	a_copy: 0

.text
	LD a
	STO a_copy

```

## 3. Declaração inicializada com expressão

**Entrada**

```txt
let b: ent = 8;
let res: ent = b + 4 - 2;
```

PASSOU
```asm
.data
	b: 8
	res: 0

.text
	LD b
	ADDI 4
	SUBI 2
	STO res

```

## 4. Atribuições e expressão múltipla

**Entrada**

```txt
let a, b, res: ent = 10, 2, 0;

a = 9;
b = a;
res = a + b - 1 + 3;
```

PASSOU
```asm
.data
	a: 10
	b: 2
	res: 0

.text
	LDI 9
	STO a

	LD a
	STO b

	LD a
	ADD b
	SUBI 1
	ADDI 3
	STO res
```

## 5. Entrada e saída escalares

**Entrada**

```txt
let a, b: ent;

ss >> a >> b;
st << a << 7 << b;
```


```asm
.data
	a: 0
	b: 0

PASSOU
.text
	LD $in_port
	STO a

	LD $in_port
	STO b

	LD a
	STO $out_port

	LDI 7
	STO $out_port

	LD b
	STO $out_port
```

## 6. Declaração de vetores

**Entrada**

```txt
let v: [3] ent;
let v_values: [3] ent = {4, 5, 6};
```

PASSOU
```asm
.data
	v: 0,0,0
	v_values: 4,5,6

.text

```

## 7. Leitura e escrita em vetores

**Entrada**

```txt
let v: [3] ent = {4, 5, 6};

ss >> v[1];
st << v[2];
```

PASSOU (mas a declaracao do vetor quando compilado fica como 4,0,0)
```asm
.data
	v: 4,5,6

.text
	LDI 1
	STO $indr
	LD $in_port
	STOV v

	LDI 2
	STO $indr
	LDV v
	STO $out_port

```

## 8. Vetor como primeiro operando em expressão

**Entrada**

```txt
let v: [3] ent = {4, 5, 6};
let b: ent = 10;
let res: ent = v[1] + b - 2;
```

PASSOU
```asm
.data
	v: 4,5,6
	b: 10
	res: 0

.text
	LDI 1
	STO $indr
	LDV v
	ADD b
	SUBI 2
	STO res
```

## 9. Vetor nos demais operandos em expressão

**Entrada**

```txt
let v: [3] ent = {4, 5, 6};
let b, c: ent = 10, 3;
let res: ent = b + v[1] + c;
```

PASSOU
```asm
.data
	v: 4,5,6
	b: 10
	c: 3
	res: 0

.text
	LDI 1
	STO $indr
	LDV v
	STO 1000

	LD b
	ADD 1000
	ADD c
	STO res
```

## 10. Atribuição em índice de vetor

**Entrada**

```txt
let v: [3] ent = {4, 5, 6};
let index, b: ent = 1, 10;

v[index] = b + 3;
st << v[index];
```

PASSOU
```asm
.data
	valores: 4,5,6
	indice: 1
	base: 10

.text
	LD indice
	STO 1000
	LD base
	ADDI 3
	STO 1001
	LD 1000
	STO $indr
	LD 1001
	STOV valores

	LD indice
	STO $indr
	LDV valores
	STO $out_port

```

## 11. Operações com vetores de tamanhos diferentes

```
let v: [3] ent = {4, 5, 6};
let v1: [1]ent = {20};
let index, b: ent = 1, 10;

v[index] = v1[index - 1] + v[b - 5 - 5];
st << v[index];
```

PASSOU
```
.data
	v: 4,5,6
	v1: 20
	index: 1
	b: 10

.text
	LD index
	STO 1000
	LD index
	SUBI 1
	STO $indr
	LD v1
	STO 1001

	LD b
	SUBI 5
	SUBI 5
	STO $indr
	LDV v
	STO 1002

	LD 1001
	ADD 1002
	STO 1001
	LD 1000
	STO $indr
	LD 1001
	STOV v

	LD index
	STO $indr
	LDV v
	STO $out_port
```

```
let v: [3] ent = {4, 5, 6};
let v1: [1]ent = {20};
let index, b: ent = 1, 10;

v[index + 1] = v1[index - 1] + v[b - 5 - 5];
st << v[index];
```

```
.data
	v: 4,5,6
	v1: 20
	index: 1
	b: 10

.text
	LD index
	ADDI 1
	STO 1000
	LD index
	SUBI 1
	STO $indr
	LD v1
	STO 1001

	LD b
	SUBI 5
	SUBI 5
	STO $indr
	LDV v
	STO 1002

	LD 1001
	ADD 1002
	STO 1001
	LD 1000
	STO $indr
	LD 1001
	STOV v

	LD index
	STO $indr
	LDV v
	STO $out_port
```

## 12. Saída de vetor com tamanho 1 usa `LD`

```txt
let unico: [1] ent = {20};

st << unico[0];
```

```asm
.data
	unico: 20

.text
	LDI 0
	STO $indr
	LD unico
	STO $out_port
```
