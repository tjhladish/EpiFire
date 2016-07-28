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
        void load();
        void save();
        void generate();

    private:
        MainWindow*  mw;
        CodeEditor* codeEditor;
        QStatusBar* _statusBar;
        QPushButton* loadButton;
        QPushButton* saveButton;
        QPushButton* generateButton;
        QPushButton* closeButton;
};

#endif
