// Advanced Desktop Text Editor (Notepad++-style)
// Entry point: sets up the QApplication, applies persisted settings/theme,
// then shows the main window.

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>

#include "MainWindow.h"
#include "core/SettingsManager.h"
#include "core/ThemeManager.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_LINUX)
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
        qputenv("QT_QPA_PLATFORMTHEME", "generic");
#endif

    QApplication app(argc, argv);

    // Identity used by QSettings (persisted preferences, recent files, ...).
    QApplication::setOrganizationName("ATE");
    QApplication::setOrganizationDomain("ate.example.org");
    QApplication::setApplicationName("AdvancedTextEditor");
    QApplication::setApplicationVersion("1.0.0");

    // Command-line: optional list of files to open on startup.
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Advanced Desktop Text Editor (Notepad++-style) built with Qt 6.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", "Files to open.", "[files...]");
    parser.process(app);

    SettingsManager settings;
    ThemeManager::applyTheme(app, settings.theme());

    MainWindow window(&settings);

    // Open any files passed on the command line.
    const QStringList args = parser.positionalArguments();
    for (const QString &path : args) {
        const QFileInfo info(path);
        if (info.exists())
            window.openFile(info.absoluteFilePath());
    }

    window.show();
    return app.exec();
}

