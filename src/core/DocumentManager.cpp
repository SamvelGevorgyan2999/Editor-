#include "core/DocumentManager.h"

#include <QFileInfo>
#include <QTabWidget>

#include "editor/EditorWidget.h"

DocumentManager::DocumentManager(QTabWidget *tabWidget, QObject *parent)
    : QObject(parent)
    , m_tabs(tabWidget)
{
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int index) {
        emit currentDocumentChanged(editorAt(index));
    });
}

EditorWidget *DocumentManager::createDocument()
{
    auto *editor = new EditorWidget;

    DocumentInfo info;
    info.filePath.clear();
    info.format = FileManager::Format::PlainText;
    info.modified = false;
    m_info.insert(editor, info);

    const QString title = FileManager::untitledName(++m_untitledCounter);
    const int index = m_tabs->addTab(editor, title);
    m_tabs->setCurrentIndex(index);

    // A document becomes modified the moment its contents change.
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, [this, editor](bool changed) {
                setModified(editor, changed);
            });

    editor->setFocus();
    return editor;
}

EditorWidget *DocumentManager::currentEditor() const
{
    return qobject_cast<EditorWidget *>(m_tabs->currentWidget());
}

EditorWidget *DocumentManager::editorAt(int index) const
{
    return qobject_cast<EditorWidget *>(m_tabs->widget(index));
}

int DocumentManager::count() const
{
    return m_tabs->count();
}

int DocumentManager::indexOf(EditorWidget *editor) const
{
    return m_tabs->indexOf(editor);
}

QString DocumentManager::filePath(EditorWidget *editor) const
{
    return m_info.value(editor).filePath;
}

void DocumentManager::setFilePath(EditorWidget *editor, const QString &path)
{
    if (!m_info.contains(editor))
        return;
    m_info[editor].filePath = path;
    m_info[editor].format = FileManager::formatForPath(path);
    updateTabText(editor);

    const int index = indexOf(editor);
    if (index >= 0)
        m_tabs->setTabToolTip(index, QFileInfo(path).absoluteFilePath());
}

FileManager::Format DocumentManager::format(EditorWidget *editor) const
{
    return m_info.value(editor).format;
}

void DocumentManager::setFormat(EditorWidget *editor, FileManager::Format format)
{
    if (m_info.contains(editor))
        m_info[editor].format = format;
}

bool DocumentManager::isModified(EditorWidget *editor) const
{
    return m_info.value(editor).modified;
}

void DocumentManager::setModified(EditorWidget *editor, bool modified)
{
    if (!m_info.contains(editor))
        return;
    if (m_info[editor].modified == modified)
        return;
    m_info[editor].modified = modified;
    updateTabText(editor);
    emit modificationChanged(editor, modified);
}

EditorWidget *DocumentManager::findByPath(const QString &path) const
{
    const QString canonical = QFileInfo(path).absoluteFilePath();
    for (auto it = m_info.cbegin(); it != m_info.cend(); ++it) {
        if (!it.value().filePath.isEmpty()
            && QFileInfo(it.value().filePath).absoluteFilePath() == canonical) {
            return it.key();
        }
    }
    return nullptr;
}

QString DocumentManager::displayName(EditorWidget *editor) const
{
    const QString path = filePath(editor);
    if (path.isEmpty()) {
        const int index = indexOf(editor);
        QString tab = index >= 0 ? m_tabs->tabText(index) : QStringLiteral("Untitled");
        tab.remove(QLatin1Char('*')).remove(QLatin1Char('&'));
        return tab.trimmed();
    }
    return QFileInfo(path).fileName();
}

void DocumentManager::updateTabText(EditorWidget *editor)
{
    const int index = indexOf(editor);
    if (index < 0)
        return;

    const DocumentInfo &info = m_info[editor];
    QString name = info.filePath.isEmpty()
                       ? m_tabs->tabText(index).remove(QLatin1Char('*')).trimmed()
                       : QFileInfo(info.filePath).fileName();
    if (name.isEmpty())
        name = QStringLiteral("Untitled");

    m_tabs->setTabText(index, info.modified ? name + QStringLiteral(" *") : name);
}

void DocumentManager::closeTab(int index)
{
    auto *editor = editorAt(index);
    if (!editor)
        return;
    m_tabs->removeTab(index);
    m_info.remove(editor);
    editor->deleteLater();
}


