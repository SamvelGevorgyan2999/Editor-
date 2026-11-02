#ifndef ATE_FILEMANAGER_H
#define ATE_FILEMANAGER_H

#include <QString>

/**
 * @brief File-system I/O with UTF-8 encoding and clear error reporting.
 *
 * The document model distinguishes two on-disk representations:
 *   - Plain text  (.txt, source files, ...): raw UTF-8 characters.
 *   - Rich text   (.html, .htm): Qt-generated HTML that preserves
 *     formatting, tables and (base64-embedded) images.
 *
 * All methods are static and throw no exceptions; failures are reported
 * through the boolean return value and the out-parameter @p errorMessage.
 */
class FileManager
{
public:
    enum class Format { PlainText, Html };

    /// Returns the storage format implied by a file's extension.
    static Format formatForPath(const QString &path);

    /// Reads a whole file into @p outContent as UTF-8. Returns false on error.
    static bool read(const QString &path, QString &outContent, QString &errorMessage);

    /// Writes @p content to @p path as UTF-8. Returns false on error.
    static bool write(const QString &path, const QString &content, QString &errorMessage);

    /// Human-readable default name for an untitled document.
    static QString untitledName(int index);

    /// Comma/space separated dialog filter for openable files.
    static QString openFilter();

    /// Dialog filter for saving.
    static QString saveFilter();
};

#endif // ATE_FILEMANAGER_H


