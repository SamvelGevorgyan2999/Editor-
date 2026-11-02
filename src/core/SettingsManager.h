#ifndef ATE_SETTINGSMANAGER_H
#define ATE_SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>

/**
 * @brief Central persistence for user preferences.
 *
 * Wraps QSettings so the rest of the application never touches raw keys.
 * Stores the UI theme, recent-files list, auto-save configuration and
 * editor view options (word wrap, line numbers, font).
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    enum class Theme { Light, Dark };

    explicit SettingsManager(QObject *parent = nullptr);

    // --- Theme ---------------------------------------------------------
    Theme theme() const;
    void setTheme(Theme theme);

    // --- Recent files --------------------------------------------------
    static constexpr int kMaxRecentFiles = 10;
    QStringList recentFiles() const;
    void addRecentFile(const QString &path);
    void clearRecentFiles();

    // --- Auto-save -----------------------------------------------------
    bool autoSaveEnabled() const;
    void setAutoSaveEnabled(bool enabled);
    int autoSaveIntervalSeconds() const;
    void setAutoSaveIntervalSeconds(int seconds);

    // --- Editor view ---------------------------------------------------
    bool wordWrap() const;
    void setWordWrap(bool enabled);
    bool lineNumbers() const;
    void setLineNumbers(bool enabled);

    QString editorFontFamily() const;
    void setEditorFontFamily(const QString &family);
    int editorFontPointSize() const;
    void setEditorFontPointSize(int size);

    // --- Window geometry ----------------------------------------------
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &geometry);
    QByteArray windowState() const;
    void setWindowState(const QByteArray &state);

signals:
    void themeChanged(SettingsManager::Theme theme);
    void recentFilesChanged();

private:
    QSettings m_settings;
};

#endif // ATE_SETTINGSMANAGER_H


