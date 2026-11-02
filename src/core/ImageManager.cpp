#include "core/ImageManager.h"

#include <QBuffer>
#include <QByteArray>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QTextCursor>
#include <QTextEdit>

bool ImageManager::insertImageFromFile(QTextEdit *editor, const QString &path,
                                       int desiredWidth, QString &errorMessage)
{
    if (!editor) {
        errorMessage = QStringLiteral("No active editor.");
        return false;
    }

    QImageReader reader(path);
    reader.setAutoTransform(true); // respect EXIF orientation
    QImage image = reader.read();
    if (image.isNull()) {
        errorMessage = QStringLiteral("Cannot load image '%1': %2")
                           .arg(path, reader.errorString());
        return false;
    }

    // Determine the render width (cap to keep documents a sane size).
    int targetWidth = desiredWidth > 0 ? desiredWidth : image.width();
    targetWidth = qMin(targetWidth, kMaxWidth);
    if (targetWidth < image.width())
        image = image.scaledToWidth(targetWidth, Qt::SmoothTransformation);

    // Encode as PNG and embed as a base64 data URI so the document is
    // self-contained (survives HTML save and PDF export).
    QByteArray png;
    QBuffer buffer(&png);
    buffer.open(QIODevice::WriteOnly);
    if (!image.save(&buffer, "PNG")) {
        errorMessage = QStringLiteral("Failed to encode image.");
        return false;
    }
    const QString base64 = QString::fromLatin1(png.toBase64());

    const QString html = QStringLiteral(
        "<img src=\"data:image/png;base64,%1\" width=\"%2\" height=\"%3\" "
        "alt=\"%4\"/>")
        .arg(base64)
        .arg(image.width())
        .arg(image.height())
        .arg(QFileInfo(path).fileName().toHtmlEscaped());

    QTextCursor cursor = editor->textCursor();
    cursor.insertHtml(html);
    editor->setFocus();
    return true;
}

void ImageManager::alignCurrentParagraph(QTextEdit *editor, Align align)
{
    if (!editor)
        return;

    Qt::Alignment flag = Qt::AlignLeft;
    switch (align) {
    case Align::Left:   flag = Qt::AlignLeft;    break;
    case Align::Center: flag = Qt::AlignHCenter; break;
    case Align::Right:  flag = Qt::AlignRight;   break;
    }
    editor->setAlignment(flag);
    editor->setFocus();
}

QString ImageManager::imageFilter()
{
    return QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp);;All files (*.*)");
}


