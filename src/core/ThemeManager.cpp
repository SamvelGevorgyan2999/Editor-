#include "core/ThemeManager.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

void ThemeManager::applyTheme(QApplication &app, SettingsManager::Theme theme)
{
    // Fusion honours custom palettes identically on every platform.
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette palette;

    if (theme == SettingsManager::Theme::Dark) {
        const QColor window(53, 53, 53);
        const QColor base(35, 35, 35);
        const QColor alternate(45, 45, 45);
        const QColor text(220, 220, 220);
        const QColor highlight(42, 130, 218);
        const QColor disabled(127, 127, 127);

        palette.setColor(QPalette::Window, window);
        palette.setColor(QPalette::WindowText, text);
        palette.setColor(QPalette::Base, base);
        palette.setColor(QPalette::AlternateBase, alternate);
        palette.setColor(QPalette::ToolTipBase, window);
        palette.setColor(QPalette::ToolTipText, text);
        palette.setColor(QPalette::Text, text);
        palette.setColor(QPalette::Button, window);
        palette.setColor(QPalette::ButtonText, text);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, highlight);
        palette.setColor(QPalette::Highlight, highlight);
        palette.setColor(QPalette::HighlightedText, Qt::black);

        palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    } else {
        // Light theme: start from the Fusion standard palette.
        palette = app.style()->standardPalette();
    }

    app.setPalette(palette);
}

