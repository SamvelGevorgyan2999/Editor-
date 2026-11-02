#include <QtTest>

#include "core/DocumentStats.h"

class TestDocumentStats : public QObject
{
    Q_OBJECT

private slots:
    void emptyText();
    void singleWord();
    void multipleWordsAndLines();
    void whitespaceHandling();
    void unicodeText();
};

void TestDocumentStats::emptyText()
{
    const DocumentStats s = DocumentStats::fromText(QString());
    QCOMPARE(s.characters, 0);
    QCOMPARE(s.charactersNoSpaces, 0);
    QCOMPARE(s.words, 0);
    QCOMPARE(s.lines, 0);
}

void TestDocumentStats::singleWord()
{
    const DocumentStats s = DocumentStats::fromText(QStringLiteral("hello"));
    QCOMPARE(s.characters, 5);
    QCOMPARE(s.charactersNoSpaces, 5);
    QCOMPARE(s.words, 1);
    QCOMPARE(s.lines, 1);
}

void TestDocumentStats::multipleWordsAndLines()
{
    const DocumentStats s =
        DocumentStats::fromText(QStringLiteral("the quick brown\nfox jumps"));
    QCOMPARE(s.words, 5);
    QCOMPARE(s.lines, 2);
    QCOMPARE(s.characters, 25);
}

void TestDocumentStats::whitespaceHandling()
{
    const DocumentStats s =
        DocumentStats::fromText(QStringLiteral("  spaced   out  words  "));
    QCOMPARE(s.words, 3);
    QVERIFY(s.charactersNoSpaces < s.characters);
    QCOMPARE(s.charactersNoSpaces, 14); // "spacedoutwords"
}

void TestDocumentStats::unicodeText()
{
    const DocumentStats s =
        DocumentStats::fromText(QStringLiteral("caf\u00e9 na\u00efve \u00fcber"));
    QCOMPARE(s.words, 3);
    QCOMPARE(s.lines, 1);
}

QTEST_APPLESS_MAIN(TestDocumentStats)
#include "test_documentstats.moc"


