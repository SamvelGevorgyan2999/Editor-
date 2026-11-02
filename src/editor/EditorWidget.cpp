#include "editor/EditorWidget.h"
#include "editor/LineNumberArea.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocument>

EditorWidget::EditorWidget(QWidget *parent)
    : QTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);
    m_baseFont = font();

    setAcceptRichText(true);
    setTabStopDistance(4 * fontMetrics().horizontalAdvance(QLatin1Char(' ')));

    // Keep the gutter in sync with content, scrolling and layout changes.
    connect(document(), &QTextDocument::blockCountChanged,
            this, &EditorWidget::updateLineNumberAreaWidth);
    connect(this, &QTextEdit::textChanged,
            this, &EditorWidget::updateLineNumberArea);
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &EditorWidget::updateLineNumberArea);
    connect(this, &QTextEdit::cursorPositionChanged,
            this, &EditorWidget::highlightCurrentLine);

    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

int EditorWidget::lineNumberAreaWidth() const
{
    if (!m_lineNumbersVisible)
        return 0;

    int digits = 1;
    int max = qMax(1, document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    const int space = 12 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void EditorWidget::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorWidget::updateLineNumberArea()
{
    if (m_lineNumbersVisible)
        m_lineNumberArea->update();
}

void EditorWidget::setLineNumbersVisible(bool visible)
{
    m_lineNumbersVisible = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth();
    m_lineNumberArea->update();
}

void EditorWidget::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void EditorWidget::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);

    const bool dark = palette().color(QPalette::Base).lightness() < 128;
    const QColor bg  = dark ? QColor(40, 40, 40)  : QColor(240, 240, 240);
    const QColor fg  = dark ? QColor(140, 140, 140): QColor(120, 120, 120);
    painter.fillRect(event->rect(), bg);
    painter.setPen(fg);

    // Walk the document blocks and paint the number of each visible line.
    // cursorRect() gives viewport coordinates, which keeps us correct across
    // word-wrap, variable line heights and scrolling.
    for (QTextBlock block = document()->begin(); block.isValid(); block = block.next()) {
        QTextCursor cursor(block);
        cursor.movePosition(QTextCursor::StartOfBlock);
        const QRect r = cursorRect(cursor);

        if (r.bottom() < 0)
            continue;                 // above the viewport
        if (r.top() > viewport()->height())
            break;                    // below the viewport

        const QString number = QString::number(block.blockNumber() + 1);
        painter.drawText(0, r.top(), m_lineNumberArea->width() - 6,
                         fontMetrics().height(),
                         Qt::AlignRight | Qt::AlignTop, number);
    }
}
void EditorWidget::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        const bool dark = palette().color(QPalette::Base).lightness() < 128;
        const QColor lineColor = dark ? QColor(60, 60, 70) : QColor(232, 242, 254);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void EditorWidget::setWordWrapEnabled(bool enabled)
{
    setLineWrapMode(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

bool EditorWidget::wordWrapEnabled() const
{
    return lineWrapMode() == QTextEdit::WidgetWidth;
}

void EditorWidget::setEditorFont(const QFont &font)
{
    m_baseFont = font;
    m_zoomLevel = 0;
    setFont(font);
    setTabStopDistance(4 * fontMetrics().horizontalAdvance(QLatin1Char(' ')));
    updateLineNumberAreaWidth();
    emit zoomLevelChanged(m_zoomLevel);
}

void EditorWidget::zoomInStep()
{
    zoomIn(1);
    ++m_zoomLevel;
    updateLineNumberAreaWidth();
    emit zoomLevelChanged(m_zoomLevel);
}

void EditorWidget::zoomOutStep()
{
    zoomOut(1);
    --m_zoomLevel;
    updateLineNumberAreaWidth();
    emit zoomLevelChanged(m_zoomLevel);
}

void EditorWidget::resetZoom()
{
    setFont(m_baseFont);
    m_zoomLevel = 0;
    updateLineNumberAreaWidth();
    emit zoomLevelChanged(m_zoomLevel);
}


