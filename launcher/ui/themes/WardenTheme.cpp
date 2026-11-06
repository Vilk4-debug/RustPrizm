// SPDX-License-Identifier: GPL-3.0-only
/*
 *  RustPrizm - Minecraft Launcher
 *  Copyright (C) 2026 RustPrizm Contributors
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 */

#include "WardenTheme.h"

#include <QObject>

QString WardenTheme::id()
{
    return "warden";
}

QString WardenTheme::name()
{
    return QObject::tr("Warden Deep Dark");
}

bool WardenTheme::hasColorScheme()
{
    return true;
}

QPalette WardenTheme::colorScheme()
{
    QPalette pal;
    // Window / Background: Sculk Dark #090d14
    pal.setColor(QPalette::Window, QColor(0x09, 0x0d, 0x14));
    pal.setColor(QPalette::WindowText, QColor(0xe2, 0xf1, 0xf8));

    // Base / View background: #101722
    pal.setColor(QPalette::Base, QColor(0x10, 0x17, 0x22));
    pal.setColor(QPalette::AlternateBase, QColor(0x17, 0x22, 0x32));

    // Tooltip
    pal.setColor(QPalette::ToolTipBase, QColor(0x10, 0x17, 0x22));
    pal.setColor(QPalette::ToolTipText, QColor(0x00, 0xe5, 0xff));

    // Text: Main text #e2f1f8, Muted placeholder #7a9ab0
    pal.setColor(QPalette::Text, QColor(0xe2, 0xf1, 0xf8));
    pal.setColor(QPalette::PlaceholderText, QColor(0x7a, 0x9a, 0xb0));

    // Buttons: Secondary background #172232
    pal.setColor(QPalette::Button, QColor(0x17, 0x22, 0x32));
    pal.setColor(QPalette::ButtonText, QColor(0xe2, 0xf1, 0xf8));

    // Highlights / Accents: Soul Fire Cyan #00e5ff
    pal.setColor(QPalette::BrightText, QColor(0x00, 0xe5, 0xff));
    pal.setColor(QPalette::Link, QColor(0x00, 0xe5, 0xff));
    pal.setColor(QPalette::LinkVisited, QColor(0x00, 0xb8, 0xd4));
    pal.setColor(QPalette::Highlight, QColor(0x00, 0xe5, 0xff));
    pal.setColor(QPalette::HighlightedText, QColor(0x09, 0x0d, 0x14));

    // Borders / Shadow
    pal.setColor(QPalette::Midlight, QColor(0x1f, 0x33, 0x4d));
    pal.setColor(QPalette::Mid, QColor(0x17, 0x22, 0x32));
    pal.setColor(QPalette::Dark, QColor(0x09, 0x0d, 0x14));
    pal.setColor(QPalette::Shadow, QColor(0x05, 0x08, 0x0c));

    return fadeInactive(pal, fadeAmount(), fadeColor());
}

double WardenTheme::fadeAmount()
{
    return 0.5;
}

QColor WardenTheme::fadeColor()
{
    return QColor(0x09, 0x0d, 0x14);
}

bool WardenTheme::hasStyleSheet()
{
    return true;
}

