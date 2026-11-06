// SPDX-License-Identifier: GPL-3.0-only
/*
 *  RustPrizm - Minecraft Launcher
 *  Copyright (C) 2026 RustPrizm Contributors
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 */

#pragma once

#include "FusionTheme.h"

class WardenTheme : public FusionTheme
{
public:
    virtual ~WardenTheme() {}

    QString id() override;
    QString name() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    bool hasColorScheme() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
