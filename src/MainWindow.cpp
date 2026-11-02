#include "MainWindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QStatusBar>
#include <QStyle>
#include <QTabWidget>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>
#include <QToolBar>
#include <QUrl>

#include "core/DocumentManager.h"
#include "core/DocumentStats.h"
#include "core/FileManager.h"
#include "core/ImageManager.h"
#include "core/PdfExportManager.h"
#include "core/TableManager.h"
#include "core/ThemeManager.h"
#include "dialogs/FindReplaceDialog.h"
#include "editor/EditorWidget.h"

MainWindow::MainWindow(SettingsManager *settings, QWidget *parent)
    : QMainWindow(parent)
    , m_settings(settings)
{
    setAcceptDrops(true);
    setWindowTitle(QStringLiteral("Advanced Text Editor"));
    resize(1100, 760);

    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(true);
    setCentralWidget(m_tabs);

    m_docs = new DocumentManager(m_tabs, this);

    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSaveTick);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    connect(m_tabs, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabCloseRequested);
    connect(m_docs, &DocumentManager::currentDocumentChanged,
            this, &MainWindow::onCurrentDocumentChanged);
    connect(m_docs, &DocumentManager::modificationChanged, this,
            [this](EditorWidget *editor, bool) {
                if (editor == m_docs->currentEditor()) {
                    updateWindowTitle();
                    updateStatusBar();
                }
            });

    // Restore window geometry / state.
    if (!m_settings->windowGeometry().isEmpty())
        restoreGeometry(m_settings->windowGeometry());
    if (!m_settings->windowState().isEmpty())
        restoreState(m_settings->windowState());

    // Auto-save configuration.
    m_autoSaveAction->setChecked(m_settings->autoSaveEnabled());
    toggleAutoSave(m_settings->autoSaveEnabled());

    updateRecentFilesMenu();

    // Start with one empty document.
    createEditorTab();
    onCurrentDocumentChanged(m_docs->currentEditor());
}

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------
// Action / UI construction
// ---------------------------------------------------------------------------
static QAction *makeAction(QObject *parent, const QString &text,
                           const QKeySequence &shortcut = QKeySequence())
{
    auto *a = new QAction(text, parent);
    if (!shortcut.isEmpty())
        a->setShortcut(shortcut);
    return a;
}


