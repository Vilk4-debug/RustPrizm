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

#include "PrepareHorizonStep.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QEventLoop>
#include <QNetworkRequest>

#include "Application.h"
#include "FileSystem.h"
#include "MMCZip.h"
#include "launch/LaunchTask.h"

PrepareHorizonStep::PrepareHorizonStep(LaunchTask *parent)
    : LaunchStep(parent)
{
}

bool PrepareHorizonStep::abort()
{
    m_aborted = true;
    if (m_currentReply) {
        m_currentReply->abort();
    }
    if (m_currentProcess && m_currentProcess->state() != QProcess::NotRunning) {
        m_currentProcess->kill();
    }
    return true;
}

QByteArray PrepareHorizonStep::httpGet(const QString &urlStr, int *statusCodeOut)
{
    QString currentUrl = urlStr;
    for (int redirects = 0; redirects < 10 && !m_aborted; ++redirects) {
        QUrl url(currentUrl);
        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::UserAgentHeader, "PolyMC/8.1");
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
#else
        req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif

        QNetworkReply *reply = m_network.get(req);
        m_currentReply = reply;

        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        m_currentReply = nullptr;

        if (m_aborted) {
            reply->deleteLater();
            return QByteArray();
        }

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCodeOut) {
            *statusCodeOut = statusCode;
        }

        if (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307 || statusCode == 308) {
            QUrl redirectUrl = reply->header(QNetworkRequest::LocationHeader).toUrl();
            if (!redirectUrl.isValid()) {
                redirectUrl = QUrl(QString::fromUtf8(reply->rawHeader("Location")));
            }
            reply->deleteLater();
            if (redirectUrl.isValid()) {
                currentUrl = redirectUrl.isRelative()
                                 ? QUrl(currentUrl).resolved(redirectUrl).toString()
                                 : redirectUrl.toString();
                continue;
            }
            return QByteArray();
        }

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "HTTP GET error for" << currentUrl << ":" << reply->errorString();
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();
        return data;
    }
    return QByteArray();
}

bool PrepareHorizonStep::downloadFile(const QString &urlStr, const QString &destPath)
{
    QString currentUrl = urlStr;
    for (int redirects = 0; redirects < 10 && !m_aborted; ++redirects) {
        QUrl url(currentUrl);
        QNetworkRequest req(url);
        req.setHeader(QNetworkRequest::UserAgentHeader, "PolyMC/8.1");
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
#else
        req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif

        QNetworkReply *reply = m_network.get(req);
        m_currentReply = reply;

        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        m_currentReply = nullptr;

        if (m_aborted) {
            reply->deleteLater();
            return false;
        }

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307 || statusCode == 308) {
            QUrl redirectUrl = reply->header(QNetworkRequest::LocationHeader).toUrl();
            if (!redirectUrl.isValid()) {
                redirectUrl = QUrl(QString::fromUtf8(reply->rawHeader("Location")));
            }
            reply->deleteLater();
            if (redirectUrl.isValid()) {
                currentUrl = redirectUrl.isRelative()
                                 ? QUrl(currentUrl).resolved(redirectUrl).toString()
                                 : redirectUrl.toString();
                continue;
            }
            return false;
        }

        if (reply->error() != QNetworkReply::NoError) {
            emit logLine(QString("Download error (%1): %2").arg(currentUrl, reply->errorString()), MessageLevel::Error);
            reply->deleteLater();
            return false;
        }

        QFile outFile(destPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            emit logLine(QString("Cannot open file for writing: %1").arg(destPath), MessageLevel::Error);
            reply->deleteLater();
            return false;
        }

        outFile.write(reply->readAll());
        outFile.close();
        reply->deleteLater();
        return true;
    }
    return false;
}

