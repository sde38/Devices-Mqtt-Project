#ifndef MAINWINDOWS_H
#define MAINWINDOWS_H

#include <QMainWindow>
#include "DevicesWidget.h"

class MainWindows : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindows(QWidget *parent = nullptr);
};

#endif // MAINWINDOWS_H