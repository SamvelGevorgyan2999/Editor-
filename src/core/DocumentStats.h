#ifndef ATE_DOCUMENTSTATS_H
#define ATE_DOCUMENTSTATS_H

#include <QString>

struct DocumentStats
{
    int words = 0;
    int characters = 0;
    int charactersNoSpaces = 0;
    int lines = 0;

    static DocumentStats fromText(const QString &text);
};

#endif // ATE_DOCUMENTSTATS_H