bool PrepareHorizonStep::runCommand(const QString &program, const QStringList &args, const QString &workDir)
{
    if (m_aborted) {
        return false;
    }

    emit logLine(QString("Running: %1 %2").arg(program, args.join(" ")), MessageLevel::Launcher);
    QProcess process;
    m_currentProcess = &process;
    process.setWorkingDirectory(workDir);

    connect(&process, &QProcess::readyReadStandardOutput, [&]() {
        QString text = QString::fromUtf8(process.readAllStandardOutput());
        QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        for (const auto &line : lines) {
            emit logLine(line, MessageLevel::StdOut);
        }
    });

    connect(&process, &QProcess::readyReadStandardError, [&]() {
        QString text = QString::fromUtf8(process.readAllStandardError());
        QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        for (const auto &line : lines) {
            emit logLine(line, MessageLevel::StdErr);
        }
    });

    QEventLoop loop;
    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);

    process.start(program, args);
    if (!process.waitForStarted(10000)) {
        m_currentProcess = nullptr;
        emit logLine(QString("Failed to start executable: %1").arg(program), MessageLevel::Error);
        return false;
    }

    loop.exec();
    m_currentProcess = nullptr;

    if (m_aborted) {
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        emit logLine(QString("Process %1 exited with code %2").arg(program).arg(process.exitCode()), MessageLevel::Error);
        return false;
    }
    return true;
}

