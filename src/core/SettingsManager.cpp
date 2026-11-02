#include "core/SettingsManager.h"

#include <QFileInfo>

namespace {
constexpr auto kKeyTheme            = "ui/theme";
constexpr auto kKeyRecentFiles      = "files/recent";
constexpr auto kKeyAutoSaveEnabled  = "autosave/enabled";
constexpr auto kKeyAutoSaveInterval = "autosave/intervalSeconds";
constexpr auto kKeyWordWrap         = "editor/wordWrap";
constexpr auto kKeyLineNumbers      = "editor/lineNumbers";
constexpr auto kKeyFontFamily       = "editor/fontFamily";
constexpr auto kKeyFontSize         = "editor/fontPointSize";
constexpr auto kKeyWindowGeometry   = "window/geometry";
constexpr auto kKeyWindowState      = "window/state";
} // namespace

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

SettingsManager::Theme SettingsManager::theme() const
{
    const int value = m_settings.value(kKeyTheme,
                                       static_cast<int>(Theme::Light)).toInt();
    return value == static_cast<int>(Theme::Dark) ? Theme::Dark : Theme::Light;
}

void SettingsManager::setTheme(Theme theme)
{
    if (this->theme() == theme)
        return;
    m_settings.setValue(kKeyTheme, static_cast<int>(theme));
    emit themeChanged(theme);
}

QStringList SettingsManager::recentFiles() const
{
    return m_settings.value(kKeyRecentFiles).toStringList();
}

void SettingsManager::addRecentFile(const QString &path)
{
    if (path.isEmpty())
        return;

    const QString canonical = QFileInfo(path).absoluteFilePath();
    QStringList files = recentFiles();
    files.removeAll(canonical);
    files.prepend(canonical);
    while (files.size() > kMaxRecentFiles)
        files.removeLast();

    m_settings.setValue(kKeyRecentFiles, files);
    emit recentFilesChanged();
}

void SettingsManager::clearRecentFiles()
{
    m_settings.remove(kKeyRecentFiles);
    emit recentFilesChanged();
}

bool SettingsManager::autoSaveEnabled() const
{
    return m_settings.value(kKeyAutoSaveEnabled, false).toBool();
}

void SettingsManager::setAutoSaveEnabled(bool enabled)
{
    m_settings.setValue(kKeyAutoSaveEnabled, enabled);
}

int SettingsManager::autoSaveIntervalSeconds() const
{
    return m_settings.value(kKeyAutoSaveInterval, 60).toInt();
}

void SettingsManager::setAutoSaveIntervalSeconds(int seconds)
{
    m_settings.setValue(kKeyAutoSaveInterval, qMax(5, seconds));
}

bool SettingsManager::wordWrap() const
{
    return m_settings.value(kKeyWordWrap, true).toBool();
}

void SettingsManager::setWordWrap(bool enabled)
{
    m_settings.setValue(kKeyWordWrap, enabled);
}

bool SettingsManager::lineNumbers() const
{
    return m_settings.value(kKeyLineNumbers, true).toBool();
}

void SettingsManager::setLineNumbers(bool enabled)
{
    m_settings.setValue(kKeyLineNumbers, enabled);
}

QString SettingsManager::editorFontFamily() const
{
    return m_settings.value(kKeyFontFamily, QStringLiteral("Consolas")).toString();
}

void SettingsManager::setEditorFontFamily(const QString &family)
{
    m_settings.setValue(kKeyFontFamily, family);
}

int SettingsManager::editorFontPointSize() const
{
    return m_settings.value(kKeyFontSize, 11).toInt();
}

void SettingsManager::setEditorFontPointSize(int size)
{
    m_settings.setValue(kKeyFontSize, qBound(6, size, 72));
}

QByteArray SettingsManager::windowGeometry() const
{
    return m_settings.value(kKeyWindowGeometry).toByteArray();
}

void SettingsManager::setWindowGeometry(const QByteArray &geometry)
{
    m_settings.setValue(kKeyWindowGeometry, geometry);
}

QByteArray SettingsManager::windowState() const
{
    return m_settings.value(kKeyWindowState).toByteArray();
}

void SettingsManager::setWindowState(const QByteArray &state)
{
    m_settings.setValue(kKeyWindowState, state);
}


