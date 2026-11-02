#include "core/FileManager.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextStream>

FileManager::Format FileManager::formatForPath(const QString &path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == QLatin1String("html") || suffix == QLatin1String("htm"))
        return Format::Html;
    return Format::PlainText;
}

bool FileManager::read(const QString &path, QString &outContent, QString &errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = QStringLiteral("Cannot open '%1' for reading: %2")
                           .arg(path, file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream.setAutoDetectUnicode(true); // honour a BOM if present
    outContent = stream.readAll();

    if (file.error() != QFileDevice::NoError) {
        errorMessage = QStringLiteral("Error while reading '%1': %2")
                           .arg(path, file.errorString());
        return false;
    }
    return true;
}

bool FileManager::write(const QString &path, const QString &content, QString &errorMessage)
{
    // QSaveFile writes to a temp file and atomically renames on commit,
    // so a crash mid-write never corrupts the existing document.
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorMessage = QStringLiteral("Cannot open '%1' for writing: %2")
                           .arg(path, file.errorString());
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << content;
    stream.flush();

    if (!file.commit()) {
        errorMessage = QStringLiteral("Error while writing '%1': %2")
                           .arg(path, file.errorString());
        return false;
    }
    return true;
}

QString FileManager::untitledName(int index)
{
    return index <= 1 ? QStringLiteral("Untitled")
                      : QStringLiteral("Untitled %1").arg(index);
}

QString FileManager::openFilter()
{
    return QStringLiteral(
        "All supported (*.txt *.md *.log *.ini *.json *.xml *.csv *.cpp *.h "
        "*.hpp *.c *.cc *.py *.js *.ts *.java *.cs *.html *.htm);;"
        "Text files (*.txt);;"
        "Rich text / HTML (*.html *.htm);;"
        "All files (*.*)");
}

QString FileManager::saveFilter()
{
    return QStringLiteral(
        "Text files (*.txt);;"
        "Rich text / HTML (*.html *.htm);;"
        "All files (*.*)");
}


