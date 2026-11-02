#include "core/PdfExportManager.h"

#include <QFileInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextDocument>

bool PdfExportManager::exportToPdf(QTextDocument *document, const QString &path,
                                   const PageSetup &setup, QString &errorMessage)
{
    if (!document) {
        errorMessage = QStringLiteral("No document to export.");
        return false;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);

    QPageLayout layout(QPageSize(setup.pageSize), setup.orient, setup.marginsMm,
                       QPageLayout::Millimeter);
    if (!printer.setPageLayout(layout)) {
        errorMessage = QStringLiteral("Unsupported page layout for PDF export.");
        return false;
    }

    // Clone so the live editor document is never mutated by print pagination.
    QTextDocument *doc = document->clone();
    doc->setParent(nullptr);
    doc->print(&printer);
    delete doc;

    if (!QFileInfo::exists(path)) {
        errorMessage = QStringLiteral("PDF file was not created at '%1'.").arg(path);
        return false;
    }
    return true;
}

bool PdfExportManager::printDocument(QTextDocument *document, QWidget *parent)
{
    if (!document)
        return false;

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, parent);
    dialog.setWindowTitle(QStringLiteral("Print Document"));
    if (dialog.exec() != QDialog::Accepted)
        return false;

    document->print(&printer);
    return true;
}

void PdfExportManager::printPreview(QTextDocument *document, QWidget *parent)
{
    if (!document)
        return;

    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, parent);
    QObject::connect(&preview, &QPrintPreviewDialog::paintRequested,
                     document, [document](QPrinter *p) { document->print(p); });
    preview.exec();
}


