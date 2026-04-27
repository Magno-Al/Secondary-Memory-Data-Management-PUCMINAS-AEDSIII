# Sistema de Matrícula Acadêmica - AED III

Este projeto consiste em um sistema de gerenciamento de dados em memória secundária, focado na manipulação de arquivos binários, indexação e persistência de dados.

## 📌 Descrição do Projeto
O sistema permite o gerenciamento de usuários Estudantes e Professores, Cursos e Módulos, com um fluxo completo de solicitação de matrícula.

## 🛠️ Modelagem de Dados (DER)

Abaixo estão as relações fundamentais que regem a integridade do sistema:

| Origem (Pai) | Destino (Filho) | Cardinalidade | Descrição |
| :--- | :--- | :--- | :--- |
| **User** | **Email** | 1 : N | Um usuário pode ter vários e-mails cadastrados. |
| **User** | **Phone** | 1 : N | Um usuário pode ter vários números de telefone. |
| **User** | **Student** | 1 : 1 | Especialização: Um usuário possui um perfil de aluno. |
| **User** | **Teacher** | 1 : 1 | Especialização: Um usuário possui um perfil de docente. |
| **Course** | **Module** | 1 : N | Um curso é composto por vários módulos. |
| **Teacher** | **Module** | 1 : N | Um professor é responsável por ministrar vários módulos. |
| **Student** | **Register** | 1 : N | Um aluno possui várias solicitações de matrícula. |
| **Module** | **Register** | 1 : N | Um módulo recebe matrículas de vários alunos. |
