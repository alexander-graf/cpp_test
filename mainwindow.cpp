#include "mainwindow.h"
#include <QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <QProcess>
#include <QRegularExpression>
#include <QTimer>

SecondWindow::SecondWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Hilfe");
    layout = new QVBoxLayout(this);
    
    label = new QLabel("Espanso Helper Hilfe:\n\n"
                      "1. Wählen Sie eine YAML-Datei aus\n"
                      "2. Geben Sie einen Trigger ein\n"
                      "3. Geben Sie den zu ersetzenden Text ein\n"
                      "4. Klicken Sie auf Speichern", this);
    closeButton = new QPushButton("Schließen", this);
    
    layout->addWidget(label);
    layout->addWidget(closeButton);
    
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    setMinimumSize(300, 200);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isDarkTheme(true), isEditing(false)
{
    setWindowFlags(Qt::FramelessWindowHint);  // Entfernt die Standard-Titelleiste
    menuBar()->hide();  // Menüleiste ausblenden
    createMenus();
    createTitleBar();
    createLayout();
    setMinimumSize(600, 400);
    setWindowTitle("Espanso Helper");
    statusBar()->showMessage("Bereit");
    setDarkTheme();
    setupAutoSave();
    
    // Versuche Recovery beim Start
    QDir configDir(getEspansoConfigPath());
    QStringList backups = configDir.entryList({"*.bak"}, QDir::Files);
    if (!backups.isEmpty()) {
        int ret = QMessageBox::question(this, "Backup gefunden",
            "Es wurden Backup-Dateien gefunden. Möchten Sie diese wiederherstellen?",
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            restoreFromBackup();
        }
    }

    // Styling für die ComboBox
    yamlComboBox->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #828790;
            border-radius: 3px;
            padding: 1px 18px 1px 3px;
            min-width: 6em;
        }

        QComboBox::drop-down {
            border: none;
            border-left: 1px solid #828790;
            width: 20px;
        }

        QComboBox::down-arrow {
            width: 0;
            height: 0;
            border: 6px solid transparent;
            border-top: 6px solid #828790;
            margin-top: 2px;
        }

        QComboBox QAbstractItemView {
            background-color: #353535;
            color: #ffffff;
            selection-background-color: #404040;
            selection-color: #ffffff;
            border: 1px solid #828790;
        }
    )");
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("&Datei");
    QMenu *viewMenu = menuBar()->addMenu("&Ansicht");
    QMenu *helpMenu = menuBar()->addMenu("&Hilfe");
    
    fileMenu->addAction("&Beenden", this, &QWidget::close);
    
    QAction *themeAction = viewMenu->addAction("Dark Mode");
    themeAction->setCheckable(true);
    themeAction->setChecked(true);
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    
    helpMenu->addAction("Ü&ber", this, &MainWindow::about);
}

void MainWindow::createTitleBar() {
    titleBar = new QWidget(this);
    titleBar->setFixedHeight(30);
    titleBar->setStyleSheet("background-color: #1a1a1a;");
    
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 0, 0);
    
    // Fenstertitel
    windowTitle = new QLabel("Espanso Helper", titleBar);
    windowTitle->setStyleSheet("color: #ffffff; font-weight: bold;");
    
    // Buttons
    minimizeButton = new QPushButton("─", titleBar);
    closeButton = new QPushButton("✕", titleBar);
    
    QString buttonStyle = R"(
        QPushButton {
            color: #ffffff;
            background: transparent;
            border: none;
            font-size: 16px;
            padding: 5px 10px;
        }
        QPushButton:hover {
            background-color: #404040;
        }
        #closeButton:hover {
            background-color: #e81123;
        }
    )";
    
    minimizeButton->setStyleSheet(buttonStyle);
    closeButton->setStyleSheet(buttonStyle);
    closeButton->setObjectName("closeButton");
    
    titleLayout->addWidget(windowTitle);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(closeButton);
    
    connect(minimizeButton, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::close);
}

