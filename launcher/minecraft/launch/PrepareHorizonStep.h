// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "launch/LaunchStep.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <atomic>

class PrepareHorizonStep : public LaunchStep
{
    Q_OBJECT
public:
    explicit PrepareHorizonStep(LaunchTask *parent);
    virtual ~PrepareHorizonStep() = default;

    void executeTask() override;
    bool canAbort() const override { return true; }
    bool abort() override;

private:
    bool ensureBinaries(const QString &horizonDir);
    bool ensureServerJar(const QString &horizonDir, const QString &dataRoot);
    bool runPrepareStep(const QString &horizonDir, const QString &dataRoot, const QString &serverJarPath);

    QByteArray httpGet(const QString &urlStr, int *statusCodeOut = nullptr);
    bool downloadFile(const QString &urlStr, const QString &destPath);
    bool runCommand(const QString &program, const QStringList &args, const QString &workDir);

private:
    QNetworkAccessManager m_network;
    QNetworkReply *m_currentReply = nullptr;
    QProcess *m_currentProcess = nullptr;
    std::atomic<bool> m_aborted{false};
};
