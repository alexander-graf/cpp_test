#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QDialog>
#include <QMessageBox>

class SecondWindow : public QDialog {
    Q_OBJECT
public:
    SecondWindow(QWidget *parent = nullptr);
private:
    QVBoxLayout *layout;
    QLabel *label;
    QPushButton *closeButton;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleButton();
    void openSecondWindow();
    void about();
    void toggleTheme();
    
private:
    void createMenus();
    void createLayout();
    void setDarkTheme();
    void setLightTheme();

    QPushButton *button;
    QPushButton *secondWindowButton;
    QLabel *label;
    QLineEdit *textfeld;
    QWidget *centralWidget;
    bool isDarkTheme;
};

#endif 