void MainWindow::createLayout() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // Keine Ränder für das Hauptlayout
    
    // Füge die Titelleiste hinzu
    mainLayout->addWidget(titleBar);
    
    // Container für den Hauptinhalt
    QWidget *contentWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(contentWidget);
    
    // YAML File Selector mit Refresh-Button
    QHBoxLayout *yamlSelectorLayout = new QHBoxLayout;
    yamlComboBox = new QComboBox;
    yamlComboBox->setIconSize(QSize(16, 16));
    QPushButton *refreshButton = new QPushButton(QIcon(":/icons/refresh.png"), "");
    refreshButton->setMaximumWidth(30);
    refreshButton->setToolTip("Aktualisieren");
    QPushButton *openFolderButton = new QPushButton(QIcon(":/icons/folder.png"), "");
    openFolderButton->setMaximumWidth(30);
    openFolderButton->setToolTip("Ordner öffnen");
    
    yamlSelectorLayout->addWidget(yamlComboBox);
    yamlSelectorLayout->addWidget(refreshButton);
    yamlSelectorLayout->addWidget(openFolderButton);
    
    // Input Fields
    QGridLayout *inputLayout = new QGridLayout;
    
    triggerEdit = new QLineEdit;
    triggerEdit->setPlaceholderText("Trigger eingeben (z.B. :btw)");
    
    replaceEdit = new QTextEdit;
    replaceEdit->setPlaceholderText("Zu ersetzenden Text eingeben");
    replaceEdit->setMinimumHeight(100);
    
    inputLayout->addWidget(new QLabel("YAML Datei:"), 0, 0);
    inputLayout->addLayout(yamlSelectorLayout, 0, 1);
    inputLayout->addWidget(new QLabel("Trigger:"), 1, 0);
    inputLayout->addWidget(triggerEdit, 1, 1);
    inputLayout->addWidget(new QLabel("Ersetzen mit:"), 2, 0);
    inputLayout->addWidget(replaceEdit, 2, 1);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    saveButton = new QPushButton(QIcon(":/icons/save.png"), "Speichern", this);
    deleteButton = new QPushButton(QIcon(":/icons/delete.png"), "Löschen", this);
    helpButton = new QPushButton(QIcon(":/icons/help.png"), "Hilfe", this);
    clearButton = new QPushButton(QIcon(":/icons/clear.png"), "Leeren", this);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(helpButton);
    buttonLayout->addStretch();  // Buttons linksbündig
    
    // Liste der Einträge
    QLabel *listLabel = new QLabel("Vorhandene Einträge:");
    entryList = new QListWidget;
    
    // Erweiterte Statusleiste
    statusEntryCount = new QLabel(this);
    statusFileName = new QLabel(this);
    statusBar()->addPermanentWidget(statusEntryCount);
    statusBar()->addPermanentWidget(statusFileName);
    
    // Zusammensetzen des Content-Layouts
    contentLayout->addLayout(inputLayout);
    contentLayout->addSpacing(10);
    contentLayout->addLayout(buttonLayout);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(listLabel);
    contentLayout->addWidget(entryList);
    
    // Connections
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::handleSave);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::handleDelete);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearInputFields);
    connect(helpButton, &QPushButton::clicked, this, &MainWindow::openSecondWindow);
    connect(yamlComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onComboBoxChanged);
    connect(entryList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString fullText = item->text();
        QString trigger = fullText.split(" - ").first();
        QString fullReplace = item->data(Qt::UserRole).toString();
        
        triggerEdit->setText(trigger);
        replaceEdit->setText(fullReplace);
        
        // Speichere den aktuellen Zustand
        isEditing = true;
        currentTrigger = trigger;
        currentReplace = fullReplace;
    });
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshYamlFiles);
    connect(openFolderButton, &QPushButton::clicked, this, &MainWindow::openConfigFolder);
    
    // Initial YAML files laden
    refreshYamlFiles();
    
    updateStatusBar();

    // Optional: Icon-Größe anpassen
    QString buttonStyle = R"(
        QPushButton {
            padding-left: 5px;
            text-align: center;
        }
        QPushButton QIcon {
            width: 16px;
            height: 16px;
        }
    )";
    
    saveButton->setStyleSheet(buttonStyle);
    deleteButton->setStyleSheet(buttonStyle);
    helpButton->setStyleSheet(buttonStyle);
    clearButton->setStyleSheet(buttonStyle);
}

void MainWindow::updateStatusBar() {
    statusEntryCount->setText(QString("Einträge: %1").arg(entries.size()));
    statusFileName->setText(QFileInfo(currentYamlFile).fileName());
}