void MainWindow::createActions()
{
    QStyle *s = style();

    // File ---------------------------------------------------------------
    m_newAction = makeAction(this, QStringLiteral("&New"), QKeySequence::New);
    m_newAction->setIcon(s->standardIcon(QStyle::SP_FileIcon));
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newDocument);

    m_openAction = makeAction(this, QStringLiteral("&Open..."), QKeySequence::Open);
    m_openAction->setIcon(s->standardIcon(QStyle::SP_DialogOpenButton));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openDocumentDialog);

    m_saveAction = makeAction(this, QStringLiteral("&Save"), QKeySequence::Save);
    m_saveAction->setIcon(s->standardIcon(QStyle::SP_DialogSaveButton));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveDocument);

    m_saveAsAction = makeAction(this, QStringLiteral("Save &As..."), QKeySequence::SaveAs);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveDocumentAs);

    m_exportPdfAction = makeAction(this, QStringLiteral("Export to &PDF..."),
                                   QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(m_exportPdfAction, &QAction::triggered, this, &MainWindow::exportToPdf);

    m_printAction = makeAction(this, QStringLiteral("&Print..."), QKeySequence::Print);
    connect(m_printAction, &QAction::triggered, this, &MainWindow::printDocument);

    m_printPreviewAction = makeAction(this, QStringLiteral("Print Pre&view..."));
    connect(m_printPreviewAction, &QAction::triggered, this, &MainWindow::printPreview);

    m_exitAction = makeAction(this, QStringLiteral("E&xit"), QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // Edit ---------------------------------------------------------------
    m_undoAction = makeAction(this, QStringLiteral("&Undo"), QKeySequence::Undo);
    m_undoAction->setIcon(s->standardIcon(QStyle::SP_ArrowBack));
    connect(m_undoAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->undo();
    });

    m_redoAction = makeAction(this, QStringLiteral("&Redo"), QKeySequence::Redo);
    m_redoAction->setIcon(s->standardIcon(QStyle::SP_ArrowForward));
    connect(m_redoAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->redo();
    });

    m_cutAction = makeAction(this, QStringLiteral("Cu&t"), QKeySequence::Cut);
    connect(m_cutAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->cut();
    });

    m_copyAction = makeAction(this, QStringLiteral("&Copy"), QKeySequence::Copy);
    connect(m_copyAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->copy();
    });

    m_pasteAction = makeAction(this, QStringLiteral("&Paste"), QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->paste();
    });

    m_selectAllAction = makeAction(this, QStringLiteral("Select &All"),
                                   QKeySequence::SelectAll);
    connect(m_selectAllAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->selectAll();
    });

    m_findAction = makeAction(this, QStringLiteral("&Find && Replace..."),
                              QKeySequence::Find);
    connect(m_findAction, &QAction::triggered, this, &MainWindow::showFindReplace);

    // Format -------------------------------------------------------------
    m_boldAction = makeAction(this, QStringLiteral("&Bold"), QKeySequence::Bold);
    m_boldAction->setCheckable(true);
    connect(m_boldAction, &QAction::triggered, this, &MainWindow::toggleBold);

    m_italicAction = makeAction(this, QStringLiteral("&Italic"), QKeySequence::Italic);
    m_italicAction->setCheckable(true);
    connect(m_italicAction, &QAction::triggered, this, &MainWindow::toggleItalic);

    m_underlineAction = makeAction(this, QStringLiteral("&Underline"),
                                   QKeySequence::Underline);
    m_underlineAction->setCheckable(true);
    connect(m_underlineAction, &QAction::triggered, this, &MainWindow::toggleUnderline);

    m_fontAction = makeAction(this, QStringLiteral("&Font..."));
    connect(m_fontAction, &QAction::triggered, this, &MainWindow::chooseFont);

    m_colorAction = makeAction(this, QStringLiteral("Text &Color..."));
    connect(m_colorAction, &QAction::triggered, this, &MainWindow::chooseTextColor);

    m_alignLeftAction = makeAction(this, QStringLiteral("Align &Left"));
    connect(m_alignLeftAction, &QAction::triggered, this, [this] {
        ImageManager::alignCurrentParagraph(m_docs->currentEditor(),
                                             ImageManager::Align::Left);
    });
    m_alignCenterAction = makeAction(this, QStringLiteral("Align &Center"));
    connect(m_alignCenterAction, &QAction::triggered, this, [this] {
        ImageManager::alignCurrentParagraph(m_docs->currentEditor(),
                                             ImageManager::Align::Center);
    });
    m_alignRightAction = makeAction(this, QStringLiteral("Align &Right"));
    connect(m_alignRightAction, &QAction::triggered, this, [this] {
        ImageManager::alignCurrentParagraph(m_docs->currentEditor(),
                                             ImageManager::Align::Right);
    });
