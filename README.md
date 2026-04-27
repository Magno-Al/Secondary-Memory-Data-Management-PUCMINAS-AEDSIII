# Sistema de Gestão de Matrícula em Cursos

Este projeto foi desenvolvido como parte da disciplina de **Algoritmos e Estruturas de Dados III (AEDS3) da PUC MINAS**. O objetivo é implementar um sistema de manipulação de arquivos binários diretos para armazenamento.

## Sobre o Projeto

A aplicação permite o gerenciamento completo (CRUD) de Estudantes, Módulos (Cursos) e Matrículas via console. O diferencial técnico do projeto reside na forma como os dados são acessados: em vez de realizar buscas sequenciais exaustivas no disco rígido, o sistema utiliza um índice denso baseado em Hash em disco para localizar registros instantaneamente.

## Tecnologias Utilizadas

* **Linguagem:** C++ 
* **Compilador:** GCC / G++
* **Persistência:** Arquivos Binários Puros (`.dat`)

## Arquitetura do Sistema

O projeto segue uma estrutura modular para garantir a separação de responsabilidades:

1. **Models (`src/models/`):** Define a estrutura física dos dados (POD - *Plain Old Data*). Foram projetadas para serem gravadas diretamente no disco sem a necessidade de ponteiros virtuais, evitando corrupção de memória.
2. **DAO (`src/dao/`):** Camada de acesso aos dados. Utiliza o padrão Template para operações de CRUD básicas e especializações para as buscas indexadas de cada entidade.
3. **Utils (`src/utils/`):** Contém o motor do sistema, incluindo o controle automático de chaves primárias (`FileManager`) e a implementação completa do `ExtendableHash`.
4. **Terminal Interface (`src/terminal-interface/`):** Camada de visão que gerencia os menus dinâmicos e o loop de interação com o usuário via terminal.

## Estrutura de Pastas

```text
TP/
├── data/               # Arquivos binários (.dat) e de índices do Hash
├── src/
│   ├── dao/            # Acesso a dados (StudentDAO, RegistrationDAO, etc)
│   ├── models/         # Structs (Student, Module, Registration)
│   ├── utils/          # Motor Genérico e Hash Extensível
│   ├── terminal-interface/ # Lógica visual de console
│   └── main.cpp        # Ponto de entrada do sistema
└── README.md           # Documentação
```

## Implementação do Hash Extensível

A Tabela Hash Extensível atua como o "motor de busca" primário, utilizada especificamente para relacionar rapidamente as Matrículas aos seus respectivos Módulos.

### Componentes Chave

* **Diretório (`HashDirectory.h`):** Um arquivo que mapeia as raízes binárias para endereços físicos de cestos. Dobra de tamanho dinamicamente conforme a Profundidade Global aumenta.
* **Cestos/Buckets (`HashBucket.h`):** Blocos de tamanho fixo no disco que armazenam os pares de dados `[ID do Módulo, Offset da Matrícula]`.
* **Tratamento de Colisão Robusto:** * **Split (Divisão):** Quando um cesto lota com chaves de módulos diferentes, ele é dividido matematicamente e a Profundidade Local é incrementada.
  * **Overflow (Cesto de Transbordo):** Caso o cesto atinja o limite com registros do *mesmo* módulo (chaves idênticas), o sistema cria um cesto anexo, garantindo integridade e prevenindo loops infinitos de divisão estrutural.

## Como Executar

### Pré-requisitos

* Compilador **G++** instalado e configurado nas variáveis de ambiente.

### Compilação e Execução

Abra o terminal na pasta raiz do projeto (`TP/`) e siga os passos abaixo:

1. **Compile o código-fonte:**
   Faça a compilação do projeto C++ executando um dos comandos abaixo, dependendo do compilador de sua preferência (`g++` ou `clang++`):

   **Usando GCC (g++):**

   ```bash
   g++ -std=c++11 src/main.cpp -o sistema_matriculas
   ```

   **Usando Clang (clang++):**

      ```bash
    clang++ -std=c++11 src/main.cpp -o sistema_matriculas
    ```

3. **Execute o programa:**
   * No Linux/macOS:

     ```bash
     ./sistema_matriculas
     ```

   * No Windows:

     ```cmd
     sistema_matriculas.exe
     ```
