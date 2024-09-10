#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

namespace fun::config {

QVector<QString> inputFiles;

void init(QApplication *app) {
    // 创建命令行解析器
    QCommandLineParser parser;

    // 设置程序名称
    parser.setApplicationDescription("命令行参数解析示例");
    parser.addHelpOption();
    parser.addVersionOption();

    // 添加自定义选项
    QCommandLineOption optionA("a", "选项 A");
    QCommandLineOption optionB("b", "选项 B");
    parser.addOption(optionA);
    parser.addOption(optionB);

    // 添加参数
    parser.addPositionalArgument("input", "输入文件路径");

    // 解析命令行
    parser.process(*app);

    // 处理选项
    if (parser.isSet(optionA)) { qDebug() << "选项 A 被设置"; }
    if (parser.isSet(optionB)) { qDebug() << "选项 B 被设置"; }

    // 处理位置参数
    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        QString temp(args.first());
        inputFiles.append(temp);
        qDebug() << "输入文件路径:" << args.first();
    }
}
}  // namespace fun::config