bool PrepareHorizonStep::ensureBinaries(const QString &horizonDir)
{
#ifdef Q_OS_WIN
    const QString clientExeName = "horizon-client.exe";
    const QString prepareExeName = "horizon-prepare.exe";
#else
    const QString clientExeName = "horizon-client";
    const QString prepareExeName = "horizon-prepare";
#endif

    QString clientExe = FS::PathCombine(horizonDir, clientExeName);
    QString prepareExe = FS::PathCombine(horizonDir, prepareExeName);

    if (QFile::exists(clientExe) && QFile::exists(prepareExe)) {
        emit logLine("Horizon binaries found.", MessageLevel::Launcher);
        return true;
    }

    emit logLine("Horizon binaries not found in " + horizonDir + ". Fetching Windows x64 build...", MessageLevel::Launcher);

    QString zipPath = FS::PathCombine(horizonDir, "horizon-windows-x64.zip");
    const QString directNightlyUrl = "https://github.com/horizon-rust/horizon/releases/download/nightly/horizon-windows-x64.zip";

    bool downloaded = false;

    // 1. Prioritize direct download link
    emit logLine("Attempting direct download of Horizon package: " + directNightlyUrl, MessageLevel::Launcher);
    if (downloadFile(directNightlyUrl, zipPath)) {
        QFileInfo fi(zipPath);
        if (fi.exists() && fi.size() >= 102400) {
            downloaded = true;
        } else {
            emit logLine("Direct download did not provide a valid archive, falling back to GitHub API...", MessageLevel::Warning);
            QFile::remove(zipPath);
        }
    }

    // 2. If direct download did not succeed, search GitHub Releases API strictly for windows-x64
    if (!downloaded) {
        auto isMatchingAsset = [](const QString &name) -> bool {
            QString lower = name.toLower();
            return lower.endsWith(".zip") && lower.contains("windows") && lower.contains("x64");
        };

        auto extractAssetUrl = [&](const QJsonObject &releaseObj) -> QString {
            if (releaseObj.contains("assets") && releaseObj["assets"].isArray()) {
                for (const auto &val : releaseObj["assets"].toArray()) {
                    if (val.isObject()) {
                        auto assetObj = val.toObject();
                        QString name = assetObj.value("name").toString();
                        if (isMatchingAsset(name)) {
                            return assetObj.value("browser_download_url").toString();
                        }
                    }
                }
            }
            return QString();
        };

        QString downloadUrl;
        int statusCode = 0;

        // Try nightly tag release
        emit logLine("Checking GitHub Releases API for nightly tag...", MessageLevel::Launcher);
        QByteArray data = httpGet("https://api.github.com/repos/horizon-rust/horizon/releases/tags/nightly", &statusCode);
        if (!data.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                downloadUrl = extractAssetUrl(doc.object());
            }
        }

        // Fallback to /releases list
        if (downloadUrl.isEmpty()) {
            emit logLine("Nightly tag asset not found via API. Checking all releases...", MessageLevel::Launcher);
            data = httpGet("https://api.github.com/repos/horizon-rust/horizon/releases", &statusCode);
            if (!data.isEmpty()) {
                QJsonDocument doc = QJsonDocument::fromJson(data);
                if (doc.isArray()) {
                    for (const auto &relVal : doc.array()) {
                        if (relVal.isObject()) {
                            downloadUrl = extractAssetUrl(relVal.toObject());
                            if (!downloadUrl.isEmpty()) {
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (downloadUrl.isEmpty()) {
            emit logLine("Failed to locate horizon-windows-x64.zip in GitHub releases.", MessageLevel::Error);
            return false;
        }

        emit logLine("Downloading Horizon package: " + downloadUrl, MessageLevel::Launcher);
        if (!downloadFile(downloadUrl, zipPath)) {
            emit logLine("Failed to download Horizon archive.", MessageLevel::Error);
            return false;
        }

        QFileInfo fi(zipPath);
        if (!fi.exists() || fi.size() < 102400) {
            emit logLine("Downloaded Horizon archive is invalid or empty.", MessageLevel::Error);
            QFile::remove(zipPath);
            return false;
        }
    }

    emit logLine("Extracting Horizon binaries to " + horizonDir + " ...", MessageLevel::Launcher);
    auto extracted = MMCZip::extractDir(zipPath, horizonDir);
    QFile::remove(zipPath);

    // If archive had an internal root folder 'horizon-windows-x64', move files up to horizonDir
    QString subDir = FS::PathCombine(horizonDir, "horizon-windows-x64");
    if (QDir(subDir).exists()) {
        QDir sDir(subDir);
        for (const auto &entry : sDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
            QString dest = FS::PathCombine(horizonDir, entry.fileName());
            if (QFile::exists(dest)) {
                if (entry.isDir()) {
                    FS::deletePath(dest);
                } else {
                    QFile::remove(dest);
                }
            }
            QFile::rename(entry.absoluteFilePath(), dest);
        }
        sDir.removeRecursively();
    }

    if (!extracted.has_value() || !QFile::exists(clientExe) || !QFile::exists(prepareExe)) {
        emit logLine("Failed to extract Horizon binaries or binaries missing after extraction.", MessageLevel::Error);
        return false;
    }

    emit logLine("Horizon binaries successfully installed.", MessageLevel::Launcher);
    return true;
}

bool PrepareHorizonStep::ensureServerJar(const QString &horizonDir, const QString &dataRoot)
{
    // 1. Ensure official Mojang versions/26.3/26.3.json exists and is valid (>= 2 KB)
    QString versionsDir = FS::PathCombine(dataRoot, "versions/26.3");
    FS::ensureFolderPathExists(versionsDir);

    QString versionJsonPath = FS::PathCombine(versionsDir, "26.3.json");
    bool hasValidVersionJson = false;

    QFileInfo jsonInfo(versionJsonPath);
    if (jsonInfo.exists() && jsonInfo.size() >= 2048) {
        hasValidVersionJson = true;
    }

    if (!hasValidVersionJson) {
        emit logLine("Official Mojang 26.3.json missing or smaller than 2 KB. Fetching manifest from Mojang...", MessageLevel::Launcher);
        QString versionJsonUrl;

        QByteArray manifestData = httpGet("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json");
        if (!manifestData.isEmpty()) {
            QJsonDocument manifestDoc = QJsonDocument::fromJson(manifestData);
            if (manifestDoc.isObject() && manifestDoc.object().contains("versions")) {
                for (const auto &verVal : manifestDoc.object()["versions"].toArray()) {
                    if (verVal.isObject()) {
                        auto verObj = verVal.toObject();
                        if (verObj.value("id").toString() == "26.3") {
                            versionJsonUrl = verObj.value("url").toString();
                            break;
                        }
                    }
                }
            }
        }

        if (versionJsonUrl.isEmpty()) {
            versionJsonUrl = "https://piston-meta.mojang.com/v1/packages/9f67e10ed013deeaad20ac3c800652765d493408/26.3.json";
        }

        emit logLine("Downloading official Minecraft 26.3 version JSON from " + versionJsonUrl, MessageLevel::Launcher);
        QByteArray verJsonData = httpGet(versionJsonUrl);
        if (verJsonData.size() >= 2048) {
            QFile outFile(versionJsonPath);
            if (outFile.open(QIODevice::WriteOnly)) {
                outFile.write(verJsonData);
                outFile.close();
                emit logLine("Successfully saved official Mojang 26.3.json to " + versionJsonPath, MessageLevel::Launcher);
                hasValidVersionJson = true;
            } else {
                emit logLine("Cannot open " + versionJsonPath + " for writing.", MessageLevel::Error);
            }
        } else {
            emit logLine("Failed to download official Mojang 26.3.json (data smaller than 2 KB).", MessageLevel::Error);
        }
    }

    if (!hasValidVersionJson) {
        emit logLine("Error: Required official Mojang manifest " + versionJsonPath + " could not be obtained.", MessageLevel::Error);
        return false;
    }

    // 2. Check server-jars/26.3.jar
    QString serverJarsDir = FS::PathCombine(horizonDir, "server-jars");
    FS::ensureFolderPathExists(serverJarsDir);

    QString serverJarPath = FS::PathCombine(serverJarsDir, "26.3.jar");
    if (QFile::exists(serverJarPath) && QFileInfo(serverJarPath).size() >= 102400) {
        emit logLine("Minecraft 26.3 server.jar found.", MessageLevel::Launcher);
        return true;
    }

    emit logLine("Minecraft 26.3 server.jar not found. Extracting server URL from " + versionJsonPath + " ...", MessageLevel::Launcher);

    QString serverUrl;
    auto extractServerUrl = [](const QJsonObject &obj) -> QString {
        if (obj.contains("downloads") && obj["downloads"].isObject()) {
            auto downloads = obj["downloads"].toObject();
            if (downloads.contains("server") && downloads["server"].isObject()) {
                return downloads["server"].toObject().value("url").toString();
            }
        }
        if (obj.contains("mojangDownloads") && obj["mojangDownloads"].isObject()) {
            auto downloads = obj["mojangDownloads"].toObject();
            if (downloads.contains("server") && downloads["server"].isObject()) {
                return downloads["server"].toObject().value("url").toString();
            }
        }
        return QString();
    };

    QFile file(versionJsonPath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject()) {
            serverUrl = extractServerUrl(doc.object());
        }
        file.close();
    }

    if (serverUrl.isEmpty()) {
        serverUrl = "https://piston-data.mojang.com/v1/objects/33680f5f2ac32864d6d7cf5e56a705fdb3e05f4c/server.jar";
    }

    emit logLine("Downloading 26.3 server.jar from: " + serverUrl, MessageLevel::Launcher);
    if (!downloadFile(serverUrl, serverJarPath)) {
        emit logLine("Failed to download Minecraft 26.3 server.jar.", MessageLevel::Error);
        return false;
    }

    QFileInfo sfi(serverJarPath);
    if (!sfi.exists() || sfi.size() < 102400) {
        emit logLine("Downloaded server.jar is invalid or empty.", MessageLevel::Error);
        QFile::remove(serverJarPath);
        return false;
    }

    emit logLine("Minecraft 26.3 server.jar successfully saved.", MessageLevel::Launcher);
    return true;
}

bool PrepareHorizonStep::runPrepareStep(const QString &horizonDir, const QString &dataRoot, const QString &serverJarPath)
{
    QString manifestPath = FS::PathCombine(horizonDir, ".local/horizon-client/26.3/horizon-resources.json");
    if (QFile::exists(manifestPath)) {
        emit logLine("Horizon resources already prepared (horizon-resources.json found).", MessageLevel::Launcher);
        return true;
    }

    // Ensure versions/26.3 directory exists
    QString versionsDir = FS::PathCombine(dataRoot, "versions/26.3");
    FS::ensureFolderPathExists(versionsDir);

    // Verify versions/26.3/26.3.json exists
    QString versionJsonPath = FS::PathCombine(versionsDir, "26.3.json");
    if (!QFile::exists(versionJsonPath) || QFileInfo(versionJsonPath).size() < 2048) {
        emit logLine("Error: Required manifest " + versionJsonPath + " is missing or invalid before running horizon-prepare.", MessageLevel::Error);
        return false;
    }

    // Ensure versions/26.3/26.3.jar exists
    QString versionJarPath = FS::PathCombine(versionsDir, "26.3.jar");
    if (!QFile::exists(versionJarPath) || QFileInfo(versionJarPath).size() < 102400) {
        QString clientJarPath = FS::PathCombine(dataRoot, "libraries/com/mojang/minecraft/26.3/minecraft-26.3-client.jar");
        if (QFile::exists(clientJarPath) && QFileInfo(clientJarPath).size() > 102400) {
            emit logLine("Copying client jar from " + clientJarPath + " to " + versionJarPath + " ...", MessageLevel::Launcher);
            if (QFile::exists(versionJarPath)) {
                QFile::remove(versionJarPath);
            }
            if (!QFile::copy(clientJarPath, versionJarPath)) {
                emit logLine("Failed to copy client jar to " + versionJarPath, MessageLevel::Error);
                return false;
            }
        } else {
            // Fallback: download client jar directly from Mojang
            emit logLine("Client jar not found at " + clientJarPath + ". Downloading 26.3 client.jar to " + versionJarPath + " ...", MessageLevel::Launcher);
            const QString clientUrl = "https://piston-data.mojang.com/v1/objects/e877b6a07acd633fb3bb475002175cec036e7b87/client.jar";
            if (!downloadFile(clientUrl, versionJarPath)) {
                emit logLine("Failed to download client jar.", MessageLevel::Error);
                return false;
            }
        }
    }

    if (!QFile::exists(versionJarPath) || QFileInfo(versionJarPath).size() < 102400) {
        emit logLine("Error: Required client jar " + versionJarPath + " is missing or corrupt.", MessageLevel::Error);
        return false;
    }

#ifdef Q_OS_WIN
    const QString prepareExeName = "horizon-prepare.exe";
#else
    const QString prepareExeName = "horizon-prepare";
#endif
    QString prepareExe = FS::PathCombine(horizonDir, prepareExeName);

    emit logLine("Horizon resources not prepared. Starting preparation pipeline...", MessageLevel::Launcher);

    // 1. --prepare
    QStringList argsPrepare = {
        "--prepare",
        "--minecraft-dir", dataRoot,
        "--server-jar", serverJarPath,
        "--root", horizonDir
    };
    if (!runCommand(prepareExe, argsPrepare, horizonDir)) {
        emit logLine("Error: horizon-prepare --prepare failed.", MessageLevel::Error);
        return false;
    }

    // 2. sounds
    QStringList argsSounds = {
        "sounds",
        "--minecraft-dir", dataRoot,
        "--root", horizonDir
    };
    runCommand(prepareExe, argsSounds, horizonDir);

    // 3. panorama
    QStringList argsPanorama = {
        "panorama",
        "--root", horizonDir
    };
    runCommand(prepareExe, argsPanorama, horizonDir);

    // 4. fonts
    QStringList argsFonts = {
        "fonts",
        "--root", horizonDir
    };
    runCommand(prepareExe, argsFonts, horizonDir);

    // 5. languages
    QStringList argsLanguages = {
        "languages",
        "--root", horizonDir
    };
    runCommand(prepareExe, argsLanguages, horizonDir);

    if (!QFile::exists(manifestPath)) {
        emit logLine("Preparation finished but horizon-resources.json was not created at " + manifestPath, MessageLevel::Error);
        return false;
    }

    emit logLine("Horizon resources prepared successfully!", MessageLevel::Launcher);
    return true;
}

void PrepareHorizonStep::executeTask()
{
    QString dataRoot = QDir::currentPath();
    QString horizonDir = FS::PathCombine(dataRoot, "horizon");
    FS::ensureFolderPathExists(horizonDir);

    emit logLine("=== Step 1/3: Checking Horizon binaries ===", MessageLevel::Launcher);
    if (!ensureBinaries(horizonDir)) {
        emitFailed(tr("Failed to install Horizon binaries."));
        return;
    }

    if (m_aborted) {
        emitFailed(tr("Aborted by user."));
        return;
    }

    emit logLine("=== Step 2/3: Checking Minecraft 26.3 server.jar and official version manifest ===", MessageLevel::Launcher);
    if (!ensureServerJar(horizonDir, dataRoot)) {
        emitFailed(tr("Failed to obtain Minecraft 26.3 server.jar or version manifest."));
        return;
    }

    if (m_aborted) {
        emitFailed(tr("Aborted by user."));
        return;
    }

    emit logLine("=== Step 3/3: Checking Horizon resources ===", MessageLevel::Launcher);
    QString serverJarPath = FS::PathCombine(horizonDir, "server-jars/26.3.jar");
    if (!runPrepareStep(horizonDir, dataRoot, serverJarPath)) {
        emitFailed(tr("Failed to prepare Horizon resources."));
        return;
    }

    emitSucceeded();
}