QString WardenTheme::appStyleSheet()
{
    return QString::fromUtf8(R"qss(
/* ==============================================================
   RustPrizm - Warden Deep Dark / Soul Fire Theme (Modern Flat QSS)
   ============================================================== */

QWidget {
    background-color: #090d14;
    color: #e2f1f8;
    selection-background-color: #00e5ff;
    selection-color: #090d14;
}

QMainWindow, QDialog {
    background-color: #090d14;
}

/* --- Push Buttons --- */
QPushButton {
    background-color: #172232;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    padding: 6px 14px;
    font-weight: 500;
}
QPushButton:hover {
    background-color: #1d2d42;
    border: 1px solid #00e5ff;
    color: #ffffff;
}
QPushButton:pressed {
    background-color: #0f1824;
    border: 1px solid #00b8d4;
    color: #00e5ff;
}
QPushButton:disabled {
    background-color: #101722;
    border: 1px solid #152233;
    color: #4a6375;
}
QPushButton:default {
    background-color: #172232;
    border: 1px solid #00e5ff;
    color: #00e5ff;
}
QPushButton:default:hover {
    background-color: #00e5ff;
    border: 1px solid #00e5ff;
    color: #090d14;
}

/* --- Tool Buttons & Toolbars --- */
QToolBar {
    background-color: #101722;
    border: 1px solid #1f334d;
    spacing: 4px;
    padding: 4px;
}
QToolBar::separator {
    background-color: #1f334d;
    width: 1px;
    margin: 4px 6px;
}
QToolButton {
    background-color: transparent;
    color: #e2f1f8;
    border: 1px solid transparent;
    border-radius: 6px;
    padding: 5px;
    margin: 1px;
}
QToolButton:hover {
    background-color: #172232;
    border: 1px solid #00e5ff;
    color: #00e5ff;
}
QToolButton:pressed {
    background-color: #101722;
    border: 1px solid #00b8d4;
}
QToolButton:checked {
    background-color: #172232;
    border: 1px solid #00e5ff;
    color: #00e5ff;
}

/* --- Input Fields --- */
QLineEdit, QSpinBox, QDoubleSpinBox, QTextEdit, QPlainTextEdit {
    background-color: #101722;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    padding: 5px 8px;
}
QLineEdit:hover, QSpinBox:hover, QTextEdit:hover, QPlainTextEdit:hover {
    border: 1px solid #2d4a6e;
}
QLineEdit:focus, QSpinBox:focus, QTextEdit:focus, QPlainTextEdit:focus {
    border: 1px solid #00e5ff;
    background-color: #131c2a;
}
QLineEdit:disabled, QTextEdit:disabled {
    background-color: #090d14;
    color: #4a6375;
    border: 1px solid #152233;
}

/* --- Combo Boxes --- */
QComboBox {
    background-color: #172232;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    padding: 5px 10px;
    min-height: 20px;
}
QComboBox:hover {
    border: 1px solid #00e5ff;
}
QComboBox:focus, QComboBox:on {
    border: 1px solid #00e5ff;
}
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 22px;
    border-left: 1px solid #1f334d;
    border-top-right-radius: 6px;
    border-bottom-right-radius: 6px;
}
QComboBox QAbstractItemView {
    background-color: #101722;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    selection-background-color: #00e5ff;
    selection-color: #090d14;
    padding: 4px;
}

/* --- Item Views (Lists, Trees, Tables, InstanceView) --- */
QListView, QTreeView, QTableView {
    background-color: #101722;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    padding: 4px;
}
QListView::item, QTreeView::item, QTableView::item {
    border-radius: 6px;
    padding: 5px;
    border: 1px solid transparent;
}
QListView::item:hover, QTreeView::item:hover, QTableView::item:hover {
    background-color: #172232;
    border: 1px solid #1f334d;
    color: #00e5ff;
}
QListView::item:selected, QTreeView::item:selected, QTableView::item:selected {
    background-color: #18283c;
    border: 1px solid #00e5ff;
    color: #00e5ff;
}
QListView::item:selected:hover, QTreeView::item:selected:hover {
    background-color: #1f3550;
    border: 1px solid #00e5ff;
    color: #ffffff;
}

/* --- ScrollBars --- */
QScrollBar:vertical {
    background-color: #090d14;
    width: 10px;
    margin: 0px;
    border-radius: 5px;
}
QScrollBar::handle:vertical {
    background-color: #1f334d;
    min-height: 24px;
    border-radius: 5px;
}
QScrollBar::handle:vertical:hover {
    background-color: #00e5ff;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
    background: none;
}
QScrollBar:horizontal {
    background-color: #090d14;
    height: 10px;
    margin: 0px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal {
    background-color: #1f334d;
    min-width: 24px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal:hover {
    background-color: #00e5ff;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0px;
}
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
    background: none;
}

/* --- Tabs --- */
QTabWidget::pane {
    background-color: #101722;
    border: 1px solid #1f334d;
    border-radius: 6px;
    top: -1px;
}
QTabBar::tab {
    background-color: #0c121b;
    color: #7a9ab0;
    border: 1px solid #1f334d;
    border-bottom: none;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    padding: 6px 14px;
    margin-right: 2px;
}
QTabBar::tab:hover {
    background-color: #172232;
    color: #00e5ff;
}
QTabBar::tab:selected {
    background-color: #101722;
    color: #00e5ff;
    border-bottom: 2px solid #00e5ff;
}

/* --- Menus & MenuBar --- */
QMenuBar {
    background-color: #090d14;
    color: #e2f1f8;
    border-bottom: 1px solid #1f334d;
}
QMenuBar::item {
    background-color: transparent;
    padding: 4px 8px;
    border-radius: 4px;
}
QMenuBar::item:selected {
    background-color: #172232;
    color: #00e5ff;
}
QMenu {
    background-color: #101722;
    color: #e2f1f8;
    border: 1px solid #1f334d;
    border-radius: 6px;
    padding: 4px;
}
QMenu::item {
    padding: 6px 24px 6px 12px;
    border-radius: 4px;
}
QMenu::item:selected {
    background-color: #172232;
    color: #00e5ff;
}
QMenu::separator {
    height: 1px;
    background-color: #1f334d;
    margin: 4px 8px;
}

/* --- Group Boxes & Controls --- */
QGroupBox {
    border: 1px solid #1f334d;
    border-radius: 6px;
    margin-top: 14px;
    padding-top: 14px;
    font-weight: bold;
    color: #00e5ff;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 10px;
    padding: 0 4px;
}
QCheckBox, QRadioButton {
    color: #e2f1f8;
    spacing: 6px;
}
QCheckBox::indicator, QRadioButton::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #1f334d;
    border-radius: 4px;
    background-color: #101722;
}
QRadioButton::indicator {
    border-radius: 8px;
}
QCheckBox::indicator:hover, QRadioButton::indicator:hover {
    border: 1px solid #00e5ff;
}
QCheckBox::indicator:checked, QRadioButton::indicator:checked {
    background-color: #00e5ff;
    border: 1px solid #00e5ff;
}

/* --- Progress Bar --- */
QProgressBar {
    background-color: #101722;
    border: 1px solid #1f334d;
    border-radius: 6px;
    text-align: center;
    color: #e2f1f8;
}
QProgressBar::chunk {
    background-color: #00e5ff;
    border-radius: 5px;
}

/* --- Tooltip, Statusbar, Header, Splitter --- */
QToolTip {
    color: #00e5ff;
    background-color: #101722;
    border: 1px solid #00e5ff;
    border-radius: 4px;
    padding: 4px 8px;
}
QStatusBar {
    background-color: #090d14;
    color: #7a9ab0;
    border-top: 1px solid #1f334d;
}
QHeaderView::section {
    background-color: #101722;
    color: #7a9ab0;
    border: none;
    border-right: 1px solid #1f334d;
    border-bottom: 1px solid #1f334d;
    padding: 4px 8px;
}
QSplitter::handle {
    background-color: #1f334d;
}
)qss");
}
