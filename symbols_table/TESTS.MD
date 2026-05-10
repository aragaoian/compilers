# Testes

> [!WARNING] Tirar os comentários '//'.

1. Inserção na tabela
```
let fizz: ent; // PASSOU
let a: ent = 1; // PASSOU
let b: tsimple = 'a'; // PASSOU
let c: bool = faux; // PASSOU
let d: flotter = 1.7; // PASSOU
let e: texte = "fizzbuzz"; // PASSOU
let v: [10]ent; // PASSOU
```

```
// PASSOU
let a, b, c: ent;
let d, e, f: bool = vrai, faux, vrai;
let d, e, f: bool = vrai, faux, vrai, vrai;
```

```
// PASSOU
let a, b, c: [3]ent;
let d, e: [2]ent = {1, 2}, {3, 4};

// NÃO PASSOU (arrumar)
let d, e: [2]ent = {1, 2}, {3, 4}, {1, 3};
let d, e: [2]ent = {1, 2}, {3, 4, 4, 5};
``` 

```
// PASSOU
fn test-> ent();

// PASSOU
fn test -> ent(){
    
}

// PASSOU
fn test1-> bool(arg1: ent, arg2: ent = 1);

// NÃO TESTADO
fn main-> ent(arg1: ent, arg2: ent = 1){
    retour 1;
}

// NÃO TESTADO
fn main2-> ent(arg3: [1]ent, arg4: [1]ent = {1}){
    retour vrai;
}
```

```
// PASSOU
fn fizz->bool(a: ent, b: ent);
fn buzz->bool(a: ent, b: ent);
```

2. Validação de escopo
```
// PASSOU
let a: ent = 1;

si(a == 1){
    let b: ent = 2;
    let c: ent = b;
}

let d: ent = b;
```

```
// PASSOU
fn fizz->bool(a: ent, b: ent);
fn buzz->bool(a: ent, b: ent){
    let d: ent = 67;
}

let out: bool = vrai;
si(out){
    let inside: ent = 69;
}

```

```
// PASSOU
fn fizz->bool(a: ent, b: ent);
fn buzz->bool(c: ent, d: ent){
    let d: ent = a;
    let e: ent = b;
}

let out: bool = vrai;
si(out){
    let inside: ent = 69;
}

```

```
// PASSOU
fn fizz->bool(a: ent, b: ent);
fn buzz->bool(c: ent, d: ent){
    let d: ent = 68;
    let c: tsimple  = 'a';
}

let out: bool = vrai;
si(out){
    let inside: ent = 69;
}
```


3. Unicidade de identificadores no mesmo escopo
```
// PASSOU
let a: ent;
let a: ent;
```

```
// PASSOU
let a: ent;

si(a < 5){
	let a: ent;
}
```

```
// PASSOU, mas precisa implementar a parte da expressão
// para b b não assumir um valor errôneo
let a: ent;

si(a < 5){
	let a: ent;
    let b: ent = 1;
    let b: ent;
}
```

4. Declarado e nao usado (aviso)
```
// PASSOU
let a: ent;
let b: ent = 1;

si(vrai){
    let c: ent = 2;
}
```

```
// PASSOU
let a: ent;
let b: texte = "fizz";

si(vrai){
	let buzz: texte = b;
}
```

// ADICIONAR A MESMA COISA SÓ QUE PARA FUNÇÃO

5. Uso sem inicializacao (aviso, nao erro)
```
let a: ent; // PASSOU
let b: ent = a;
```

6. Compatibilidade de tipos em atribuicoes
```
// NÃO TESTADO
let a: ent = 1;
let b: bool = vrai;
let c: texte = "abc";

// NÃO TESTADO
let d: ent = b;
let e: bool = c;
let f: tsimple = 1;
```

7. Compatibilidade de tipos em expressoes
```
// NÃO TESTADO
let a: ent = 1 + 2;
let b: flotter = 1.5 + 2.5;
let c: texte = "a" + "b";

// NÃO TESTADO
let d: ent = "a" + 1;
let e: bool = vrai + faux;
let f: flotter = "abc" * 2;
```