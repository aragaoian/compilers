# Tests para BipOS v2

1. Testes com operador relacional e condicional

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