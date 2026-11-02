# Advanced Text Editor

A modern, cross-platform desktop text editor and lightweight word processor in
the spirit of Notepad++, built with **C++17** and **Qt 6**. It combines fast
plain-text/code editing with rich-document features: tables, embedded images,
formatting, and PDF export.

> Primary implementation language: **C++ (Qt 6)**. No Python is required — the
> rich-text, image and PDF features are all provided natively by Qt.

---

## Features

### File management
- New / Open / Save / Save As
- Tabbed multi-document interface
- Recent files list (persisted)
- Auto-save (configurable, off by default)
- Unsaved-changes detection with save prompts
- Drag-and-drop file opening
- UTF-8 everywhere (with BOM detection on read, atomic writes on save)

### Editing
- Cut, Copy, Paste, Select All
- **Multi-level Undo / Redo** (backed by `QTextDocument`'s undo stack)
- Find & Replace (case sensitivity, whole-word, backward, wrap-around, replace all)
- Word wrap toggle
- Line numbering gutter
- Current-line highlight
- Keyboard shortcuts for all common actions

### Rich document features
- **Tables**: insert N×M tables, add/remove rows and columns, edit cells
- **Images**: insert PNG/JPG/JPEG/BMP, scale on insert, align left/center/right.
  Images are embedded (base64) so documents stay self-contained.
- Text formatting: bold, italic, underline, font, text color, paragraph alignment

### Output
- **Export to PDF** (page orientation selectable; preserves text, tables, images)
- Print and Print Preview

### View & UX
- **Dark mode / Light mode**
- Zoom in / out / reset
- Document statistics (words, characters with/without spaces, lines)
- Live status bar: cursor position, counts, zoom level, modified state

---

## Architecture

The code follows the modular design requested in the project brief. Everything
except `main()` lives in a static library (`ate_core`) so it can be unit-tested.

```
Application (main.cpp)
│
├── MainWindow                – menus, toolbar, status bar, orchestration
├── DocumentManager           – open documents / tabs + per-doc metadata
├── EditorWidget              – QTextEdit-based editing surface
│   └── LineNumberArea        – line-number gutter
├── FileManager               – UTF-8 read/write, format detection
├── SettingsManager           – persisted preferences (QSettings)
├── ThemeManager              – light/dark palettes
├── TableManager              – table insert/edit operations
├── ImageManager              – image insert/align (embedded)
├── PdfExportManager          – PDF export, print, print preview
├── DocumentStats             – word/char/line statistics (pure, tested)
└── FindReplaceDialog         – modeless find & replace UI
```

Design principles: object-oriented, modular, UTF-8-safe, exception-free I/O with
explicit error reporting, and RAII/Qt parent-ownership for memory management.

---

## Keyboard shortcuts

| Action           | Shortcut            | Action            | Shortcut              |
|------------------|---------------------|-------------------|-----------------------|
| New              | `Ctrl+N`            | Find & Replace    | `Ctrl+F`              |
| Open             | `Ctrl+O`            | Bold              | `Ctrl+B`              |
| Save             | `Ctrl+S`            | Italic            | `Ctrl+I`              |
| Save As          | `Ctrl+Shift+S`      | Underline         | `Ctrl+U`              |
| Export to PDF    | `Ctrl+E`            | Zoom In           | `Ctrl++`              |
| Print            | `Ctrl+P`            | Zoom Out          | `Ctrl+-`              |
| Undo / Redo      | `Ctrl+Z` / `Ctrl+Y` | Reset Zoom        | `Ctrl+0`              |
| Cut/Copy/Paste   | `Ctrl+X/C/V`        | Select All        | `Ctrl+A`              |
| Quit             | `Ctrl+Q`            |                   |                       |

---

## Building

See [INSTALL.md](INSTALL.md) for full prerequisites and per-platform steps. In
short:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt6
cmake --build build --config Release
ctest --test-dir build            # run unit tests
```

The resulting `AdvancedTextEditor` executable is in `build/` (or
`build/Release/` on multi-config generators such as Visual Studio).

---

## File formats

| Extension      | Stored as        | Preserves formatting/tables/images |
|----------------|------------------|------------------------------------|
| `.txt`, code   | Plain UTF-8 text | No (plain text)                    |
| `.html`, `.htm`| Qt-generated HTML| Yes                                |
| `.pdf` (export)| PDF              | Yes (read-only output)             |

To keep tables and images when saving, use **Save As → Rich text / HTML**, or
**Export to PDF** for a fixed-layout document.

---

## Tests

Unit tests use the Qt Test framework and cover the pure logic components
(`DocumentStats`, `FileManager`). Run them with `ctest` after building, or
execute the individual `test_*` binaries directly.


