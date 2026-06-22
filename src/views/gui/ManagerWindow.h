#ifndef GUI_MANAGER_WINDOW_H
#define GUI_MANAGER_WINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>

#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <chrono>

#include "Daos.h"
#include "../../utils/pattern/KMP.h"
#include "../../utils/pattern/BoyerMoore.h"

class ManagerWindow : public QWidget
{
public:
    explicit ManagerWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("Painel do Gerente");
        resize(900, 600);

        auto *tabs = new QTabWidget(this);
        tabs->addTab(buildAlunosTab(), "Alunos");
        tabs->addTab(buildProfessoresTab(), "Professores");
        tabs->addTab(buildCursosTab(), "Cursos");
        tabs->addTab(buildMatriculasTab(), "Matriculas");
        tabs->addTab(buildPesquisaTab(), "Pesquisa");
        tabs->addTab(buildBackupTab(), "Backup");

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(tabs);

        refreshAlunos();
        refreshProfessores();
        refreshCursos();
        refreshModuleCombo();
    }

private:

    static void setText(char *dst, int size, const QString &src)
    {
        std::memset(dst, 0, size);
        std::strncpy(dst, src.toUtf8().constData(), size - 1);
    }

    int createUser(const QString &uname, const QString &pwd, const char *type)
    {
        User u;
        setText(u.username, sizeof(u.username), uname);
        setText(u.password, sizeof(u.password), pwd);
        setText(u.type, sizeof(u.type), type);
        UserDAO dao;
        dao.create(u);
        return u._id;
    }

    int selectedId(QListWidget *list)
    {
        auto *item = list->currentItem();
        return item ? item->data(Qt::UserRole).toInt() : -1;
    }

    static bool matchPattern(int algo, const std::string &text, const std::string &pattern)
    {
        if (pattern.empty())
            return true;
        std::string t = text, p = pattern;
        std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c) { return std::tolower(c); });
        std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c) { return std::tolower(c); });
        return (algo == 1) ? BoyerMoore::contains(t, p) : KMP::contains(t, p);
    }

    QWidget *buildAlunosTab()
    {
        auto *w = new QWidget();
        alunosList = new QListWidget(w);
        alunosSearch = new QLineEdit(w);
        alunosSearch->setPlaceholderText("Buscar por nome (casamento de padrao)...");
        alunosAlgo = new QComboBox(w);
        alunosAlgo->addItem("KMP");
        alunosAlgo->addItem("Boyer-Moore");

        auto *searchRow = new QHBoxLayout();
        searchRow->addWidget(alunosSearch);
        searchRow->addWidget(alunosAlgo);

        auto *btnNovo = new QPushButton("Cadastrar", w);
        auto *btnEdit = new QPushButton("Editar", w);
        auto *btnDel = new QPushButton("Excluir", w);

        auto *btns = new QHBoxLayout();
        btns->addWidget(btnNovo);
        btns->addWidget(btnEdit);
        btns->addWidget(btnDel);
        btns->addStretch();

        auto *layout = new QVBoxLayout(w);
        layout->addLayout(searchRow);
        layout->addWidget(alunosList);
        layout->addLayout(btns);

        connect(alunosSearch, &QLineEdit::textChanged, this, [this] { refreshAlunos(); });
        connect(alunosAlgo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { refreshAlunos(); });
        connect(btnNovo, &QPushButton::clicked, this, [this] { alunoDialog(-1); });
        connect(btnEdit, &QPushButton::clicked, this, [this] { alunoDialog(selectedId(alunosList)); });
        connect(btnDel, &QPushButton::clicked, this, [this] {
            int id = selectedId(alunosList);
            if (id == -1) return;
            StudentDAO dao;
            if (dao.remove(id)) refreshAlunos();
        });
        return w;
    }

    void refreshAlunos()
    {
        alunosList->clear();
        StudentDAO dao;
        std::string filter = alunosSearch ? alunosSearch->text().trimmed().toStdString() : "";
        int algo = alunosAlgo ? alunosAlgo->currentIndex() : 0;

        for (const Student &s : dao.listOrderedById())
        {
            QString name = QString::fromUtf8(s.name);

            if (!matchPattern(algo, s.name, filter))
                continue;
            auto *item = new QListWidgetItem(
                QString("ID %1  |  %2  |  Nasc: %3").arg(s._id).arg(name).arg((long long)s.birth_date));
            item->setData(Qt::UserRole, s._id);
            alunosList->addItem(item);
        }
    }

    void alunoDialog(int id)
    {
        bool isNew = (id == -1);
        Student s;
        if (!isNew)
        {
            StudentDAO dao;
            s = dao.read(id);
            if (s._id == -1) return;
        }

        QDialog dlg(this);
        dlg.setWindowTitle(isNew ? "Cadastrar Aluno" : "Editar Aluno");
        auto *name = new QLineEdit(QString::fromUtf8(s.name), &dlg);
        auto *birth = new QLineEdit(isNew ? "" : QString::number((long long)s.birth_date), &dlg);
        auto *uname = new QLineEdit(&dlg);
        auto *pwd = new QLineEdit(&dlg);
        pwd->setEchoMode(QLineEdit::Password);

        auto *form = new QFormLayout();
        form->addRow("Nome:", name);
        form->addRow("Nascimento (AAAAMMDD):", birth);
        if (isNew)
        {
            form->addRow("Usuario (login):", uname);
            form->addRow("Senha:", pwd);
        }
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        auto *lay = new QVBoxLayout(&dlg);
        lay->addLayout(form);
        lay->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) return;

        setText(s.name, sizeof(s.name), name->text());
        s.birth_date = birth->text().toLongLong();

        StudentDAO dao;
        if (isNew)
        {
            if (uname->text().isEmpty())
            {
                QMessageBox::warning(this, "Aluno", "Informe um usuario para login.");
                return;
            }
            s.user_id = createUser(uname->text(), pwd->text(), "aluno");
            dao.create(s);
        }
        else
        {
            dao.update(s);
        }
        refreshAlunos();
    }

    QWidget *buildProfessoresTab()
    {
        auto *w = new QWidget();
        profList = new QListWidget(w);
        auto *btnNovo = new QPushButton("Cadastrar", w);
        auto *btnEdit = new QPushButton("Editar", w);
        auto *btnDel = new QPushButton("Excluir", w);

        auto *btns = new QHBoxLayout();
        btns->addWidget(btnNovo);
        btns->addWidget(btnEdit);
        btns->addWidget(btnDel);
        btns->addStretch();

        auto *layout = new QVBoxLayout(w);
        layout->addWidget(profList);
        layout->addLayout(btns);

        connect(btnNovo, &QPushButton::clicked, this, [this] { profDialog(-1); });
        connect(btnEdit, &QPushButton::clicked, this, [this] { profDialog(selectedId(profList)); });
        connect(btnDel, &QPushButton::clicked, this, [this] {
            int id = selectedId(profList);
            if (id == -1) return;
            TeacherDAO dao;
            if (dao.remove(id)) { refreshProfessores(); refreshModuleCombo(); }
        });
        return w;
    }

    void refreshProfessores()
    {
        profList->clear();
        TeacherDAO dao;
        for (const Teacher &t : dao.readAll())
        {
            auto *item = new QListWidgetItem(
                QString("ID %1  |  %2  |  %3").arg(t._id).arg(QString::fromUtf8(t.name)).arg(QString::fromUtf8(t.expertise)));
            item->setData(Qt::UserRole, t._id);
            profList->addItem(item);
        }
    }

    void profDialog(int id)
    {
        bool isNew = (id == -1);
        Teacher t;
        if (!isNew)
        {
            TeacherDAO dao;
            t = dao.read(id);
            if (t._id == -1) return;
        }

        QDialog dlg(this);
        dlg.setWindowTitle(isNew ? "Cadastrar Professor" : "Editar Professor");
        auto *name = new QLineEdit(QString::fromUtf8(t.name), &dlg);
        auto *birth = new QLineEdit(isNew ? "" : QString::number((long long)t.birth_date), &dlg);
        auto *exp = new QLineEdit(QString::fromUtf8(t.expertise), &dlg);
        auto *uname = new QLineEdit(&dlg);
        auto *pwd = new QLineEdit(&dlg);
        pwd->setEchoMode(QLineEdit::Password);

        auto *form = new QFormLayout();
        form->addRow("Nome:", name);
        form->addRow("Nascimento (AAAAMMDD):", birth);
        form->addRow("Especialidade:", exp);
        if (isNew)
        {
            form->addRow("Usuario (login):", uname);
            form->addRow("Senha:", pwd);
        }
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        auto *lay = new QVBoxLayout(&dlg);
        lay->addLayout(form);
        lay->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) return;

        setText(t.name, sizeof(t.name), name->text());
        t.birth_date = birth->text().toLongLong();
        setText(t.expertise, sizeof(t.expertise), exp->text());

        TeacherDAO dao;
        if (isNew)
        {
            if (uname->text().isEmpty())
            {
                QMessageBox::warning(this, "Professor", "Informe um usuario para login.");
                return;
            }
            t.user_id = createUser(uname->text(), pwd->text(), "professor");
            dao.create(t);
        }
        else
        {
            dao.update(t);
        }
        refreshProfessores();
        refreshModuleCombo();
    }

    QWidget *buildCursosTab()
    {
        auto *w = new QWidget();
        cursosList = new QListWidget(w);
        auto *btnNovo = new QPushButton("Cadastrar", w);
        auto *btnEdit = new QPushButton("Editar", w);
        auto *btnDel = new QPushButton("Excluir", w);

        auto *btns = new QHBoxLayout();
        btns->addWidget(btnNovo);
        btns->addWidget(btnEdit);
        btns->addWidget(btnDel);
        btns->addStretch();

        auto *layout = new QVBoxLayout(w);
        layout->addWidget(cursosList);
        layout->addLayout(btns);

        connect(btnNovo, &QPushButton::clicked, this, [this] { cursoDialog(-1); });
        connect(btnEdit, &QPushButton::clicked, this, [this] { cursoDialog(selectedId(cursosList)); });
        connect(btnDel, &QPushButton::clicked, this, [this] {
            int id = selectedId(cursosList);
            if (id == -1) return;
            ModuleDAO dao;
            if (dao.remove(id)) { refreshCursos(); refreshModuleCombo(); }
        });
        return w;
    }

    void refreshCursos()
    {
        cursosList->clear();
        ModuleDAO dao;
        TeacherDAO tdao;
        std::vector<Teacher> teachers = tdao.readAll();
        for (const Module &m : dao.readAll())
        {
            QString prof = "-";
            for (const Teacher &t : teachers)
                if (t._id == m.teacher_id) prof = QString::fromUtf8(t.name);
            auto *item = new QListWidgetItem(
                QString("ID %1  |  %2  |  Prof: %3").arg(m._id).arg(QString::fromUtf8(m.name)).arg(prof));
            item->setData(Qt::UserRole, m._id);
            cursosList->addItem(item);
        }
    }

    void cursoDialog(int id)
    {
        bool isNew = (id == -1);
        Module m;
        if (!isNew)
        {
            ModuleDAO dao;
            m = dao.read(id);
            if (m._id == -1) return;
        }

        QDialog dlg(this);
        dlg.setWindowTitle(isNew ? "Cadastrar Curso" : "Editar Curso");
        auto *name = new QLineEdit(QString::fromUtf8(m.name), &dlg);
        auto *courseId = new QLineEdit(isNew ? "" : QString::number(m.course_id), &dlg);
        auto *teacherCombo = new QComboBox(&dlg);
        TeacherDAO tdao;
        for (const Teacher &t : tdao.readAll())
            teacherCombo->addItem(QString("%1 (ID %2)").arg(QString::fromUtf8(t.name)).arg(t._id), t._id);
        if (!isNew)
        {
            int idx = teacherCombo->findData(m.teacher_id);
            if (idx >= 0) teacherCombo->setCurrentIndex(idx);
        }

        auto *form = new QFormLayout();
        form->addRow("Nome do curso:", name);
        form->addRow("Course ID:", courseId);
        form->addRow("Professor:", teacherCombo);
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
        auto *lay = new QVBoxLayout(&dlg);
        lay->addLayout(form);
        lay->addWidget(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() != QDialog::Accepted) return;

        setText(m.name, sizeof(m.name), name->text());
        m.course_id = courseId->text().toInt();
        m.teacher_id = teacherCombo->currentData().isValid() ? teacherCombo->currentData().toInt() : -1;

        ModuleDAO dao;
        if (isNew) dao.create(m);
        else dao.update(m);
        refreshCursos();
        refreshModuleCombo();
    }

    QWidget *buildMatriculasTab()
    {
        auto *w = new QWidget();
        moduleCombo = new QComboBox(w);
        matriculasList = new QListWidget(w);
        auto *info = new QLabel("Selecione um curso para listar as matriculas (busca via Hash Extensivel):", w);

        auto *layout = new QVBoxLayout(w);
        layout->addWidget(info);
        layout->addWidget(moduleCombo);
        layout->addWidget(matriculasList);

        connect(moduleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this](int) { refreshMatriculas(); });
        return w;
    }

    void refreshModuleCombo()
    {
        if (!moduleCombo) return;
        int prev = moduleCombo->currentData().isValid() ? moduleCombo->currentData().toInt() : -1;
        moduleCombo->clear();
        ModuleDAO dao;
        for (const Module &m : dao.readAll())
            moduleCombo->addItem(QString("%1 (ID %2)").arg(QString::fromUtf8(m.name)).arg(m._id), m._id);
        int idx = moduleCombo->findData(prev);
        if (idx >= 0) moduleCombo->setCurrentIndex(idx);
        refreshMatriculas();
    }

    void refreshMatriculas()
    {
        if (!matriculasList) return;
        matriculasList->clear();
        if (!moduleCombo->currentData().isValid()) return;
        int moduleId = moduleCombo->currentData().toInt();

        RegistrationDAO rdao;
        StudentDAO sdao;
        for (const Registration &r : rdao.findByModule(moduleId))
        {
            Student s = sdao.read(r.student_id);
            QString name = (s._id != -1) ? QString::fromUtf8(s.name) : QString("Aluno %1").arg(r.student_id);
            QString status = r.revised ? (r.approved ? "Aprovada" : "Recusada") : "Pendente";
            matriculasList->addItem(QString("Matricula %1  |  %2  |  %3").arg(r._id).arg(name).arg(status));
        }
    }

    QWidget *buildPesquisaTab()
    {
        auto *w = new QWidget();
        pesqEntidade = new QComboBox(w);
        pesqEntidade->addItem("Nome do Aluno");
        pesqEntidade->addItem("Nome do Curso");
        pesqAlgo = new QComboBox(w);
        pesqAlgo->addItem("KMP");
        pesqAlgo->addItem("Boyer-Moore");
        pesqPadrao = new QLineEdit(w);
        pesqPadrao->setPlaceholderText("Digite o padrao a pesquisar...");
        auto *btn = new QPushButton("Pesquisar", w);
        pesqInfo = new QLabel("Informe um padrao e clique em Pesquisar.", w);
        pesqResultados = new QListWidget(w);

        auto *row = new QHBoxLayout();
        row->addWidget(new QLabel("Buscar em:", w));
        row->addWidget(pesqEntidade);
        row->addWidget(new QLabel("Algoritmo:", w));
        row->addWidget(pesqAlgo);

        auto *row2 = new QHBoxLayout();
        row2->addWidget(pesqPadrao);
        row2->addWidget(btn);

        auto *layout = new QVBoxLayout(w);
        layout->addLayout(row);
        layout->addLayout(row2);
        layout->addWidget(pesqInfo);
        layout->addWidget(pesqResultados);

        connect(btn, &QPushButton::clicked, this, [this] { doPesquisa(); });
        connect(pesqPadrao, &QLineEdit::returnPressed, this, [this] { doPesquisa(); });
        return w;
    }

    void doPesquisa()
    {
        pesqResultados->clear();
        std::string pattern = pesqPadrao->text().trimmed().toStdString();
        int algo = pesqAlgo->currentIndex();
        const char *algoNome = (algo == 1) ? "Boyer-Moore" : "KMP";

        std::vector<QString> achados;
        double micros = 0.0;

        if (pesqEntidade->currentIndex() == 0)
        {
            StudentDAO dao;
            std::vector<Student> regs = dao.readAll();
            auto t0 = std::chrono::high_resolution_clock::now();
            for (const Student &s : regs)
                if (matchPattern(algo, s.name, pattern))
                    achados.push_back(QString("Aluno ID %1  |  %2").arg(s._id).arg(QString::fromUtf8(s.name)));
            auto t1 = std::chrono::high_resolution_clock::now();
            micros = std::chrono::duration<double, std::micro>(t1 - t0).count();
        }
        else
        {
            ModuleDAO dao;
            std::vector<Module> regs = dao.readAll();
            auto t0 = std::chrono::high_resolution_clock::now();
            for (const Module &m : regs)
                if (matchPattern(algo, m.name, pattern))
                    achados.push_back(QString("Curso ID %1  |  %2").arg(m._id).arg(QString::fromUtf8(m.name)));
            auto t1 = std::chrono::high_resolution_clock::now();
            micros = std::chrono::duration<double, std::micro>(t1 - t0).count();
        }

        for (const QString &a : achados)
            pesqResultados->addItem(a);

        pesqInfo->setText(QString("Algoritmo: %1  |  %2 registro(s) encontrado(s)  |  tempo: %3 us")
                              .arg(algoNome)
                              .arg(achados.size())
                              .arg(micros, 0, 'f', 1));
    }

    QWidget *buildBackupTab()
    {
        auto *w = new QWidget();
        auto *hb = new QPushButton("Compactar (Huffman)", w);
        auto *lb = new QPushButton("Compactar (LZW)", w);
        auto *hr = new QPushButton("Restaurar (Huffman)", w);
        auto *lr = new QPushButton("Restaurar (LZW)", w);
        backupLog = new QPlainTextEdit(w);
        backupLog->setReadOnly(true);

        auto *row1 = new QHBoxLayout();
        row1->addWidget(hb);
        row1->addWidget(lb);
        auto *row2 = new QHBoxLayout();
        row2->addWidget(hr);
        row2->addWidget(lr);

        auto *layout = new QVBoxLayout(w);
        layout->addLayout(row1);
        layout->addLayout(row2);
        layout->addWidget(backupLog);

        connect(hb, &QPushButton::clicked, this, [this] { doCompress(BackupManager::Algorithm::HUFFMAN, "Huffman"); });
        connect(lb, &QPushButton::clicked, this, [this] { doCompress(BackupManager::Algorithm::LZW, "LZW"); });
        connect(hr, &QPushButton::clicked, this, [this] { doRestore(BackupManager::Algorithm::HUFFMAN, "Huffman"); });
        connect(lr, &QPushButton::clicked, this, [this] { doRestore(BackupManager::Algorithm::LZW, "LZW"); });
        return w;
    }

    void doCompress(BackupManager::Algorithm algo, const QString &nome)
    {
        BackupManager::Stats s = BackupManager::compress(algo);
        if (!s.ok)
        {
            backupLog->appendPlainText(QString("[%1] Falha ao compactar.").arg(nome));
            return;
        }
        double ratio = s.originalSize ? (double)s.compressedSize / (double)s.originalSize : 0.0;
        backupLog->appendPlainText(QString("[%1] %2 -> %3 bytes | taxa %4% | economia %5%")
                                       .arg(nome)
                                       .arg(s.originalSize)
                                       .arg(s.compressedSize)
                                       .arg(ratio * 100.0, 0, 'f', 2)
                                       .arg((1.0 - ratio) * 100.0, 0, 'f', 2));
        backupLog->appendPlainText(QString("       arquivo: %1").arg(QString::fromStdString(s.outputPath)));
    }

    void doRestore(BackupManager::Algorithm algo, const QString &nome)
    {
        bool ok = BackupManager::restore(algo);
        backupLog->appendPlainText(QString("[%1] Restauracao %2.").arg(nome, ok ? "concluida" : "falhou"));
        if (ok)
        {
            refreshAlunos();
            refreshProfessores();
            refreshCursos();
            refreshModuleCombo();
        }
    }

    QListWidget *alunosList = nullptr;
    QLineEdit *alunosSearch = nullptr;
    QComboBox *alunosAlgo = nullptr;
    QListWidget *profList = nullptr;
    QListWidget *cursosList = nullptr;
    QComboBox *moduleCombo = nullptr;
    QListWidget *matriculasList = nullptr;
    QComboBox *pesqEntidade = nullptr;
    QComboBox *pesqAlgo = nullptr;
    QLineEdit *pesqPadrao = nullptr;
    QLabel *pesqInfo = nullptr;
    QListWidget *pesqResultados = nullptr;
    QPlainTextEdit *backupLog = nullptr;
};

#endif
