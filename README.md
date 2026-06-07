# Sistema de Matrículas — AEDS3 | PUC Minas

Trabalho Prático — Entrega 4  
Gerenciamento de Alunos, Módulos e Matrículas com persistência em arquivos binários, Árvore B+, Hash Extensível e compressão de backup (Huffman e LZW).

---

## Funcionalidades

- CRUD completo de Alunos, Módulos e Matrículas
- CRUD de Professores, Usuários, Emails e Telefones
- **Árvore B+** (`ORDEM=4`) para listagem de alunos ordenados por ID
- **Hash Extensível** para busca de matrículas por módulo (O(1))
- **Compressão / Backup** de todos os arquivos de dados em um único arquivo, com **Huffman** e **LZW**, incluindo restauração
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

## Detalhes técnicos

- Todos os dados são armazenados como structs de tamanho fixo em arquivos `.dat` binários
- Deleção lógica (campo `removed`) — registros nunca são fisicamente removidos
- O header de cada arquivo armazena o último ID utilizado (4 bytes)
- A Árvore B+ indexa alunos por `_id` e suporta percurso ordenado via ponteiros de folha
- O Hash Extensível indexa matrículas por `module_id` usando endereçamento por offset de arquivo
- A compressão (Huffman e LZW) opera sobre bytes crus, sem dependências externas, em headers
  `header-only` (`src/utils/compression/`)
