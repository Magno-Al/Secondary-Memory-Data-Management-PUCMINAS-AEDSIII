# Sistema de Matrículas — AEDS3 | PUC Minas

Trabalho Prático — Entrega 5  
Gerenciamento de Alunos, Módulos e Matrículas com persistência em arquivos binários, Árvore B+, Hash Extensível, compressão de backup (Huffman e LZW), casamento de padrões (KMP e Boyer–Moore) e criptografia XOR.

---

## Funcionalidades

- CRUD completo de Alunos, Módulos e Matrículas
- CRUD de Professores, Usuários, Emails e Telefones
- **Árvore B+** (`ORDEM=4`) para listagem de alunos ordenados por ID
- **Hash Extensível** para busca de matrículas por módulo (O(1))
- **Compressão / Backup** de todos os arquivos de dados em um único arquivo, com **Huffman** e **LZW**, incluindo restauração
- **Casamento de padrões** (**KMP** e **Boyer–Moore**) na busca por nome de Aluno e de Curso
- **Criptografia XOR** da senha do usuário (campo sensível)
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
│   │   └── compression/  # Huffman.h, LZW.h, BackupManager.h
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

## Backup e Compressão (Entrega 4)

No menu principal, a opção **`4. Backup / Compressao`** dá acesso às operações de compressão:

```text
--- BACKUP / COMPRESSAO ---
1. Compactar dados (Huffman)
2. Compactar dados (LZW)
3. Restaurar backup (Huffman)
4. Restaurar backup (LZW)
5. Comparar taxas (Huffman vs LZW)
0. Voltar ao Menu Principal
```

- A compactação é **a nível de arquivo**: todos os arquivos do diretório `data/` são
  empacotados em um **único arquivo de backup** (formato tipo TAR) e então comprimidos.
- Dois algoritmos são suportados, gerando cada um seu arquivo único:
  - **Huffman** → `data/backup_huffman.huff`
  - **LZW** → `data/backup_lzw.lzw`
- A restauração descomprime o backup e reconstrói **todos** os arquivos de `data/`
  (dados + índices Árvore B+ e Hash) de forma consistente entre si.
- Cada compactação imprime **tamanho original, tamanho comprimido e taxa de compressão**,
  valores usados no formulário técnico do relatório.

> Observação: para volumes muito pequenos de dados, o cabeçalho do Huffman (tabela de
> frequências) pode tornar o arquivo comprimido maior que o original — comportamento
> esperado; os ganhos aparecem conforme o volume de dados cresce.

---

## Pesquisa por padrão e Criptografia (Entrega 5)

### Casamento de padrões (KMP e Boyer–Moore)

No menu principal, a opção **`5. Pesquisar por padrao (KMP / BM)`**:

```text
--- PESQUISA POR PADRAO (KMP / Boyer-Moore) ---
Pesquisar em:  1. Nome do Aluno   2. Nome do Curso (Modulo)
Algoritmo:     1. KMP             2. Boyer-Moore (bad character)
Padrao a pesquisar: <string>
```

- O usuário escolhe **o campo** (nome do Aluno ou do Curso), **o algoritmo** (KMP ou
  Boyer–Moore) e informa o **padrão**; o sistema lista os registros cujo campo contém o padrão.
- A busca é **case-insensitive**. Os algoritmos são header-only em `src/utils/pattern/`
  (`KMP.h` — tabela LPS; `BoyerMoore.h` — regra *bad character*).
- Na **GUI**, a aba **Pesquisa** oferece a mesma funcionalidade, e o filtro da aba **Alunos**
  também usa KMP/Boyer–Moore (com seletor de algoritmo).

### Criptografia XOR

- A **senha do usuário** (`User.password`) é o campo sensível: é gravada **cifrada com XOR**
  (chave fixa repetida) em `data/users.dat`, e nunca exibida em texto puro.
- A cifragem é centralizada no `UserDAO` (`src/utils/crypto/XorCipher.h`): `create` cifra antes
  de gravar e `login` cifra a senha informada e compara com a armazenada.

> **Importante:** se você já possui um `data/users.dat` criado **antes** desta etapa (senha em
> texto puro), apague-o (ou a pasta `data/`) — em uma execução nova o usuário gerente padrão
> (`gerente` / `gerente`) é recriado já com a senha cifrada.

---

## Detalhes técnicos

- Todos os dados são armazenados como structs de tamanho fixo em arquivos `.dat` binários
- Deleção lógica (campo `removed`) — registros nunca são fisicamente removidos
- O header de cada arquivo armazena o último ID utilizado (4 bytes)
- A Árvore B+ indexa alunos por `_id` e suporta percurso ordenado via ponteiros de folha
- O Hash Extensível indexa matrículas por `module_id` usando endereçamento por offset de arquivo
- A compressão (Huffman e LZW) opera sobre bytes crus, sem dependências externas, em headers
  `header-only` (`src/utils/compression/`)