// Insert -------------------------------------------------------------
    m_insertTableAction = makeAction(this, QStringLiteral("Insert &Table..."));
    connect(m_insertTableAction, &QAction::triggered, this, &MainWindow::insertTable);

    m_insertImageAction = makeAction(this, QStringLiteral("Insert &Image..."));
    connect(m_insertImageAction, &QAction::triggered, this, &MainWindow::insertImage);

    m_rowAboveAction = makeAction(this, QStringLiteral("Insert Row &Above"));
    connect(m_rowAboveAction, &QAction::triggered, this, [this] {
        TableManager::insertRowAbove(m_docs->currentEditor());
    });
    m_rowBelowAction = makeAction(this, QStringLiteral("Insert Row &Below"));
    connect(m_rowBelowAction, &QAction::triggered, this, [this] {
        TableManager::insertRowBelow(m_docs->currentEditor());
    });
    m_colLeftAction = makeAction(this, QStringLiteral("Insert Column &Left"));
    connect(m_colLeftAction, &QAction::triggered, this, [this] {
        TableManager::insertColumnLeft(m_docs->currentEditor());
    });
    m_colRightAction = makeAction(this, QStringLiteral("Insert Column &Right"));
    connect(m_colRightAction, &QAction::triggered, this, [this] {
        TableManager::insertColumnRight(m_docs->currentEditor());
    });
    m_delRowAction = makeAction(this, QStringLiteral("Delete Row"));
    connect(m_delRowAction, &QAction::triggered, this, [this] {
        TableManager::removeCurrentRow(m_docs->currentEditor());
    });
    m_delColAction = makeAction(this, QStringLiteral("Delete Column"));
    connect(m_delColAction, &QAction::triggered, this, [this] {
        TableManager::removeCurrentColumn(m_docs->currentEditor());
    });

    // View ---------------------------------------------------------------
    m_lineNumbersAction = makeAction(this, QStringLiteral("Show &Line Numbers"));
    m_lineNumbersAction->setCheckable(true);
    m_lineNumbersAction->setChecked(m_settings->lineNumbers());
    connect(m_lineNumbersAction, &QAction::toggled, this, &MainWindow::toggleLineNumbers);

    m_wordWrapAction = makeAction(this, QStringLiteral("&Word Wrap"));
    m_wordWrapAction->setCheckable(true);
    m_wordWrapAction->setChecked(m_settings->wordWrap());
    connect(m_wordWrapAction, &QAction::toggled, this, &MainWindow::toggleWordWrap);

    m_zoomInAction = makeAction(this, QStringLiteral("Zoom &In"), QKeySequence::ZoomIn);
    connect(m_zoomInAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->zoomInStep();
    });
    m_zoomOutAction = makeAction(this, QStringLiteral("Zoom &Out"), QKeySequence::ZoomOut);
    connect(m_zoomOutAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->zoomOutStep();
    });
    m_zoomResetAction = makeAction(this, QStringLiteral("&Reset Zoom"),
                                   QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(m_zoomResetAction, &QAction::triggered, this, [this] {
        if (auto *e = m_docs->currentEditor()) e->resetZoom();
    });

    m_lightThemeAction = makeAction(this, QStringLiteral("&Light Theme"));
    m_lightThemeAction->setCheckable(true);
    connect(m_lightThemeAction, &QAction::triggered, this, &MainWindow::setLightTheme);
    m_darkThemeAction = makeAction(this, QStringLiteral("&Dark Theme"));
    m_darkThemeAction->setCheckable(true);
    connect(m_darkThemeAction, &QAction::triggered, this, &MainWindow::setDarkTheme);
    const bool dark = m_settings->theme() == SettingsManager::Theme::Dark;
    m_darkThemeAction->setChecked(dark);
    m_lightThemeAction->setChecked(!dark);

    // Tools --------------------------------------------------------------
    m_statsAction = makeAction(this, QStringLiteral("Document &Statistics..."));
    connect(m_statsAction, &QAction::triggered, this, &MainWindow::showStatistics);

    m_autoSaveAction = makeAction(this, QStringLiteral("&Auto-Save"));
    m_autoSaveAction->setCheckable(true);
    connect(m_autoSaveAction, &QAction::toggled, this, &MainWindow::toggleAutoSave);

    // Help ---------------------------------------------------------------
    m_aboutAction = makeAction(this, QStringLiteral("&About"));
    connect(m_aboutAction, &QAction::triggered, this, [this] {
        QMessageBox::about(this, QStringLiteral("About Advanced Text Editor"),
            QStringLiteral(
                "<h3>Advanced Text Editor 1.0</h3>"
                "<p>A Notepad++-style editor and lightweight word processor "
                "built with Qt 6 and C++17.</p>"
                "<p>Features: tabbed editing, rich text, tables, embedded "
                "images, PDF export, printing, find &amp; replace, "
                "undo/redo, themes and more.</p>"));
    });
}


