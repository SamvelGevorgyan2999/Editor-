#include "core/TableManager.h"

#include <QTextCursor>
#include <QTextEdit>
#include <QTextTable>
#include <QTextTableCell>

bool TableManager::insertTable(QTextEdit *editor, int rows, int columns,
                               double borderWidth, double cellPadding)
{
    if (!editor || rows < 1 || columns < 1)
        return false;

    QTextTableFormat format;
    format.setBorder(borderWidth);
    format.setCellPadding(cellPadding);
    format.setCellSpacing(0);
    format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    format.setHeaderRowCount(1);

    QTextCursor cursor = editor->textCursor();
    cursor.insertTable(rows, columns, format);
    editor->setFocus();
    return true;
}

bool TableManager::cursorInTable(QTextEdit *editor)
{
    return editor && editor->textCursor().currentTable() != nullptr;
}

bool TableManager::insertRowAbove(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->insertRows(cell.row(), 1);
    return true;
}

bool TableManager::insertRowBelow(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->insertRows(cell.row() + 1, 1);
    return true;
}

bool TableManager::insertColumnLeft(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->insertColumns(cell.column(), 1);
    return true;
}

bool TableManager::insertColumnRight(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->insertColumns(cell.column() + 1, 1);
    return true;
}

bool TableManager::removeCurrentRow(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->removeRows(cell.row(), 1);
    return true;
}

bool TableManager::removeCurrentColumn(QTextEdit *editor)
{
    if (!editor)
        return false;
    QTextCursor cursor = editor->textCursor();
    QTextTable *table = cursor.currentTable();
    if (!table)
        return false;
    const QTextTableCell cell = table->cellAt(cursor);
    table->removeColumns(cell.column(), 1);
    return true;
}


