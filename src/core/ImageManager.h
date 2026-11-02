#ifndef ATE_IMAGEMANAGER_H
#define ATE_IMAGEMANAGER_H

#include <QString>

class QTextEdit;

/**
 * @brief Insert and align images inside a rich-text document.
 *
 * Images are embedded directly into the document as base64 data URIs. This
 * keeps the document self-contained, so saving to HTML or exporting to PDF
 * preserves the pictures without any external file dependencies.
 *
 * Supported input formats: PNG, JPG/JPEG, BMP (via Qt's image plugins).
 */
class ImageManager
{
public:
    /**
     * Insert an image at the cursor.
     * @param desiredWidth  Target width in pixels; 0 keeps the natural size.
     *                      Oversized images are always capped to kMaxWidth.
     * @return false (with errorMessage set) if the file cannot be loaded.
     */
    static bool insertImageFromFile(QTextEdit *editor, const QString &path,
                                    int desiredWidth, QString &errorMessage);

    /// Set the alignment of the paragraph containing the cursor.
    enum class Align { Left, Center, Right };
    static void alignCurrentParagraph(QTextEdit *editor, Align align);

    /// Dialog filter for the insert-image file chooser.
    static QString imageFilter();

    static constexpr int kMaxWidth = 1600;
};

#endif // ATE_IMAGEMANAGER_H


