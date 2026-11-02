#include <QtTest>
#include <QTemporaryDir>

#include "core/FileManager.h"

class TestFileManager : public QObject
{
    Q_OBJECT

private slots:
    void formatDetection();
    void roundTripUtf8();
    void readMissingFile();

private:
    QTemporaryDir m_dir;
};

void TestFileManager::formatDetection()
{
    QCOMPARE(FileManager::formatForPath("a.txt"),  FileManager::Format::PlainText);
    QCOMPARE(FileManager::formatForPath("a.cpp"),  FileManager::Format::PlainText);
    QCOMPARE(FileManager::formatForPath("a.html"), FileManager::Format::Html);
    QCOMPARE(FileManager::formatForPath("a.HTM"),  FileManager::Format::Html);
}

void TestFileManager::roundTripUtf8()
{
    QVERIFY(m_dir.isValid());
    const QString path = m_dir.filePath("sample.txt");
    const QString content =
        QStringLiteral("Hello, \u4e16\u754c!\nLine 2 with caf\u00e9.\n");

    QString error;
    QVERIFY2(FileManager::write(path, content, error), qPrintable(error));

    QString readBack;
    QVERIFY2(FileManager::read(path, readBack, error), qPrintable(error));
    QCOMPARE(readBack, content);
}

void TestFileManager::readMissingFile()
{
    QString content;
    QString error;
    const bool ok =
        FileManager::read(m_dir.filePath("does_not_exist.txt"), content, error);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(TestFileManager)
#include "test_filemanager.moc"


