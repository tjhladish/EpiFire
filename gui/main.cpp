#include <QApplication>
#include <main.h>
#include <dialog.h>


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  Dialog dialog;
  return dialog.exec();
}

