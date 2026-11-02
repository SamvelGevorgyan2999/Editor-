#ifndef ATE_FINDREPLACEDIALOG_H
#define ATE_FINDREPLACEDIALOG_H

#include <QDialog>
#include <QTextDocument>

class QLineEdit;
class QCheckBox;

/**
 * @brief Modeless Find & Replace dialog.
 *
 * The dialog owns no document state; it builds QTextDocument::FindFlags from
 * the user's options and emits requests. MainWindow performs the actual
 * search/replace against whichever editor tab is active.
 */
class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);

    QString findText() const;
    QString replaceText() const;
    QTextDocument::FindFlags flags() const;
    bool wrapAround() const;

    void focusFindField();
    void setFindText(const QString &text);

signals:
    void findRequested();
    void replaceRequested();
    void replaceAllRequested();

private:
    QLineEdit *m_findEdit;
    QLineEdit *m_replaceEdit;
    QCheckBox *m_caseSensitive;
    QCheckBox *m_wholeWords;
    QCheckBox *m_searchBackward;
    QCheckBox *m_wrapAround;
};

#endif // ATE_FINDREPLACEDIALOG_H


