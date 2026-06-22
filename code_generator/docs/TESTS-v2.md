# Tests para BipOS v2

## 1. Testes com operador relacional e condicional

### 1.1 Condicional if e else + constantes + variáveis
```
si(1 < 2){
	let a: ent;
}autre{
	let b: ent;
	si(45 == 46){
		let c: ent = b + 1;
	}autre{
		let d: ent = 1 + 1;
	}
}
```

```
.data
	a: 0
	b: 0
	c: 0
	d: 0

.text
	LDI 1
	STO 1000
	LDI 2
	STO 1001
	LD 1000
	SUB 1001
	BGE else_0

	JMP end_condition_0
	else_0:
	LDI 45
	STO 1000
	LDI 46
	STO 1001
	LD 1000
	SUB 1001
	BNE else_1

	LD b
	ADDI 1
	STO c

	JMP end_condition_1
	else_1:
	LDI 1
	ADDI 1
	STO d

	end_condition_1:
	end_condition_0:
```

### 1.2 Condicional simples + constantes + variáveis + vetor

```
let a: [1]ent = {1};
let b: [1]ent = {2};

si(a[0] < 2){}
si(1 < b[0]){}
si(a[0] < b[0]){}
```

```
.data
	a: 1
	b: 2

.text
	LDI 0
	STO $indr
	LD a
	STO 1000
	LDI 2
	STO 1001
	LD 1000
	SUB 1001
	BGE else_0

	else_0:
	LDI 0
	STO $indr
	LD b
	STO 1000
	LDI 1
	STO 1001
	LD 1001
	SUB 1000
	BGE else_1

	else_1:
	LDI 0
	STO $indr
	LD a
	STO 1000
	LDI 0
	STO $indr
	LD b
	STO 1001
	LD 1000
	SUB 1001
	BGE else_2

	else_2:
```

```
let i: ent = 0;
let v: [5]ent = {1, 2, 3, 4, 5};

si(v[i + 1] < v[i + 7]){}
```