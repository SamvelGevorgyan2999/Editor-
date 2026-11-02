#ifndef ATE_LINENUMBERAREA_H
#define ATE_LINENUMBERAREA_H

#include <QWidget>

#include "editor/EditorWidget.h"

/**
 * @brief Narrow gutter widget that paints line numbers beside the editor.
 *
 * It delegates sizing and painting back to the owning EditorWidget, which
 * knows the document layout. Kept header-only for brevity.
 */
class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(EditorWidget *editor)
        : QWidget(editor), m_editor(editor) {}

    QSize sizeHint() const override
    {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_editor->lineNumberAreaPaintEvent(event);
    }

private:
    EditorWidget *m_editor;
};

#endif // ATE_LINENUMBERAREA_H


