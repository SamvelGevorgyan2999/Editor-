#ifndef ATE_EDITORWIDGET_H
#define ATE_EDITORWIDGET_H

#include <QTextEdit>

class QPaintEvent;
class QResizeEvent;
class LineNumberArea;

/**
 * @brief The core editing surface for a single document / tab.
 *
 * Built on QTextEdit so that rich-text features (tables, embedded images,
 * character/paragraph formatting) work natively, while still behaving as a
 * capable code/plain-text editor. Adds:
 *   - an optional line-number gutter,
 *   - current-line highlighting,
 *   - word-wrap toggling,
 *   - zoom level tracking.
 *
 * Undo/redo is delegated to the underlying QTextDocument, which maintains a
 * full multi-level undo/redo stack out of the box.
 */
class EditorWidget : public QTextEdit
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);

    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    void setLineNumbersVisible(bool visible);
    bool lineNumbersVisible() const { return m_lineNumbersVisible; }

    void setWordWrapEnabled(bool enabled);
    bool wordWrapEnabled() const;

    void zoomInStep();
    void zoomOutStep();
    void resetZoom();
    int zoomLevel() const { return m_zoomLevel; }

    void setEditorFont(const QFont &font);

signals:
    void zoomLevelChanged(int level);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea();
    void highlightCurrentLine();

private:
    LineNumberArea *m_lineNumberArea;
    bool m_lineNumbersVisible = true;
    int m_zoomLevel = 0;
    QFont m_baseFont;
};

#endif // ATE_EDITORWIDGET_H
