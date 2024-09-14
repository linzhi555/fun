#pragma once

#include <QTextEdit>
class EditorWidgt : public QTextEdit {
   protected:
    void keyPressEvent(QKeyEvent *event) override;

   public:
    explicit EditorWidgt(QWidget *parent = nullptr);
    ~EditorWidgt();
};
