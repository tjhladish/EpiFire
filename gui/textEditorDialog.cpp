#include "textEditorDialog.h"

TextEditorDialog::TextEditorDialog(MainWindow* w, QString title) {
    mw = w;
    this->setWindowTitle(title);

    codeEditor = new CodeEditor();
    QPushButton* loadButton = new QPushButton("Load",this);
    QPushButton* saveButton = new QPushButton("Save",this);
    QPushButton* generateButton = new QPushButton("Generate network",this);
    QPushButton* closeButton = new QPushButton("Close", this);

    connect(loadButton,  SIGNAL(clicked()), this, SLOT(load()));
    connect(saveButton,  SIGNAL(clicked()), this, SLOT(save()));
    connect(generateButton, SIGNAL(clicked()), this, SLOT(generate()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* buttonBoxLayout   = new QHBoxLayout();
    QWidget* buttonBox = new QWidget();
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(loadButton);
    buttonBoxLayout->addWidget(saveButton);
    buttonBoxLayout->addWidget(generateButton);
    buttonBoxLayout->addWidget(closeButton);
    buttonBox->setLayout(buttonBoxLayout);

    QVBoxLayout* vLayout   = new QVBoxLayout();
    vLayout->addWidget(codeEditor);
    vLayout->addWidget(buttonBox);
    QStatusBar* _statusBar = new QStatusBar(this);
    vLayout->addWidget(_statusBar);

    this->setLayout(vLayout);
    _statusBar->showMessage("Enter degree distribution");
}


void TextEditorDialog::load() {
    QString startdir = ".";
    QString fileName = QFileDialog::getOpenFileName(
            this, "Select degree distribution file to load:", startdir, "Plain text (*.txt)(*.txt)");
    QFile data(fileName);
    if (data.open(QFile::ReadOnly)) {
        QTextStream istream(&data);
        QString line = istream.readAll();
        codeEditor->appendPlainText(line); 
    }
}


void TextEditorDialog::save() {
}


void TextEditorDialog::generate() {
    QStringList dist_str = codeEditor->toPlainText().split(QRegExp("\n|\r\n|\r"));
    vector<double> dist;
    for (auto str: dist_str) {
        bool ok;
        double val = str.toDouble(&ok);
        if (ok) {
            dist.push_back(val);
        } else {
            dist.push_back(0.0);
        }
        cerr << str.toStdString() << ", " << val << endl; 
    }

for (unsigned int i = 0; i<dist.size(); ++i) cerr << "deg, un-normed freq: " << i << ", " << dist[i] << endl;
    vector<double> normed_dist = normalize_dist(dist);

for (unsigned int i = 0; i<normed_dist.size(); ++i) cerr << "deg, freq: " << i << ", " << normed_dist[i] << endl;

    if(mw->network) { delete(mw->network); }
    mw->netComponents.clear();
    int netSize = mw->numnodesLine->text().toInt();

    setCursor(Qt::WaitCursor);
    mw->appendOutputLine("Generating network from user-provided degree distribution . . . ");
    mw->network = new Network("mynetwork", Network::Undirected);
    mw->network->populate(netSize);
    mw->network->rand_connect_user(normed_dist);
    //network->dumper();
    //network->validate();
    //netfileLine->setText(fileName);
    //numnodesLine->setText(QString::number(network->size()));
    mw->distBox->setCurrentIndex(6);
    mw->netDoneUpdate(true);
cerr << "done\n";
cerr << "netsize:" << mw->network->size() << endl;
    close();
}

