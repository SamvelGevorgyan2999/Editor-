#ifndef ATE_MAINWINDOW_H
#define ATE_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "core/SettingsManager.h"

class QTabWidget;
class QLabel;
class QMenu;
class QTimer;
class QAction;

class DocumentManager;
class EditorWidget;
class FindReplaceDialog;

/**
 * @brief Top-level application window.
 *
 * Builds the menu bar, toolbar and status bar, hosts the tabbed document
 * area, and orchestrates the feature managers (file, table, image, PDF).
 * Also handles auto-save, drag-and-drop, recent files and the close-with-
 * unsaved-changes workflow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SettingsManager *settings, QWidget *parent = nullptr);
    ~MainWindow() override;

    /// Open @p path in a new tab (or focus an existing tab for that file).
    void openFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    // File
    void newDocument();
    void openDocumentDialog();
    bool saveDocument();
    bool saveDocumentAs();
    void exportToPdf();
    void printDocument();
    void printPreview();
    void openRecentFile();
    void clearRecentFiles();

    // Edit / format
    void showFindReplace();
    void performFind();
    void performReplace();
    void performReplaceAll();
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void chooseFont();
    void chooseTextColor();

    // Insert
    void insertTable();
    void insertImage();

    // View
    void toggleLineNumbers(bool enabled);
    void toggleWordWrap(bool enabled);
    void setLightTheme();
    void setDarkTheme();

    // Tools
    void showStatistics();
    void toggleAutoSave(bool enabled);
    void autoSaveTick();

    // Housekeeping
    void onCurrentDocumentChanged(EditorWidget *editor);
    void onTabCloseRequested(int index);
    void updateStatusBar();
    void updateWindowTitle();
    void updateEditActions();

private:
    void createActions();      ///< Build all QActions (stored as members).
    void createMenus();        ///< Assemble the menu bar from the actions.
    void createToolBar();      ///< Assemble the main toolbar from the actions.
    void createStatusBar();
    void updateRecentFilesMenu();
    EditorWidget *createEditorTab(); ///< New tab + wires per-editor signals.

    bool maybeSave(EditorWidget *editor);
    bool saveToPath(EditorWidget *editor, const QString &path);
    bool loadIntoEditor(EditorWidget *editor, const QString &path);
    EditorWidget *requireEditor();
    void applyEditorPreferences(EditorWidget *editor);

    // --- State ---------------------------------------------------------
    SettingsManager *m_settings;
    QTabWidget      *m_tabs;
    DocumentManager *m_docs;
    FindReplaceDialog *m_findDialog = nullptr;
    QTimer *m_autoSaveTimer;

    // --- Status bar widgets -------------------------------------------
    QLabel *m_cursorLabel;
    QLabel *m_statsLabel;
    QLabel *m_zoomLabel;
    QLabel *m_modifiedLabel;

    // --- Recent files menu --------------------------------------------
    QMenu *m_recentMenu = nullptr;
    enum { kMaxRecentActions = SettingsManager::kMaxRecentFiles };
    QAction *m_recentActions[kMaxRecentActions];
    QAction *m_clearRecentAction = nullptr;

    // --- Actions (created in createActions, used by menus & toolbar) ---
    QAction *m_newAction = nullptr;
    QAction *m_openAction = nullptr;
    QAction *m_saveAction = nullptr;
    QAction *m_saveAsAction = nullptr;
    QAction *m_exportPdfAction = nullptr;
    QAction *m_printAction = nullptr;
    QAction *m_printPreviewAction = nullptr;
    QAction *m_exitAction = nullptr;

    QAction *m_undoAction = nullptr;
    QAction *m_redoAction = nullptr;
    QAction *m_cutAction  = nullptr;
    QAction *m_copyAction = nullptr;
    QAction *m_pasteAction = nullptr;
    QAction *m_selectAllAction = nullptr;
    QAction *m_findAction = nullptr;

    QAction *m_boldAction = nullptr;
    QAction *m_italicAction = nullptr;
    QAction *m_underlineAction = nullptr;
    QAction *m_fontAction = nullptr;
    QAction *m_colorAction = nullptr;
    QAction *m_alignLeftAction = nullptr;
    QAction *m_alignCenterAction = nullptr;
    QAction *m_alignRightAction = nullptr;

    QAction *m_insertTableAction = nullptr;
    QAction *m_insertImageAction = nullptr;
    QAction *m_rowAboveAction = nullptr;
    QAction *m_rowBelowAction = nullptr;
    QAction *m_colLeftAction = nullptr;
    QAction *m_colRightAction = nullptr;
    QAction *m_delRowAction = nullptr;
    QAction *m_delColAction = nullptr;

    QAction *m_lineNumbersAction = nullptr;
    QAction *m_wordWrapAction = nullptr;
    QAction *m_zoomInAction = nullptr;
    QAction *m_zoomOutAction = nullptr;
    QAction *m_zoomResetAction = nullptr;
    QAction *m_lightThemeAction = nullptr;
    QAction *m_darkThemeAction = nullptr;

    QAction *m_statsAction = nullptr;
    QAction *m_autoSaveAction = nullptr;
    QAction *m_aboutAction = nullptr;
};

#endif // ATE_MAINWINDOW_H
