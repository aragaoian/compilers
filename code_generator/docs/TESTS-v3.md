# Tests para BipOS v3

## 1. Chamada de sub-rotinas

### 1.1 Teste com o main()

```
fn main->ent(){
    st << 67;
    retour 0;
}
```

```
.data
	return_main_0: 0

.text
	JMP function_main_0
	function_main_0:
	LDI 67
	STO $out_port

	LDI 0
	STO return_main_0
	HLT
```

### 1.2 Teste chamada de função com argumentos

```
fn teste->ent(a: ent){
	retour a + 1;
}

fn main->ent(){
    let a: ent = teste(2);
}
```

```
.data
	return_teste_1: 0
	return_main_0: 0
	param_a_1_teste: 0
	a: 0

.text
	JMP function_main_0
	function_teste_1:
	LD param_a_1_teste
	ADDI 1
	STO return_teste_1
	RETURN

	function_main_0:
	LDI 2
	STO param_a_1_teste
	CALL function_teste_1

	LD return_teste_1
	STO a
```

## 2. Retornos

### 2.1 Com expressão

```
fn teste->ent(a: ent){
	retour a + 1;
}

fn main->ent(){
    si(teste(1) == 2){
		st << 69;
	}
}
```

```
.data
	return_teste_1: 0
	return_main_0: 0
	param_a_1_teste: 0

.text
	JMP function_main_0
	function_teste_1:
	LD param_a_1_teste
	ADDI 1
	STO return_teste_1
	RETURN

	function_main_0:
	LDI 1
	STO param_a_1_teste
	CALL function_teste_1

	LDI 2
	STO 1000
	LD return_teste_1
	SUB 1000
	BNE else_0

	LDI 69
	STO $out_port

	else_0:
	HLT
```


### 2.2 Com atribuição/declaração

```
fn teste->ent(a: ent){
	retour a + 1;
}

fn main->ent(){
    let a: ent = teste(a);
    st << a;
}
```

```
.data
	return_teste_1: 0
	return_main_0: 0
	param_a_1_teste: 0
	var_a_main: 0

.text
	JMP function_main_0
	function_teste_1:
	LD param_a_1_teste
	ADDI 1
	STO return_teste_1
	RETURN

	function_main_0:
	LD var_a_main
	STO param_a_1_teste
	CALL function_teste_1

	LD return_teste_1
	STO var_a_main

	LD var_a_main
	STO $out_port

	HLT
```
