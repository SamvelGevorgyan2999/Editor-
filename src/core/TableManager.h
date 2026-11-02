#ifndef ATE_TABLEMANAGER_H
#define ATE_TABLEMANAGER_H

class QTextEdit;

/**
 * @brief Insert and edit tables inside a rich-text document.
 *
 * All operations act on the QTextEdit's current cursor. Row/column mutations
 * require the cursor to be located inside an existing table; the helpers
 * return false otherwise so callers can show appropriate feedback.
 */
class TableManager
{
public:
    static bool insertTable(QTextEdit *editor, int rows, int columns,
                            double borderWidth = 1.0, double cellPadding = 4.0);

    static bool insertRowAbove(QTextEdit *editor);
    static bool insertRowBelow(QTextEdit *editor);
    static bool insertColumnLeft(QTextEdit *editor);
    static bool insertColumnRight(QTextEdit *editor);
    static bool removeCurrentRow(QTextEdit *editor);
    static bool removeCurrentColumn(QTextEdit *editor);
    static bool cursorInTable(QTextEdit *editor);
};

#endif // ATE_TABLEMANAGER_H