void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    m_recentMenu = fileMenu->addMenu(QStringLiteral("Open &Recent"));
    for (int i = 0; i < kMaxRecentActions; ++i) {
        m_recentActions[i] = new QAction(this);
        m_recentActions[i]->setVisible(false);
        connect(m_recentActions[i], &QAction::triggered,
                this, &MainWindow::openRecentFile);
        m_recentMenu->addAction(m_recentActions[i]);
    }
    m_recentMenu->addSeparator();
    m_clearRecentAction = m_recentMenu->addAction(QStringLiteral("Clear List"));
    connect(m_clearRecentAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);

    fileMenu->addSeparator();
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exportPdfAction);
    fileMenu->addAction(m_printAction);
    fileMenu->addAction(m_printPreviewAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    QMenu *editMenu = menuBar()->addMenu(QStringLiteral("&Edit"));
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(m_selectAllAction);
    editMenu->addAction(m_findAction);

    QMenu *formatMenu = menuBar()->addMenu(QStringLiteral("F&ormat"));
    formatMenu->addAction(m_boldAction);
    formatMenu->addAction(m_italicAction);
    formatMenu->addAction(m_underlineAction);
    formatMenu->addSeparator();
    formatMenu->addAction(m_fontAction);
    formatMenu->addAction(m_colorAction);
    formatMenu->addSeparator();
    formatMenu->addAction(m_alignLeftAction);
    formatMenu->addAction(m_alignCenterAction);
    formatMenu->addAction(m_alignRightAction);

    QMenu *insertMenu = menuBar()->addMenu(QStringLiteral("&Insert"));
    insertMenu->addAction(m_insertTableAction);
    insertMenu->addAction(m_insertImageAction);
    insertMenu->addSeparator();
    insertMenu->addAction(m_rowAboveAction);
    insertMenu->addAction(m_rowBelowAction);
    insertMenu->addAction(m_colLeftAction);
    insertMenu->addAction(m_colRightAction);
    insertMenu->addSeparator();
    insertMenu->addAction(m_delRowAction);
    insertMenu->addAction(m_delColAction);

    QMenu *viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    viewMenu->addAction(m_lineNumbersAction);
    viewMenu->addAction(m_wordWrapAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_zoomInAction);
    viewMenu->addAction(m_zoomOutAction);
    viewMenu->addAction(m_zoomResetAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_lightThemeAction);
    viewMenu->addAction(m_darkThemeAction);

    QMenu *toolsMenu = menuBar()->addMenu(QStringLiteral("&Tools"));
    toolsMenu->addAction(m_statsAction);
    toolsMenu->addAction(m_autoSaveAction);

    QMenu *helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBar()
{
    QToolBar *tb = addToolBar(QStringLiteral("Main"));
    tb->setObjectName(QStringLiteral("MainToolBar"));
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);

    tb->addAction(m_newAction);
    tb->addAction(m_openAction);
    tb->addAction(m_saveAction);
    tb->addSeparator();
    tb->addAction(m_undoAction);
    tb->addAction(m_redoAction);
    tb->addSeparator();
    tb->addAction(m_cutAction);
    tb->addAction(m_copyAction);
    tb->addAction(m_pasteAction);
    tb->addSeparator();
    tb->addAction(m_boldAction);
    tb->addAction(m_italicAction);
    tb->addAction(m_underlineAction);
    tb->addSeparator();
    tb->addAction(m_findAction);
    tb->addAction(m_insertTableAction);
    tb->addAction(m_insertImageAction);
    tb->addSeparator();
    tb->addAction(m_zoomInAction);
    tb->addAction(m_zoomOutAction);
}


void MainWindow::createStatusBar()
{
    m_cursorLabel   = new QLabel(QStringLiteral("Ln 1, Col 1"), this);
    m_statsLabel    = new QLabel(QStringLiteral("Words: 0  Chars: 0"), this);
    m_zoomLabel     = new QLabel(QStringLiteral("Zoom: 100%"), this);
    m_modifiedLabel = new QLabel(this);

    statusBar()->addWidget(m_cursorLabel);
    statusBar()->addWidget(m_statsLabel, 1);
    statusBar()->addPermanentWidget(m_modifiedLabel);
    statusBar()->addPermanentWidget(m_zoomLabel);
}

// ---------------------------------------------------------------------------
// Tab / editor lifecycle
// ---------------------------------------------------------------------------
EditorWidget *MainWindow::createEditorTab()
{
    EditorWidget *editor = m_docs->createDocument();
    applyEditorPreferences(editor);

    connect(editor, &QTextEdit::cursorPositionChanged, this, [this] {
        updateStatusBar();
        updateEditActions();
    });
    connect(editor, &QTextEdit::selectionChanged, this, &MainWindow::updateEditActions);
    connect(editor, &QTextEdit::textChanged, this, &MainWindow::updateStatusBar);
    connect(editor, &EditorWidget::zoomLevelChanged, this, [this](int) {
        updateStatusBar();
    });
    connect(editor->document(), &QTextDocument::undoAvailable,
            this, &MainWindow::updateEditActions);
    connect(editor->document(), &QTextDocument::redoAvailable,
            this, &MainWindow::updateEditActions);
    connect(editor, &QTextEdit::currentCharFormatChanged, this,
            [this](const QTextCharFormat &fmt) {
                m_boldAction->setChecked(fmt.fontWeight() >= QFont::Bold);
                m_italicAction->setChecked(fmt.fontItalic());
                m_underlineAction->setChecked(fmt.fontUnderline());
            });

    return editor;
}