void MainWindow::refreshYamlFiles() {
    yamlComboBox->clear();
    entries.clear();
    currentYamlFile.clear();
    updateStatusBar();
    
    QString configPath = getEspansoConfigPath();
    QDir configDir(configPath);
    
    // Versuche Ordner zu erstellen wenn er nicht existiert
    if (!configDir.exists()) {
        if (!configDir.mkpath(".")) {
            QMessageBox::warning(this, "Fehler", 
                "Espanso Konfigurationsordner konnte nicht erstellt werden:\n" + configPath);
            return;
        }
        statusBar()->showMessage("Konfigurationsordner wurde erstellt: " + configPath, 3000);
    }
    
    // Prüfe ob der Ordner lesbar ist
    if (!configDir.isReadable()) {
        QMessageBox::warning(this, "Fehler", 
            "Konfigurationsordner ist nicht lesbar:\n" + configPath);
        return;
    }
    
    // Suche nach YAML-Dateien
    QStringList filters;
    filters << "*.yml";
    configDir.setNameFilters(filters);
    
    QStringList yamlFiles = configDir.entryList(QDir::Files);
    
    // Zeige Statusmeldung
    if (yamlFiles.isEmpty()) {
        statusBar()->showMessage("Keine YAML-Dateien in: " + configPath, 3000);
        updateEntryList();  // Liste leeren
        return;
    }
    
    // Füge gefundene Dateien mit Icons hinzu
    for (const QString &file : yamlFiles) {
        yamlComboBox->addItem(QIcon(":/icons/file.png"), file);
    }
    
    // Lade die erste Datei
    if (!yamlFiles.isEmpty()) {
        yamlComboBox->setCurrentText(yamlFiles.first());
        loadYamlFile(yamlFiles.first());
    }
    
    statusBar()->showMessage(QString("%1 YAML-Datei(en) gefunden").arg(yamlFiles.size()), 2000);
}

void MainWindow::openConfigFolder() {
    QString path = getEspansoConfigPath();
    QDir dir(path);
    
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::warning(this, "Fehler", 
                "Konnte Konfigurationsordner nicht erstellen:\n" + path);
            return;
        }
    }
    
    #ifdef Q_OS_WIN
        QProcess::startDetached("explorer", {QDir::toNativeSeparators(path)});
    #else
        QProcess::startDetached("xdg-open", {path});
    #endif
    
    statusBar()->showMessage("Öffne Konfigurationsordner: " + path, 3000);
}

QString MainWindow::getEspansoConfigPath() {
    QString configPath;
#ifdef Q_OS_WIN
    configPath = QDir::homePath() + "/AppData/Roaming/espanso/match/";
#else
    configPath = QDir::homePath() + "/.config/espanso/match/";
#endif
    return configPath;  // Nicht mehr normalisieren, das machen wir später
}

void MainWindow::loadYamlFile(const QString &filename) {
    if (filename.isEmpty()) {
        return;
    }
    
    QString fullPath = getEspansoConfigPath() + filename;
    QFileInfo fileInfo(fullPath);
    
    // Prüfe Dateizugriff
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "Fehler", "Die Datei existiert nicht: " + fullPath);
        return;
    }
    
    if (!fileInfo.isReadable()) {
        QMessageBox::warning(this, "Fehler", "Die Datei ist nicht lesbar: " + fullPath);
        return;
    }
    
    currentYamlFile = QDir::toNativeSeparators(fullPath);
    entries.clear();
    clearInputFields();  // Leere die Eingabefelder
    
    try {
        std::string stdPath = currentYamlFile.toStdString();
        std::replace(stdPath.begin(), stdPath.end(), '\\', '/');
        
        YAML::Node config = YAML::LoadFile(stdPath);
        
        if (config["matches"]) {
            for (const auto& match : config["matches"]) {
                if (match["trigger"] && match["replace"]) {
                    EspansoEntry entry;
                    entry.trigger = QString::fromStdString(match["trigger"].as<std::string>());
                    entry.replace = QString::fromStdString(match["replace"].as<std::string>());
                    entries.insert(entry.trigger, entry);
                }
            }
        }
        
        updateEntryList();
        updateStatusBar();
        statusBar()->showMessage("YAML-Datei geladen: " + filename, 2000);
    } catch (const YAML::Exception& e) {
        QString errorMsg = QString::fromStdString(e.what());
        errorMsg.replace("bad file: ", "");
        statusBar()->showMessage("Fehler beim Laden der Datei: " + errorMsg, 3000);
        updateEntryList();
        updateStatusBar();
    }
}

