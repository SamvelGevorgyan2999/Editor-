#ifndef ATE_DOCUMENTMANAGER_H
#define ATE_DOCUMENTMANAGER_H

#include <QHash>
#include <QObject>
#include <QString>

#include "core/FileManager.h"

class QTabWidget;
class EditorWidget;

/**
 * @brief Owns the set of open documents presented as tabs.
 *
 * Each tab holds an EditorWidget. The manager tracks per-document metadata
 * (file path, storage format, dirty state) and keeps tab titles in sync with
 * the modified flag (an asterisk marks unsaved changes).
 */
class DocumentManager : public QObject
{
    Q_OBJECT

public:
    explicit DocumentManager(QTabWidget *tabWidget, QObject *parent = nullptr);

    EditorWidget *createDocument();
    EditorWidget *currentEditor() const;
    EditorWidget *editorAt(int index) const;
    int count() const;

    QString filePath(EditorWidget *editor) const;
    void setFilePath(EditorWidget *editor, const QString &path);
    FileManager::Format format(EditorWidget *editor) const;
    void setFormat(EditorWidget *editor, FileManager::Format format);

    bool isModified(EditorWidget *editor) const;
    void setModified(EditorWidget *editor, bool modified);

    EditorWidget *findByPath(const QString &path) const;
    QString displayName(EditorWidget *editor) const;
    void closeTab(int index);

signals:
    void currentDocumentChanged(EditorWidget *editor);
    void modificationChanged(EditorWidget *editor, bool modified);

private:
    struct DocumentInfo {
        QString filePath;
        FileManager::Format format = FileManager::Format::PlainText;
        bool modified = false;
    };

    void updateTabText(EditorWidget *editor);
    int indexOf(EditorWidget *editor) const;

    QTabWidget *m_tabs;
    QHash<EditorWidget *, DocumentInfo> m_info;
    int m_untitledCounter = 0;
};

#endif // ATE_DOCUMENTMANAGER_H
