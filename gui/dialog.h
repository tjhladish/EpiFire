 #ifndef DIALOG_H
 #define DIALOG_H

#include <QDialog>
#include <iostream>
#include "plotarea.h"

class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QComboBox;
class QCheckBox;
class PlotArea;

class Dialog : public QDialog
{
     Q_OBJECT

 public:
  Dialog();
  void appendOutput(QString); 
   
  QTextEdit *bigEditor;

  public slots:

  void simWindow();

 private:
  
  void createMenu();
  void createHorizontalGroupBox();
  void createGridGroupBox();
  void createFormGroupBox();
   
  enum { NumGridRows = 6, NumButtons = 4 };

  QMenuBar *menuBar;
  QGroupBox *horizontalGroupBox;
  QGroupBox *gridGroupBox;
  QGroupBox *formGroupBox;
  QTextEdit *smallEditor;
  QComboBox *distBox;
  QCheckBox *reuseCheckBox;
  QTextEdit *texttest; // Get rid of this one
  QLabel *labels[NumGridRows];
  QLineEdit *lineEdits[NumGridRows];
  QPushButton *buttons[NumButtons];
  QDialogButtonBox *buttonBox;

  // Define textboxes and other main menu items

  QLineEdit *numrunsLine;
  QLineEdit *numnodesLine;
  QLineEdit *param1Line;
  QLineEdit *param2Line;
  QLineEdit *pzeroLine;
  QLineEdit *rzeroLine;

  PlotArea* plotArea;
  
  QMenu *fileMenu;
  QAction *exitAction;
};

 #endif
