#include "dialogs/FindReplaceDialog.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Find & Replace"));
    setModal(false);

    m_findEdit    = new QLineEdit(this);
    m_replaceEdit = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Find:"), m_findEdit);
    form->addRow(QStringLiteral("Replace with:"), m_replaceEdit);

    m_caseSensitive  = new QCheckBox(QStringLiteral("Match &case"), this);
    m_wholeWords     = new QCheckBox(QStringLiteral("Whole &words only"), this);
    m_searchBackward = new QCheckBox(QStringLiteral("Search &backward"), this);
    m_wrapAround     = new QCheckBox(QStringLiteral("Wrap &around"), this);
    m_wrapAround->setChecked(true);

    auto *options = new QVBoxLayout;
    options->addWidget(m_caseSensitive);
    options->addWidget(m_wholeWords);
    options->addWidget(m_searchBackward);
    options->addWidget(m_wrapAround);

    auto *findButton       = new QPushButton(QStringLiteral("Find &Next"), this);
    auto *replaceButton    = new QPushButton(QStringLiteral("&Replace"), this);
    auto *replaceAllButton = new QPushButton(QStringLiteral("Replace &All"), this);
    auto *closeButton      = new QPushButton(QStringLiteral("Close"), this);
    findButton->setDefault(true);

    auto *buttons = new QVBoxLayout;
    buttons->addWidget(findButton);
    buttons->addWidget(replaceButton);
    buttons->addWidget(replaceAllButton);
    buttons->addStretch();
    buttons->addWidget(closeButton);

    auto *middle = new QHBoxLayout;
    middle->addLayout(options);
    middle->addStretch();
    middle->addLayout(buttons);

    auto *root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addLayout(middle);

    connect(findButton,       &QPushButton::clicked, this, &FindReplaceDialog::findRequested);
    connect(replaceButton,    &QPushButton::clicked, this, &FindReplaceDialog::replaceRequested);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::replaceAllRequested);
    connect(closeButton,      &QPushButton::clicked, this, &QDialog::hide);
}

QString FindReplaceDialog::findText() const
{
    return m_findEdit->text();
}

QString FindReplaceDialog::replaceText() const
{
    return m_replaceEdit->text();
}

QTextDocument::FindFlags FindReplaceDialog::flags() const
{
    QTextDocument::FindFlags f;
    if (m_caseSensitive->isChecked())
        f |= QTextDocument::FindCaseSensitively;
    if (m_wholeWords->isChecked())
        f |= QTextDocument::FindWholeWords;
    if (m_searchBackward->isChecked())
        f |= QTextDocument::FindBackward;
    return f;
}

bool FindReplaceDialog::wrapAround() const
{
    return m_wrapAround->isChecked();
}

void FindReplaceDialog::focusFindField()
{
    m_findEdit->setFocus();
    m_findEdit->selectAll();
}

void FindReplaceDialog::setFindText(const QString &text)
{
    if (!text.isEmpty())
        m_findEdit->setText(text);
}


