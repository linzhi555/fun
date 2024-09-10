#include "mainwindow.h"
#include "config.h"

#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      textEdit(new QTextEdit(this)),
      filePathLabel(new QLabel(this)),
      fileSizeLabel(new QLabel(this)),
      lineCountLabel(new QLabel(this)),
      fileSaveState(new QLabel(this)),
      currentFile("") {
    setCentralWidget(textEdit);

    // 设置状态栏
    statusBar()->addWidget(filePathLabel);
    statusBar()->addWidget(fileSizeLabel);
    statusBar()->addWidget(lineCountLabel);
    statusBar()->addWidget(fileSaveState);

    QMenu *fileMenu = menuBar()->addMenu("文件");

    QAction *openAction = new QAction("打开", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);

    QAction *saveAction = new QAction("保存", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->addAction(saveAction);

    QAction *exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    setStyleSheet("background-color: #FFFACD;");

    // 设置初始大小
    resize(800, 600);  // 设置窗口初始大小为 800x600

    // 更新初始状态
    updateStatusBar();

    // 连接文本更改信号
    connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::updateStatusBar);
    _openFile(fun::config::inputFiles.first());
}

MainWindow::~MainWindow() {}

void MainWindow::_openFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        currentFile = filePath;
        textEdit->setPlainText(file.readAll());
        file.close();
        updateFileInfo(filePath);
    }
}

void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "打开文件");
    if (!filePath.isEmpty()) { _openFile(filePath); }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // 检测 Ctrl + S
    if (event->key() == Qt::Key_S && event->modifiers() & Qt::ControlModifier) { saveFile(); }
    QMainWindow::keyPressEvent(event);  // 调用基类处理其他按键
}

void MainWindow::saveFile() {
    if (currentFile != "") {
        QFile file(currentFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(textEdit->toPlainText().toUtf8());
            file.close();
            updateFileInfo(currentFile);
            fileSaveState->setText("written");
        }
    }
}

void MainWindow::updateStatusBar() {
    QString text = textEdit->toPlainText();
    int lineCount = text.isEmpty() ? 0 : text.count('\n') + 1;  // 计算行数
    lineCountLabel->setText(QString("行数: %1").arg(lineCount));
}

void MainWindow::updateFileInfo(const QString &filePath) {
    filePathLabel->setText(QString("%1").arg(filePath));

    QFile file(filePath);
    if (file.exists()) {
        fileSizeLabel->setText(QString("大小: %1 字节").arg(file.size()));
    } else {
        fileSizeLabel->setText("大小: 0 字节");
    }

    updateStatusBar();  // 更新行数信息
}