void MainWindow::applyEditorPreferences(EditorWidget *editor)
{
    QFont font(m_settings->editorFontFamily(), m_settings->editorFontPointSize());
    font.setStyleHint(QFont::Monospace);
    editor->setEditorFont(font);
    editor->setLineNumbersVisible(m_settings->lineNumbers());
    editor->setWordWrapEnabled(m_settings->wordWrap());
}

EditorWidget *MainWindow::requireEditor()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        editor = createEditorTab();
    return editor;
}

// ---------------------------------------------------------------------------
// File operations
// ---------------------------------------------------------------------------
void MainWindow::newDocument()
{
    createEditorTab();
    onCurrentDocumentChanged(m_docs->currentEditor());
}

void MainWindow::openDocumentDialog()
{
    const QStringList paths = QFileDialog::getOpenFileNames(
        this, QStringLiteral("Open Files"), QString(), FileManager::openFilter());
    for (const QString &path : paths)
        openFile(path);
}

void MainWindow::openFile(const QString &path)
{
    if (path.isEmpty())
        return;

    // Already open? Just focus it.
    if (EditorWidget *existing = m_docs->findByPath(path)) {
        m_tabs->setCurrentWidget(existing);
        return;
    }

    EditorWidget *editor = m_docs->currentEditor();
    const bool reuseCurrent = editor
        && m_docs->filePath(editor).isEmpty()
        && !m_docs->isModified(editor)
        && editor->document()->isEmpty();
    if (!reuseCurrent)
        editor = createEditorTab();

    if (!loadIntoEditor(editor, path)) {
        if (!reuseCurrent)
            m_docs->closeTab(m_tabs->indexOf(editor));
        return;
    }

    m_settings->addRecentFile(path);
    updateRecentFilesMenu();
    onCurrentDocumentChanged(editor);
}
bool MainWindow::loadIntoEditor(EditorWidget *editor, const QString &path)
{
    QString content;
    QString error;
    if (!FileManager::read(path, content, error)) {
        QMessageBox::warning(this, QStringLiteral("Open Failed"), error);
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (FileManager::formatForPath(path) == FileManager::Format::Html)
        editor->setHtml(content);
    else
        editor->setPlainText(content);
    QApplication::restoreOverrideCursor();

    editor->document()->setModified(false);
    m_docs->setFilePath(editor, path);
    m_docs->setModified(editor, false);
    m_tabs->setCurrentWidget(editor);
    return true;
}

bool MainWindow::saveDocument()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return false;

    const QString path = m_docs->filePath(editor);
    if (path.isEmpty())
        return saveDocumentAs();
    return saveToPath(editor, path);
}

bool MainWindow::saveDocumentAs()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return false;

    QString suggested = m_docs->filePath(editor);
    if (suggested.isEmpty())
        suggested = m_docs->displayName(editor) + QStringLiteral(".txt");

    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Save As"), suggested, FileManager::saveFilter());
    if (path.isEmpty())
        return false;

    return saveToPath(editor, path);
}

bool MainWindow::saveToPath(EditorWidget *editor, const QString &path)
{
    const FileManager::Format fmt = FileManager::formatForPath(path);
    const QString content = (fmt == FileManager::Format::Html)
                                ? editor->document()->toHtml()
                                : editor->toPlainText();

    QString error;
    if (!FileManager::write(path, content, error)) {
        QMessageBox::warning(this, QStringLiteral("Save Failed"), error);
        return false;
    }

    m_docs->setFilePath(editor, path);
    editor->document()->setModified(false);
    m_docs->setModified(editor, false);
    m_settings->addRecentFile(path);
    updateRecentFilesMenu();
    updateWindowTitle();
    statusBar()->showMessage(QStringLiteral("Saved %1").arg(path), 3000);
    return true;
}

