#ifndef GUI_PROFESSOR_WINDOW_H
#define GUI_PROFESSOR_WINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <vector>

#include "Daos.h"

class ProfessorWindow : public QWidget
{
public:
    explicit ProfessorWindow(int teacherId, QWidget *parent = nullptr)
        : QWidget(parent), teacherId(teacherId)
    {
        setWindowTitle("Painel do Professor");
        resize(860, 580);

        auto *tabs = new QTabWidget(this);
        tabs->addTab(buildMeusCursosTab(), "Meus Cursos");
        tabs->addTab(buildSolicitacoesTab(), "Solicitacoes");

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(tabs);

        refreshCursos();
        refreshSolicitacoes();
    }

private:
    std::vector<int> myModuleIds()
    {
        std::vector<int> ids;
        ModuleDAO mdao;
        for (const Module &m : mdao.readAll())
            if (m.teacher_id == teacherId)
                ids.push_back(m._id);
        return ids;
    }

    QWidget *buildMeusCursosTab()
    {
        auto *w = new QWidget();
        cursosList = new QListWidget(w);
        alunosCursoList = new QListWidget(w);

        auto *left = new QVBoxLayout();
        left->addWidget(new QLabel("Meus cursos:", w));
        left->addWidget(cursosList);

        auto *right = new QVBoxLayout();
        right->addWidget(new QLabel("Alunos matriculados (aprovados):", w));
        right->addWidget(alunosCursoList);

        auto *cols = new QHBoxLayout(w);
        cols->addLayout(left);
        cols->addLayout(right);

        connect(cursosList, &QListWidget::currentItemChanged, this, [this] { refreshAlunosDoCurso(); });
        return w;
    }

    QWidget *buildSolicitacoesTab()
    {
        auto *w = new QWidget();
        solicitacoesList = new QListWidget(w);
        auto *aprovar = new QPushButton("Aprovar", w);
        auto *recusar = new QPushButton("Recusar", w);

        auto *btns = new QHBoxLayout();
        btns->addWidget(aprovar);
        btns->addWidget(recusar);
        btns->addStretch();

        auto *layout = new QVBoxLayout(w);
        layout->addWidget(new QLabel("Solicitacoes pendentes nos seus cursos:", w));
        layout->addWidget(solicitacoesList);
        layout->addLayout(btns);

        connect(aprovar, &QPushButton::clicked, this, [this] { revisar(true); });
        connect(recusar, &QPushButton::clicked, this, [this] { revisar(false); });
        return w;
    }

    void refreshCursos()
    {
        cursosList->clear();
        ModuleDAO mdao;
        for (const Module &m : mdao.readAll())
        {
            if (m.teacher_id != teacherId) continue;
            auto *item = new QListWidgetItem(QString("ID %1  |  %2").arg(m._id).arg(QString::fromUtf8(m.name)));
            item->setData(Qt::UserRole, m._id);
            cursosList->addItem(item);
        }
        refreshAlunosDoCurso();
    }

    void refreshAlunosDoCurso()
    {
        alunosCursoList->clear();
        auto *item = cursosList->currentItem();
        if (!item) return;
        int moduleId = item->data(Qt::UserRole).toInt();

        RegistrationDAO rdao;
        StudentDAO sdao;

        for (const Registration &r : rdao.findByModule(moduleId))
        {
            if (!r.approved) continue;
            Student s = sdao.read(r.student_id);
            QString name = (s._id != -1) ? QString::fromUtf8(s.name) : QString("Aluno %1").arg(r.student_id);
            alunosCursoList->addItem(QString("ID %1  |  %2").arg(s._id).arg(name));
        }
    }

    void refreshSolicitacoes()
    {
        solicitacoesList->clear();
        std::vector<int> mods = myModuleIds();
        RegistrationDAO rdao;
        StudentDAO sdao;
        ModuleDAO mdao;
        for (const Registration &r : rdao.readAll())
        {
            if (r.revised) continue;
            bool mine = false;
            for (int id : mods) if (id == r.module_id) mine = true;
            if (!mine) continue;

            Student s = sdao.read(r.student_id);
            Module m = mdao.read(r.module_id);
            QString sname = (s._id != -1) ? QString::fromUtf8(s.name) : QString("Aluno %1").arg(r.student_id);
            QString mname = (m._id != -1) ? QString::fromUtf8(m.name) : QString("Curso %1").arg(r.module_id);
            auto *item = new QListWidgetItem(QString("Matricula %1  |  %2  ->  %3").arg(r._id).arg(sname, mname));
            item->setData(Qt::UserRole, r._id);
            solicitacoesList->addItem(item);
        }
    }

    void revisar(bool aprovar)
    {
        auto *item = solicitacoesList->currentItem();
        if (!item) return;
        int regId = item->data(Qt::UserRole).toInt();

        RegistrationDAO rdao;
        Registration r = rdao.read(regId);
        if (r._id == -1) return;
        r.revised = true;
        r.approved = aprovar;
        rdao.update(r);

        refreshSolicitacoes();
        refreshAlunosDoCurso();
    }

    int teacherId;
    QListWidget *cursosList = nullptr;
    QListWidget *alunosCursoList = nullptr;
    QListWidget *solicitacoesList = nullptr;
};

#endif
