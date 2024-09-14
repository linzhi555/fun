#include "editorwidget.h"

#include <QDebug>
#include <QKeyEvent>
EditorWidgt::EditorWidgt(QWidget *parent) : QTextEdit(parent) {
}

EditorWidgt::~EditorWidgt() {
}

void EditorWidgt::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Q && event->modifiers() & Qt::ControlModifier) {
        qDebug() << "ctrl-q pressed";
        qDebug() << this->document()->toRawText();
    }

    QTextEdit::keyPressEvent(event);
}
