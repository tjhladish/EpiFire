#ifndef TEXTEDITDIALOG
#define TEXTEDITDIALOG

#include <QDialog>
#include "mainWindow.h"
#include "codeeditor.h"

class MainWindow;

class TextEditorDialog: public QDialog {
    Q_OBJECT

    friend class MainWindow;

    public:
        TextEditorDialog(MainWindow* w, QString title);

    public slots:

    private:
        CodeEditor* codeEditor;
        QPushButton* saveButton;
        QPushButton* loadButton;
};

#endif