void MainWindow::saveYamlEntries() {
    if (currentYamlFile.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Keine YAML-Datei ausgewählt");
        return;
    }
    
    try {
        // Backup der alten Datei erstellen
        QString backupFile = currentYamlFile + ".bak";
        QFile::remove(backupFile);  // Entferne altes Backup
        QFile::copy(currentYamlFile, backupFile);
        
        YAML::Node config;
        config["matches"] = YAML::Node(YAML::NodeType::Sequence);
        
        // Alle Einträge aus dem MultiMap in die YAML-Node schreiben
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            YAML::Node match;
            match["trigger"] = it.value().trigger.toStdString();
            match["replace"] = it.value().replace.toStdString();
            config["matches"].push_back(match);
        }
        
        // Temporäre Datei zum Speichern verwenden
        QString tempFile = currentYamlFile + ".tmp";
        {
            std::ofstream fout(tempFile.toStdString());
            if (!fout.is_open()) {
                throw std::runtime_error("Konnte temporäre Datei nicht öffnen");
            }
            fout << config;
            fout.close();
        }
        
        // Wenn alles geklappt hat, temporäre Datei umbenennen
        QFile oldFile(currentYamlFile);
        if (oldFile.exists() && !oldFile.remove()) {
            throw std::runtime_error("Konnte alte Datei nicht entfernen");
        }
        
        if (!QFile::rename(tempFile, currentYamlFile)) {
            // Versuche Backup wiederherzustellen
            QFile::copy(backupFile, currentYamlFile);
            throw std::runtime_error("Konnte temporäre Datei nicht umbenennen");
        }
        
        updateEntryList();
        updateStatusBar();
        statusBar()->showMessage("Einträge erfolgreich gespeichert", 2000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Fehler", 
            "Fehler beim Speichern:\n" + QString::fromStdString(e.what()) + 
            "\nBitte prüfen Sie die Backup-Datei: " + currentYamlFile + ".bak");
    }
}

void MainWindow::handleSave() {
    QString trigger = triggerEdit->text().trimmed();
    QString replace = replaceEdit->toPlainText().trimmed();
    
    // Grundlegende Validierung
    if (trigger.isEmpty() || replace.isEmpty()) {
        QMessageBox::warning(this, "Fehler", "Bitte füllen Sie alle Felder aus!");
        return;
    }
    
    // Prüfe auf ungültige Zeichen im Trigger
    QRegularExpression validTrigger("^[a-zA-Z0-9_:]+$");
    if (!validTrigger.match(trigger).hasMatch()) {
        QMessageBox::warning(this, "Fehler", 
            "Der Trigger darf nur Buchstaben, Zahlen, Unterstriche und Doppelpunkte enthalten!");
        return;
    }
    
    // Prüfe Trigger-Länge
    if (trigger.length() < 2) {
        QMessageBox::warning(this, "Fehler", "Der Trigger muss mindestens 2 Zeichen lang sein!");
        return;
    }
    
    // Prüfe ob der Trigger mit : beginnt
    if (!trigger.startsWith(":")) {
        trigger = ":" + trigger;
        triggerEdit->setText(trigger);
    }
    
    if (isEditing) {
        // Bearbeiten-Modus: Finde und aktualisiere den existierenden Eintrag
        auto range = entries.equal_range(currentTrigger);
        bool found = false;
        
        for (auto it = range.first; it != range.second; ++it) {
            if (it.value().replace == currentReplace) {
                // Lösche den alten Eintrag
                it = entries.erase(it);
                found = true;
                break;
            }
        }
        
        if (!found) {
            statusBar()->showMessage("Originaler Eintrag nicht gefunden", 2000);
            return;
        }
        
        // Füge den aktualisierten Eintrag hinzu
        EspansoEntry entry;
        entry.trigger = trigger;
        entry.replace = replace;
        entries.insert(trigger, entry);
        
        statusBar()->showMessage("Eintrag aktualisiert", 2000);
    } else {
        // Neuer Eintrag
        EspansoEntry entry;
        entry.trigger = trigger;
        entry.replace = replace;
        entries.insert(trigger, entry);
        statusBar()->showMessage("Neuer Eintrag hinzugefügt", 2000);
    }
    
    // Speichern und GUI aktualisieren
    saveYamlEntries();
    clearInputFields();
    updateStatusBar();
}