void MainWindow::exportToPdf()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;

    QString suggested = m_docs->filePath(editor);
    if (suggested.isEmpty())
        suggested = m_docs->displayName(editor);
    suggested = QFileInfo(suggested).completeBaseName() + QStringLiteral(".pdf");

    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Export to PDF"), suggested,
        QStringLiteral("PDF documents (*.pdf)"));
    if (path.isEmpty())
        return;

    // Simple page setup prompt: orientation.
    const QStringList orientations{QStringLiteral("Portrait"),
                                   QStringLiteral("Landscape")};
    bool ok = false;
    const QString choice = QInputDialog::getItem(
        this, QStringLiteral("Page Setup"), QStringLiteral("Orientation:"),
        orientations, 0, false, &ok);
    if (!ok)
        return;

    PdfExportManager::PageSetup setup;
    setup.orient = (choice == QStringLiteral("Landscape"))
                       ? QPageLayout::Landscape
                       : QPageLayout::Portrait;

    QString error;
    if (!PdfExportManager::exportToPdf(editor->document(), path, setup, error))
        QMessageBox::warning(this, QStringLiteral("PDF Export Failed"), error);
    else
        statusBar()->showMessage(QStringLiteral("Exported %1").arg(path), 3000);
}

void MainWindow::printDocument()
{
    if (EditorWidget *editor = m_docs->currentEditor())
        PdfExportManager::printDocument(editor->document(), this);
}

void MainWindow::printPreview()
{
    if (EditorWidget *editor = m_docs->currentEditor())
        PdfExportManager::printPreview(editor->document(), this);
}

// ---------------------------------------------------------------------------
// Recent files
// ---------------------------------------------------------------------------
void MainWindow::updateRecentFilesMenu()
{
    const QStringList files = m_settings->recentFiles();
    const int shown = qMin(files.size(), static_cast<int>(kMaxRecentActions));
    for (int i = 0; i < shown; ++i) {
        const QString text = QStringLiteral("&%1  %2")
                                 .arg(i + 1)
                                 .arg(QFileInfo(files.at(i)).fileName());
        m_recentActions[i]->setText(text);
        m_recentActions[i]->setData(files.at(i));
        m_recentActions[i]->setToolTip(files.at(i));
        m_recentActions[i]->setVisible(true);
    }
    for (int i = shown; i < kMaxRecentActions; ++i)
        m_recentActions[i]->setVisible(false);

    if (m_recentMenu)
        m_recentMenu->setEnabled(shown > 0);
}

void MainWindow::openRecentFile()
{
    if (auto *action = qobject_cast<QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::clearRecentFiles()
{
    m_settings->clearRecentFiles();
    updateRecentFilesMenu();
}

// ---------------------------------------------------------------------------
// Find & Replace
// ---------------------------------------------------------------------------
void MainWindow::showFindReplace()
{
    if (!m_findDialog) {
        m_findDialog = new FindReplaceDialog(this);
        connect(m_findDialog, &FindReplaceDialog::findRequested,
                this, &MainWindow::performFind);
        connect(m_findDialog, &FindReplaceDialog::replaceRequested,
                this, &MainWindow::performReplace);
        connect(m_findDialog, &FindReplaceDialog::replaceAllRequested,
                this, &MainWindow::performReplaceAll);
    }

    if (EditorWidget *editor = m_docs->currentEditor()) {
        const QString selected = editor->textCursor().selectedText();
        m_findDialog->setFindText(selected);
    }
    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
    m_findDialog->focusFindField();
}

void MainWindow::performFind()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor || !m_findDialog)
        return;

    const QString text = m_findDialog->findText();
    if (text.isEmpty())
        return;

    const QTextDocument::FindFlags flags = m_findDialog->flags();
    if (editor->find(text, flags))
        return;

    if (m_findDialog->wrapAround()) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(flags.testFlag(QTextDocument::FindBackward)
                                ? QTextCursor::End
                                : QTextCursor::Start);
        editor->setTextCursor(cursor);
        if (editor->find(text, flags))
            return;
    }
    statusBar()->showMessage(QStringLiteral("Not found: %1").arg(text), 3000);
}

void MainWindow::performReplace()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor || !m_findDialog)
        return;

    QTextCursor cursor = editor->textCursor();
    const Qt::CaseSensitivity cs =
        m_findDialog->flags().testFlag(QTextDocument::FindCaseSensitively)
            ? Qt::CaseSensitive : Qt::CaseInsensitive;

    if (cursor.hasSelection()
        && QString::compare(cursor.selectedText(), m_findDialog->findText(), cs) == 0) {
        cursor.insertText(m_findDialog->replaceText());
    }
    performFind(); // advance to the next match
}

