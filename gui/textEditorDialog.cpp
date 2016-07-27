#include "textEditorDialog.h"

TextEditorDialog::TextEditorDialog(MainWindow* w, QString title) {

    this->setWindowTitle(title);

    codeEditor = new CodeEditor();
    QPushButton* saveButton = new QPushButton("Save",this);
    QPushButton* loadButton = new QPushButton("Load",this);
    QPushButton* closeButton = new QPushButton("Close", this);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(loadButton,  SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton,  SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* buttonBoxLayout   = new QHBoxLayout();
    QWidget* buttonBox = new QWidget();
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(saveButton);
    buttonBoxLayout->addWidget(loadButton);
    buttonBoxLayout->addWidget(closeButton);
    buttonBox->setLayout(buttonBoxLayout);

    QVBoxLayout* vLayout   = new QVBoxLayout();
    vLayout->addWidget(codeEditor);
    vLayout->addWidget(buttonBox);

    this->setLayout(vLayout);

}

