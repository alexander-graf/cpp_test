#include "mainwindow.h"
#include <QApplication>
#include <QStyle>

SecondWindow::SecondWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Zweites Fenster");
    layout = new QVBoxLayout(this);
    
    label = new QLabel("Dies ist ein neues Fenster!", this);
    closeButton = new QPushButton("Schließen", this);
    
    layout->addWidget(label);
    layout->addWidget(closeButton);
    
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    setMinimumSize(250, 150);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isDarkTheme(false)
{
    createMenus();
    createLayout();
    setMinimumSize(400, 300);
    setWindowTitle("Erweiterte GUI-Anwendung");
    statusBar()->showMessage("Bereit");
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("&Datei");
    QMenu *viewMenu = menuBar()->addMenu("&Ansicht");
    QMenu *helpMenu = menuBar()->addMenu("&Hilfe");
    
    fileMenu->addAction("&Beenden", this, &QWidget::close);
    
    QAction *themeAction = viewMenu->addAction("Dark Mode");
    themeAction->setCheckable(true);
    connect(themeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    
    helpMenu->addAction("Ü&ber", this, &MainWindow::about);
}

void MainWindow::createLayout() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    label = new QLabel("Willkommen bei der erweiterten GUI-App!", this);
    label->setAlignment(Qt::AlignCenter);
    
    textfeld = new QLineEdit(this);
    textfeld->setPlaceholderText("Geben Sie hier Text ein...");
    
    button = new QPushButton("Klick mich!", this);
    secondWindowButton = new QPushButton("Neues Fenster öffnen", this);
    
    buttonLayout->addWidget(button);
    buttonLayout->addWidget(secondWindowButton);
    
    mainLayout->addWidget(label);
    mainLayout->addWidget(textfeld);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    connect(button, &QPushButton::clicked, this, &MainWindow::handleButton);
    connect(secondWindowButton, &QPushButton::clicked, this, &MainWindow::openSecondWindow);
}

void MainWindow::handleButton() {
    QString text = textfeld->text();
    if (!text.isEmpty()) {
        label->setText("Sie haben eingegeben: " + text);
        statusBar()->showMessage("Text aktualisiert", 2000);
    } else {
        label->setText("Bitte geben Sie etwas ein!");
        statusBar()->showMessage("Keine Eingabe", 2000);
    }
}

void MainWindow::openSecondWindow() {
    SecondWindow *secondWindow = new SecondWindow(this);
    secondWindow->exec();
    delete secondWindow;
}

void MainWindow::about() {
    QMessageBox::about(this, "Über",
        "Eine erweiterte Qt GUI-Anwendung\n"
        "Mit Menüs, Dialogen und Themes\n"
        "Version 1.0");
}

void MainWindow::setDarkTheme() {
    qApp->setStyleSheet(R"(
        QMainWindow, QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLabel, QMenuBar, QMenu, QStatusBar {
            color: #ffffff;
        }
        QPushButton {
            background-color: #404040;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 5px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QLineEdit {
            background-color: #404040;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 5px;
            border-radius: 3px;
        }
        QMenu {
            background-color: #2b2b2b;
            border: 1px solid #555555;
        }
        QMenu::item:selected {
            background-color: #404040;
        }
        QMenuBar {
            background-color: #2b2b2b;
        }
        QMenuBar::item:selected {
            background-color: #404040;
        }
    )");
}

void MainWindow::setLightTheme() {
    qApp->setStyleSheet("");
}

void MainWindow::toggleTheme() {
    isDarkTheme = !isDarkTheme;
    if (isDarkTheme) {
        setDarkTheme();
    } else {
        setLightTheme();
    }
} 