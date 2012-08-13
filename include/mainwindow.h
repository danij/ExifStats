//ExifStats - Statistics
//Copyright (C) 2010-2012 Dani J

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QFileDialog>
#include <QMutex>
#include "collector.h"
#include "entryreader.h"
#include "makefilter.h"
#include "modelfilter.h"
#include "htmlexporter.h"
#include "tabexporter.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QTimer* progressTimer;
    Collector* focalLength;
    Collector* focalLength35;
    Collector* aperture;
    Collector* iso;
    Collector* exposureTime;
    Collector* camera;
    EntryReader* reader;
    MakeFilter* makeFilter;
    ModelFilter* modelFilter;
    int totalPictures;
    int counter;

    void showResults();

private slots:
    void on_btnExport_clicked();
    void on_btnClose_clicked();
    void on_btnBrowse_clicked();
    void on_btnStart_clicked();
    void progressTimer_timeout();
};

#endif // MAINWINDOW_H
