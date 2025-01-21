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
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <yaml-cpp/yaml.h>
#include <QMultiMap>
#include <QPoint>
#include <QMouseEvent>
#include <QTimer>

class SecondWindow : public QDialog {
    Q_OBJECT
public:
    explicit SecondWindow(QWidget *parent = nullptr);
private:
    QVBoxLayout *layout;
    QLabel *label;
    QPushButton *closeButton;
};

struct EspansoEntry {
    QString trigger;
    QString replace;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleSave();
    void handleDelete();
    void openSecondWindow();
    void about();
    void toggleTheme();
    void loadYamlFile(const QString &filename);
    void onComboBoxChanged(const QString &text);
    void openConfigFolder();
    void refreshYamlFiles();
    
private:
    void createMenus();
    void createLayout();
    void setDarkTheme();
    void setLightTheme();
    void loadYamlEntries();
    void saveYamlEntries();
    QString getEspansoConfigPath();
    void updateEntryList();
    void clearInputFields();
    void createTitleBar();
    void updateStatusBar();
    void restoreFromBackup();

    bool isDarkTheme;
    bool isEditing;
    QString currentTrigger;
    QString currentReplace;
    QWidget *centralWidget;
    QLabel *titleLabel;
    QComboBox *yamlComboBox;
    QLineEdit *triggerEdit;
    QTextEdit *replaceEdit;
    QPushButton *saveButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *secondWindowButton;
    QPushButton *helpButton;
    QPushButton *clearButton;
    QPushButton *minimizeButton;
    QPushButton *closeButton;

    QMultiMap<QString, EspansoEntry> entries;
    QString currentYamlFile;
    QListWidget *entryList;

    QPoint dragPosition;
    QWidget *titleBar;
    QLabel *windowTitle;

    QTimer *autoSaveTimer;
    void setupAutoSave();
    void createBackup();

    QLabel *statusEntryCount;
    QLabel *statusFileName;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif 