#ifndef ATE_THEMEMANAGER_H
#define ATE_THEMEMANAGER_H

#include "core/SettingsManager.h"

class QApplication;

/**
 * @brief Applies light / dark palettes application-wide.
 *
 * Stateless helper: a palette is built for the requested theme and installed
 * on the QApplication. The Fusion style is used so the custom palette renders
 * consistently across supported platforms.
 */
class ThemeManager
{
public:
    static void applyTheme(QApplication &app, SettingsManager::Theme theme);
};

#endif // ATE_THEMEMANAGER_H
