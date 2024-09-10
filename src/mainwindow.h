#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   protected:
    void keyPressEvent(QKeyEvent *event) override;

   private slots:
    void openFile();
    void saveFile();
    void updateStatusBar();

   private:
    QString currentFile;
    QTextEdit *textEdit;
    QLabel *filePathLabel;
    QLabel *fileSizeLabel;
    QLabel *lineCountLabel;
    QLabel *fileSaveState;

    void updateFileInfo(const QString &filePath);
    void _openFile(const QString &filePath);
};

#endif  // MAINWINDOW_H
