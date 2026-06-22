#ifndef GUI_STUDENT_WINDOW_H
#define GUI_STUDENT_WINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "Daos.h"

class StudentWindow : public QWidget
{
public:
    explicit StudentWindow(int studentId, QWidget *parent = nullptr)
        : QWidget(parent), studentId(studentId)
    {
        setWindowTitle("Painel do Aluno");
        resize(820, 560);

        auto *tabs = new QTabWidget(this);
        tabs->addTab(buildMeusCursosTab(), "Meus Cursos");
        tabs->addTab(buildSolicitacoesTab(), "Solicitacoes");

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(tabs);

        refreshAll();
    }

private:
    QWidget *buildMeusCursosTab()
    {
        auto *w = new QWidget();
        meusCursosList = new QListWidget(w);
        auto *layout = new QVBoxLayout(w);
        layout->addWidget(new QLabel("Cursos em que voce esta matriculado (aprovados):", w));
        layout->addWidget(meusCursosList);
        return w;
    }

    QWidget *buildSolicitacoesTab()
    {
        auto *w = new QWidget();
        cursosList = new QListWidget(w);
        solicitacoesList = new QListWidget(w);
        auto *btn = new QPushButton("Solicitar matricula", w);

        auto *left = new QVBoxLayout();
        left->addWidget(new QLabel("Cursos disponiveis:", w));
        left->addWidget(cursosList);
        left->addWidget(btn);

        auto *right = new QVBoxLayout();
        right->addWidget(new QLabel("Minhas solicitacoes:", w));
        right->addWidget(solicitacoesList);

        auto *cols = new QHBoxLayout(w);
        cols->addLayout(left);
        cols->addLayout(right);

        connect(btn, &QPushButton::clicked, this, [this] { solicitar(); });
        return w;
    }

    void refreshAll()
    {
        ModuleDAO mdao;
        RegistrationDAO rdao;
        std::vector<Module> modules = mdao.readAll();
        std::vector<Registration> regs = rdao.readAll();

        meusCursosList->clear();
        for (const Registration &r : regs)
        {
            if (r.student_id == studentId && r.approved)
            {
                for (const Module &m : modules)
                    if (m._id == r.module_id)
                        meusCursosList->addItem(QString("ID %1  |  %2").arg(m._id).arg(QString::fromUtf8(m.name)));
            }
        }

        cursosList->clear();
        for (const Module &m : modules)
        {
            auto *item = new QListWidgetItem(QString("ID %1  |  %2").arg(m._id).arg(QString::fromUtf8(m.name)));
            item->setData(Qt::UserRole, m._id);
            cursosList->addItem(item);
        }

        solicitacoesList->clear();
        for (const Registration &r : regs)
        {
            if (r.student_id != studentId) continue;
            QString nome = QString("Curso %1").arg(r.module_id);
            for (const Module &m : modules)
                if (m._id == r.module_id) nome = QString::fromUtf8(m.name);
            QString status = r.revised ? (r.approved ? "Aprovada" : "Recusada") : "Pendente";
            solicitacoesList->addItem(QString("%1  -  %2").arg(nome, status));
        }
    }

    void solicitar()
    {
        auto *item = cursosList->currentItem();
        if (!item) return;
        int moduleId = item->data(Qt::UserRole).toInt();

        RegistrationDAO rdao;
        for (const Registration &r : rdao.readAll())
        {
            if (r.student_id == studentId && r.module_id == moduleId)
            {
                QMessageBox::information(this, "Matricula", "Voce ja solicitou matricula neste curso.");
                return;
            }
        }

        Registration r;
        r.student_id = studentId;
        r.module_id = moduleId;
        r.revised = false;
        r.approved = false;
        if (rdao.create(r))
        {
            QMessageBox::information(this, "Matricula", "Solicitacao enviada. Aguarde aprovacao.");
            refreshAll();
        }
    }

    int studentId;
    QListWidget *meusCursosList = nullptr;
    QListWidget *cursosList = nullptr;
    QListWidget *solicitacoesList = nullptr;
};

#endif