void MainWindow::performReplaceAll()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor || !m_findDialog)
        return;

    const QString findText = m_findDialog->findText();
    if (findText.isEmpty())
        return;
    const QString replaceText = m_findDialog->replaceText();
    QTextDocument::FindFlags flags = m_findDialog->flags();
    flags.setFlag(QTextDocument::FindBackward, false); // always sweep forward

    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();                 // single undo step for the whole op
    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);

    int count = 0;
    while (editor->find(findText, flags)) {
        QTextCursor found = editor->textCursor();
        found.insertText(replaceText);
        ++count;
    }
    cursor.endEditBlock();

    statusBar()->showMessage(QStringLiteral("Replaced %1 occurrence(s)").arg(count),
                             3000);
}

// ---------------------------------------------------------------------------
// Formatting
// ---------------------------------------------------------------------------
void MainWindow::toggleBold()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;
    QTextCharFormat fmt;
    fmt.setFontWeight(m_boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::toggleItalic()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;
    QTextCharFormat fmt;
    fmt.setFontItalic(m_italicAction->isChecked());
    editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::toggleUnderline()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;
    QTextCharFormat fmt;
    fmt.setFontUnderline(m_underlineAction->isChecked());
    editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::chooseFont()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, editor->currentFont(), this);
    if (!ok)
        return;
    QTextCharFormat fmt;
    fmt.setFont(font);
    editor->mergeCurrentCharFormat(fmt);
}

void MainWindow::chooseTextColor()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;
    const QColor color = QColorDialog::getColor(editor->textColor(), this,
                                                QStringLiteral("Text Color"));
    if (!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    editor->mergeCurrentCharFormat(fmt);
}

// ---------------------------------------------------------------------------
// Insert
// ---------------------------------------------------------------------------
void MainWindow::insertTable()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;

    bool ok = false;
    const int rows = QInputDialog::getInt(
        this, QStringLiteral("Insert Table"), QStringLiteral("Rows:"),
        2, 1, 200, 1, &ok);
    if (!ok)
        return;
    const int cols = QInputDialog::getInt(
        this, QStringLiteral("Insert Table"), QStringLiteral("Columns:"),
        2, 1, 50, 1, &ok);
    if (!ok)
        return;

    TableManager::insertTable(editor, rows, cols);
}

void MainWindow::insertImage()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;

    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Insert Image"), QString(),
        ImageManager::imageFilter());
    if (path.isEmpty())
        return;

    bool ok = false;
    const int width = QInputDialog::getInt(
        this, QStringLiteral("Image Width"),
        QStringLiteral("Width in pixels (0 = original):"),
        0, 0, ImageManager::kMaxWidth, 10, &ok);
    if (!ok)
        return;

    QString error;
    if (!ImageManager::insertImageFromFile(editor, path, width, error))
        QMessageBox::warning(this, QStringLiteral("Insert Image Failed"), error);
}

// ---------------------------------------------------------------------------
// View / theme
// ---------------------------------------------------------------------------
void MainWindow::toggleLineNumbers(bool enabled)
{
    m_settings->setLineNumbers(enabled);
    for (int i = 0; i < m_docs->count(); ++i)
        if (EditorWidget *e = m_docs->editorAt(i))
            e->setLineNumbersVisible(enabled);
}

void MainWindow::toggleWordWrap(bool enabled)
{
    m_settings->setWordWrap(enabled);
    for (int i = 0; i < m_docs->count(); ++i)
        if (EditorWidget *e = m_docs->editorAt(i))
            e->setWordWrapEnabled(enabled);
}
void MainWindow::setLightTheme()
{
    m_settings->setTheme(SettingsManager::Theme::Light);
    ThemeManager::applyTheme(*qApp, SettingsManager::Theme::Light);
    m_lightThemeAction->setChecked(true);
    m_darkThemeAction->setChecked(false);
}

void MainWindow::setDarkTheme()
{
    m_settings->setTheme(SettingsManager::Theme::Dark);
    ThemeManager::applyTheme(*qApp, SettingsManager::Theme::Dark);
    m_darkThemeAction->setChecked(true);
    m_lightThemeAction->setChecked(false);
}

// ---------------------------------------------------------------------------
// Tools
// ---------------------------------------------------------------------------
void MainWindow::showStatistics()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor)
        return;

    const DocumentStats stats = DocumentStats::fromText(editor->toPlainText());
    QMessageBox::information(
        this, QStringLiteral("Document Statistics"),
        QStringLiteral("Words: %1\nCharacters (with spaces): %2\n"
                       "Characters (no spaces): %3\nLines: %4")
            .arg(stats.words)
            .arg(stats.characters)
            .arg(stats.charactersNoSpaces)
            .arg(stats.lines));
}