void MainWindow::handleDelete() {
    QString trigger = triggerEdit->text();
    QString replace = replaceEdit->toPlainText();
    
    if (trigger.isEmpty()) {
        statusBar()->showMessage("Bitte wählen Sie einen Eintrag aus", 2000);
        return;
    }
    
    // Finde den exakten Eintrag zum Löschen
    auto range = entries.equal_range(trigger);
    for (auto it = range.first; it != range.second; ++it) {
        if (it.value().replace == replace) {
            entries.erase(it);
            saveYamlEntries();
            clearInputFields();
            updateStatusBar();
            statusBar()->showMessage("Eintrag gelöscht", 2000);
            return;
        }
    }
    
    statusBar()->showMessage("Eintrag nicht gefunden", 2000);
}

void MainWindow::clearInputFields() {
    triggerEdit->clear();
    replaceEdit->clear();
    isEditing = false;
    currentTrigger.clear();
    currentReplace.clear();
}

void MainWindow::updateEntryList() {
    entryList->clear();
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        // Speichere den vollen Text als versteckte Daten
        QString displayText = it.value().trigger + " - " + 
                            it.value().replace.left(30) + 
                            (it.value().replace.length() > 30 ? "..." : "");
        QListWidgetItem* item = new QListWidgetItem(displayText);
        // Speichere den vollständigen replace-Text als Daten
        item->setData(Qt::UserRole, it.value().replace);
        entryList->addItem(item);
    }
}

void MainWindow::openSecondWindow() {
    SecondWindow *secondWindow = new SecondWindow(this);
    secondWindow->exec();
    delete secondWindow;
}

void MainWindow::about() {
    QMessageBox::about(this, "Über",
        "Espanso Helper\n"
        "Ein Tool zum einfachen Verwalten von Espanso-Snippets\n"
        "Version 1.0");
}

void MainWindow::toggleTheme() {
    isDarkTheme = !isDarkTheme;
    if (isDarkTheme) {
        setDarkTheme();
    } else {
        setLightTheme();
    }
}

void MainWindow::setDarkTheme() {
    qApp->setStyleSheet(R"(
        QMainWindow, QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLabel, QStatusBar {
            color: #ffffff;
        }
        QMenuBar {
            background-color: #1a1a1a;
            color: #ffffff;
            border: none;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 10px;
        }
        QMenuBar::item:selected {
            background-color: #404040;
        }
        QMenu {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #404040;
        }
        QMenu::item:selected {
            background-color: #404040;
        }
        QPushButton {
            background-color: #404040;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 5px 10px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QLineEdit, QTextEdit, QComboBox, QListWidget {
            background-color: #353535;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 3px;
        }
        QComboBox::drop-down {
            border: none;
            background-color: #404040;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #ffffff;
        }
    )");
}

void MainWindow::setLightTheme() {
    qApp->setStyleSheet(R"(
        QMainWindow, QDialog {
            background-color: #f0f0f0;
            color: #000000;
        }
        QLabel, QMenuBar, QMenu, QStatusBar {
            color: #000000;
        }
        QPushButton {
            background-color: #e0e0e0;
            color: #000000;
            border: 1px solid #cccccc;
            padding: 5px 10px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #d0d0d0;
        }
        QLineEdit, QTextEdit, QComboBox {
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #cccccc;
            border-radius: 3px;
            padding: 3px;
        }
        QComboBox::drop-down {
            border: none;
            background-color: #e0e0e0;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #000000;
        }
    )");
}

void MainWindow::onComboBoxChanged(const QString &text) {
    loadYamlFile(text);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (titleBar->geometry().contains(event->pos())) {
            dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (!dragPosition.isNull()) {
            move(event->globalPosition().toPoint() - dragPosition);
            event->accept();
        }
    }
}

void MainWindow::setupAutoSave() {
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(5 * 60 * 1000);  // 5 Minuten
    connect(autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (!entries.isEmpty() && !currentYamlFile.isEmpty()) {
            saveYamlEntries();
        }
    });
    autoSaveTimer->start();
}

void MainWindow::restoreFromBackup() {
    // Implementieren Sie die Logik zur Wiederherstellung aus einer Backup-Datei
    // Dies ist eine Platzhalterfunktion und sollte an Ihre spezifischen Anforderungen angepasst werden
    QMessageBox::information(this, "Info", "Backup-Wiederherstellung wird implementiert");
} 