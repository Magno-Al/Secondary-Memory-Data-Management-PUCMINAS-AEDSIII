# Sistema de Matrículas — AEDS3 | PUC Minas

Trabalho Prático — Entrega 3  
Gerenciamento de Alunos, Módulos e Matrículas com persistência em arquivos binários, Árvore B+ e Hash Extensível.

---

## Funcionalidades

- CRUD completo de Alunos, Módulos e Matrículas
- CRUD de Professores, Usuários, Emails e Telefones
- **Árvore B+** (`ORDEM=4`) para listagem de alunos ordenados por ID
- **Hash Extensível** para busca de matrículas por módulo (O(1))
- Interface de terminal interativa

---

## Estrutura do Projeto

```text
TP/
├── build/
│   ├── sistema_matriculas_linux   # binário Linux pré-compilado (estático)
│   └── sistema_matriculas.exe     # binário Windows pré-compilado (estático)
├── data/                          # gerado automaticamente em tempo de execução
├── src/
│   ├── dao/          # StudentDAO, ModuleDAO, RegistrationDAO, TeacherDAO, ...
│   ├── models/       # Student, Module, Registration, Teacher, User, Email, Phone
│   ├── indexes/
│   │   ├── btree/    # BPlusTree.h, BPlusTree.cpp
│   │   └── hash/     # ExtendableHash.h, HashDirectory.h, HashBucket.h
│   ├── utils/        # CrudTemplate.h, FileManager.h
│   ├── views/terminal-interface/
│   └── main.cpp
├── requirements/
└── README.md
```

---

## Executar com o binário pré-compilado

### Linux

```bash
./build/sistema_matriculas_linux
```

### Windows

```cmd
build\sistema_matriculas.exe
```

> A pasta `data/` é criada automaticamente no diretório onde o binário for executado.  
> Não é necessário instalar nada — os binários são estáticos, sem dependências externas.

---

## Compilar manualmente

Requer compilador C++17 (g++ 8+ ou equivalente).

```bash
g++ -std=c++17 src/main.cpp -o sistema_matriculas
./sistema_matriculas
```

Para compilação estática (Linux):

```bash
g++ -std=c++17 -static src/main.cpp -o sistema_matriculas
```

---

## Detalhes técnicos

- Todos os dados são armazenados como structs de tamanho fixo em arquivos `.dat` binários
- Deleção lógica (campo `removed`) — registros nunca são fisicamente removidos
- O header de cada arquivo armazena o último ID utilizado (4 bytes)
- A Árvore B+ indexa alunos por `_id` e suporta percurso ordenado via ponteiros de folha
- O Hash Extensível indexa matrículas por `module_id` usando endereçamento por offset de arquivo