void MainWindow::toggleAutoSave(bool enabled)
{
    m_settings->setAutoSaveEnabled(enabled);
    if (enabled)
        m_autoSaveTimer->start(m_settings->autoSaveIntervalSeconds() * 1000);
    else
        m_autoSaveTimer->stop();
}

void MainWindow::autoSaveTick()
{
    for (int i = 0; i < m_docs->count(); ++i) {
        EditorWidget *editor = m_docs->editorAt(i);
        if (!editor)
            continue;
        const QString path = m_docs->filePath(editor);
        if (!path.isEmpty() && m_docs->isModified(editor))
            saveToPath(editor, path);
    }
}

// ---------------------------------------------------------------------------
// Status bar / titles / action state
// ---------------------------------------------------------------------------
void MainWindow::onCurrentDocumentChanged(EditorWidget *editor)
{
    Q_UNUSED(editor)
    updateStatusBar();
    updateWindowTitle();
    updateEditActions();
}

void MainWindow::onTabCloseRequested(int index)
{
    EditorWidget *editor = m_docs->editorAt(index);
    if (!editor)
        return;
    if (!maybeSave(editor))
        return;
    m_docs->closeTab(index);
    if (m_docs->count() == 0)
        createEditorTab();
}

void MainWindow::updateStatusBar()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor) {
        m_cursorLabel->setText(QStringLiteral("Ln 1, Col 1"));
        m_statsLabel->setText(QStringLiteral("Words: 0  Chars: 0"));
        m_zoomLabel->setText(QStringLiteral("Zoom: 100%"));
        m_modifiedLabel->clear();
        return;
    }

    const QTextCursor cursor = editor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int col  = cursor.columnNumber() + 1;
    m_cursorLabel->setText(QStringLiteral("Ln %1, Col %2").arg(line).arg(col));

    const DocumentStats stats = DocumentStats::fromText(editor->toPlainText());
    m_statsLabel->setText(QStringLiteral("Words: %1  Chars: %2")
                              .arg(stats.words).arg(stats.characters));

    const int percent = 100 + editor->zoomLevel() * 10;
    m_zoomLabel->setText(QStringLiteral("Zoom: %1%").arg(percent));

    m_modifiedLabel->setText(m_docs->isModified(editor)
                                 ? QStringLiteral("Modified")
                                 : QStringLiteral("Saved"));
}

void MainWindow::updateWindowTitle()
{
    EditorWidget *editor = m_docs->currentEditor();
    if (!editor) {
        setWindowTitle(QStringLiteral("Advanced Text Editor"));
        return;
    }
    const QString name = m_docs->displayName(editor);
    const QString star = m_docs->isModified(editor) ? QStringLiteral("*") : QString();
    setWindowTitle(QStringLiteral("%1%2 - Advanced Text Editor").arg(star, name));
}

void MainWindow::updateEditActions()
{
    EditorWidget *editor = m_docs->currentEditor();
    const bool has = editor != nullptr;

    m_undoAction->setEnabled(has && editor->document()->isUndoAvailable());
    m_redoAction->setEnabled(has && editor->document()->isRedoAvailable());

    const bool hasSelection = has && editor->textCursor().hasSelection();
    m_cutAction->setEnabled(hasSelection);
    m_copyAction->setEnabled(hasSelection);
}

// ---------------------------------------------------------------------------
// Close / unsaved-changes handling
// ---------------------------------------------------------------------------
bool MainWindow::maybeSave(EditorWidget *editor)
{
    if (!editor || !m_docs->isModified(editor))
        return true;

    m_tabs->setCurrentWidget(editor);
    const QString name = m_docs->displayName(editor);
    const auto choice = QMessageBox::warning(
        this, QStringLiteral("Unsaved Changes"),
        QStringLiteral("'%1' has unsaved changes.\nDo you want to save them?")
            .arg(name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (choice) {
    case QMessageBox::Save:
        return saveDocument();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < m_docs->count(); ++i) {
        if (!maybeSave(m_docs->editorAt(i))) {
            event->ignore();
            return;
        }
    }

    m_settings->setWindowGeometry(saveGeometry());
    m_settings->setWindowState(saveState());
    event->accept();
}

// ---------------------------------------------------------------------------
// Drag & drop
// ---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls) {
        if (url.isLocalFile())
            openFile(url.toLocalFile());
    }
    event->acceptProposedAction();
}


