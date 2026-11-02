#ifndef ATE_PDFEXPORTMANAGER_H
#define ATE_PDFEXPORTMANAGER_H

#include <QMarginsF>
#include <QPageLayout>
#include <QPageSize>
#include <QString>

class QTextDocument;
class QWidget;

/**
 * @brief Exports and prints rich-text documents.
 *
 * PDF export uses QPrinter's native PDF backend, which renders the whole
 * QTextDocument (text formatting, tables and embedded images are all
 * preserved). Page size, orientation and margins are configurable.
 */
class PdfExportManager
{
public:
    struct PageSetup {
        QPageSize::PageSizeId pageSize   = QPageSize::A4;
        QPageLayout::Orientation orient  = QPageLayout::Portrait;
        QMarginsF marginsMm              = QMarginsF(15, 15, 15, 15);
    };

    /// Render @p document to a PDF file. Returns false (errorMessage set) on failure.
    static bool exportToPdf(QTextDocument *document, const QString &path,
                            const PageSetup &setup, QString &errorMessage);

    /// Show the system print dialog and print @p document. Returns false if cancelled.
    static bool printDocument(QTextDocument *document, QWidget *parent);

    /// Show a print-preview dialog for @p document.
    static void printPreview(QTextDocument *document, QWidget *parent);
};

#endif // ATE_PDFEXPORTMANAGER_H


