#include "core/DocumentStats.h"

DocumentStats DocumentStats::fromText(const QString &text)
{
    DocumentStats stats;
    stats.characters = static_cast<int>(text.size());

    bool inWord = false;
    for (const QChar ch : text) {
        if (!ch.isSpace()) {
            ++stats.charactersNoSpaces;
            if (!inWord) {
                ++stats.words;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }

    if (text.isEmpty()) {
        stats.lines = 0;
    } else {
        // Lines = number of newline separators + 1.
        stats.lines = static_cast<int>(text.count(QLatin1Char('\n'))) + 1;
    }

    return stats;
}


