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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), reader(0), makeFilter(0), modelFilter(0)
{
    ui->setupUi(this);
    aboutDialog = new AboutDialog(this);


    progressTimer = new QTimer(this);
    focalLength = new Collector();
    focalLength35 = new Collector();
    aperture = new Collector();
    iso = new Collector();
    exposureTime = new Collector();
    camera = new Collector();

    connect(progressTimer, SIGNAL(timeout()), this, SLOT(progressTimer_timeout()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete progressTimer;
    delete focalLength;
    delete focalLength35;
    delete aperture;
    delete iso;
    delete exposureTime;
    delete camera;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool compareExtension(const QString& extension, const QStringList& filters) {
    for (int i = 0; i < filters.count(); i++) {
        if (extension.compare(filters.at(i), Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

void MainWindow::on_btnStart_clicked()
{
    if (ui->txtFolderName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, windowTitle(), "No folder was specified");
        return;
    }
    QDir folder(ui->txtFolderName->text());
    if ( ! folder.exists()) {
        QMessageBox::warning(this, windowTitle(), "Invalid Folder");
        return;
    }
    QStringList extensionFilters;

    if (ui->chkJPEG->isChecked()) {
        extensionFilters << "jpg" << "jpeg";
    }
    if (ui->chkTIFF->isChecked()) {
        extensionFilters << "tif" << "tiff";
    }
    if (ui->chkRAW->isChecked()) {
        extensionFilters << "3rf" << "ari" << "arw" << "srf";
        extensionFilters << "sr2" << "bay" << "crw" << "cr2";
        extensionFilters << "cap" << "iiq" << "eip" << "dcs";
        extensionFilters << "dcr" << "drf" << "k25" << "kdc";
        extensionFilters << "dng" << "erf" << "fff" << "mef";
        extensionFilters << "mos" << "mrw" << "nef" << "nrw";
        extensionFilters << "orf" << "ptx" << "pef" << "pxn";
        extensionFilters << "r3d" << "raf" << "raw" << "rw2";
        extensionFilters << "rwl" << "rwz" << "x3f";
    }

    QStringList all;

    QDirIterator iterator (
        ui->txtFolderName->text(),
        all,
        QDir::Dirs | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories
    );

    QStringList files;
    QFileInfoList infoList;
    QString make = ui->txtMake->text();
    QString model = ui->txtModel->text();
    QStringList makeList;
    QStringList modelList;

    while (make.indexOf("  ") != -1) {
        make.replace("  "," ");
    }
    while (model.indexOf("  ") != -1) {
        model.replace("  "," ");
    }

    make.replace(";",",").replace(" ,",",").replace(", ",",");
    model.replace(";",",").replace(" ,",",").replace(", ",",");

    make = make.trimmed();
    model = model.trimmed();

    makeList = make.split(',', QString::SkipEmptyParts);
    modelList = model.split(',', QString::SkipEmptyParts);

    this->setCursor(Qt::WaitCursor);

    infoList << folder.entryInfoList(all, QDir::Files | QDir::Hidden);
    while (iterator.hasNext()) {
        folder.setPath(iterator.next());
        infoList << folder.entryInfoList(all, QDir::Files | QDir::Hidden);
    }

    this->setCursor(Qt::ArrowCursor);

    for (int i = 0; i < infoList.count(); i++) {
        if (compareExtension(infoList.at(i).suffix(), extensionFilters)) {
            files << infoList.at(i).absoluteFilePath();
        }
    }
    totalPictures = files.count();

    if (totalPictures < 1) {
        QMessageBox::warning(this, windowTitle(), "No files were found");
        return;
    }
    else {
        ui->btnStart->setEnabled(false);
        ui->btnExport->setEnabled(false);
        ui->progressBar->setValue(0);
        focalLength->clear();
        focalLength35->clear();
        aperture->clear();
        iso->clear();
        exposureTime->clear();
        camera->clear();
        counter = 0;

        reader = new EntryReader();
        reader->setData(&counter,files);
        if (makeList.count() > 0) {
            makeFilter = new MakeFilter(makeList);
            makeFilter->SetExcept(ui->chkExceptMake->isChecked());
            reader->addFilter(makeFilter);
        }
        if (modelList.count() > 0) {
            modelFilter = new ModelFilter(modelList);
            modelFilter->SetExcept(ui->chkExceptModel->isChecked());
            reader->addFilter(modelFilter);
        }
        reader->addCollector(FOCALLENGTH, focalLength);
        reader->addCollector(FOCALLENGTH35, focalLength35);
        reader->addCollector(APERTURE, aperture);
        reader->addCollector(ISO, iso);
        reader->addCollector(EXPOSURETIME, exposureTime);
        reader->addCollector(CAMERA, camera);
        progressTimer->setInterval(100);
        progressTimer->start();
        reader->start();
    }
}

void MainWindow::on_btnBrowse_clicked()
{
    ui->txtFolderName->setText(
        QFileDialog::getExistingDirectory(
            this,
            "Select Folder",
            ui->txtFolderName->text()
        )
    );
}

void MainWindow::progressTimer_timeout()
{
    float percent = static_cast<float>(counter * 100) /
        static_cast<float>(this->totalPictures);

    QString totalPictures = QString::number(counter);
    if (counter < this->totalPictures) {
        ui->progressBar->setValue(percent * 100);
        ui->lblPicturesFound->setText(totalPictures);
    }
    else {
        reader->wait();
        ui->btnStart->setEnabled(true);
        ui->btnExport->setEnabled(true);
        progressTimer->stop();
        ui->progressBar->setValue(10000);
        ui->lblPicturesFound->setText(totalPictures);
        if (makeFilter != 0) {
            delete makeFilter;
        }
        if (modelFilter != 0) {
            delete modelFilter;
        }
        delete reader;
        showResults();
    }
}

void MainWindow::showResults()
{
    ui->treeFocal->clear();
    ui->treeFocalSorted->clear();
    ui->treeFocal35->clear();
    ui->treeFocal35Sorted->clear();
    ui->treeAperture->clear();
    ui->treeApertureSorted->clear();
    ui->treeISO->clear();
    ui->treeISOSorted->clear();
    ui->treeExposure->clear();
    ui->treeExposureSorted->clear();
    ui->treeCamera->clear();
    ui->treeCameraSorted->clear();

    QTreeWidgetItem *item;
    QFont font;
    font.setBold(true);

    focalLength->sortByValue();
    for (int i = 0; i < focalLength->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*focalLength)[i]->stringValue));
        item->setText(1, QString::number((*focalLength)[i]->pictures));
        item->setText(2, QString::number((*focalLength)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeFocal->addTopLevelItem(item);
    }

    focalLength->sortByCount();
    for (int i = 0; i < focalLength->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*focalLength)[i]->stringValue));
        item->setText(1, QString::number((*focalLength)[i]->pictures));
        item->setText(2, QString::number((*focalLength)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeFocalSorted->addTopLevelItem(item);
    }

    focalLength35->sortByValue();
    for (int i = 0; i < focalLength35->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*focalLength35)[i]->stringValue));
        if (item->text(0) == "0") {
            item->setText(0, "Unknown");
        }
        item->setText(1, QString::number((*focalLength35)[i]->pictures));
        item->setText(2, QString::number((*focalLength35)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeFocal35->addTopLevelItem(item);
    }

    focalLength35->sortByCount();
    for (int i = 0; i < focalLength35->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*focalLength35)[i]->stringValue));
        if (item->text(0) == "0") {
            item->setText(0, "Unknown");
        }
        item->setText(1, QString::number((*focalLength35)[i]->pictures));
        item->setText(2, QString::number((*focalLength35)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeFocal35Sorted->addTopLevelItem(item);
    }

    aperture->sortByValue();
    for (int i = 0; i < aperture->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*aperture)[i]->stringValue));
        item->setText(1, QString::number((*aperture)[i]->pictures));
        item->setText(2, QString::number((*aperture)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeAperture->addTopLevelItem(item);
    }

    aperture->sortByCount();
    for (int i = 0; i < aperture->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*aperture)[i]->stringValue));
        item->setText(1, QString::number((*aperture)[i]->pictures));
        item->setText(2, QString::number((*aperture)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeApertureSorted->addTopLevelItem(item);
    }

    iso->sortByValue();
    for (int i = 0; i < iso->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*iso)[i]->stringValue));
        item->setText(1, QString::number((*iso)[i]->pictures));
        item->setText(2, QString::number((*iso)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeISO->addTopLevelItem(item);
    }

    iso->sortByCount();
    for (int i = 0; i < iso->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*iso)[i]->stringValue));
        item->setText(1, QString::number((*iso)[i]->pictures));
        item->setText(2, QString::number((*iso)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeISOSorted->addTopLevelItem(item);
    }

    exposureTime->sortByValue();
    for (int i = 0; i < exposureTime->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*exposureTime)[i]->stringValue));
        item->setText(1, QString::number((*exposureTime)[i]->pictures));
        item->setText(2, QString::number((*exposureTime)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeExposure->addTopLevelItem(item);
    }

    exposureTime->sortByCount();
    for (int i = 0; i < exposureTime->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*exposureTime)[i]->stringValue));
        item->setText(1, QString::number((*exposureTime)[i]->pictures));
        item->setText(2, QString::number((*exposureTime)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeExposureSorted->addTopLevelItem(item);
    }

    camera->sortByValue();
    for (int i = 0; i < camera->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*camera)[i]->stringValue));
        item->setText(1, QString::number((*camera)[i]->pictures));
        item->setText(2, QString::number((*camera)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeCamera->addTopLevelItem(item);
    }

    camera->sortByCount();
    for (int i = 0; i < camera->count(); i++) {
        item = new QTreeWidgetItem();
        item->setText(0, QString((*camera)[i]->stringValue));
        item->setText(1, QString::number((*camera)[i]->pictures));
        item->setText(2, QString::number((*camera)[i]->percent, 'f', 2));
        item->setFont(0, font);
        item->setTextAlignment(0, Qt::AlignLeft);
        item->setTextAlignment(1, Qt::AlignHCenter);
        item->setTextAlignment(2, Qt::AlignHCenter);

        ui->treeCameraSorted->addTopLevelItem(item);
    }

}

void MainWindow::on_btnClose_clicked()
{
    exit(0);
}

void MainWindow::on_btnExport_clicked()
{
    QString fileName;

    if (focalLength->count() < 1) {
        QMessageBox::warning(this, windowTitle(), "Nothing to export");
        return;
    }

    fileName = QFileDialog::getSaveFileName(
        this,
        "Save Stats",
        "",
        "HTML File (*.htm);;TAB Separated (*.txt)"
    );

    if (fileName.length() < 3) {
        return;
    }

    Exporter* exporter;

    if (fileName.endsWith(QString(".htm"), Qt::CaseInsensitive)) {
        exporter = new HTMLExporter(fileName);
    }
    else {
        exporter = new TABExporter(fileName);
    }

    exporter->addCollector("Focal Length", "mm", focalLength);
    exporter->addCollector("Focal Length (35mm equivalent)", "mm", focalLength35);
    exporter->addCollector("Aperture", "f/", aperture);
    exporter->addCollector("ISO", "ISO", iso);
    exporter->addCollector("Exposure Time", "Seconds", exposureTime);
    exporter->addCollector("Camera", "Make & Model", camera);

    if (exporter->doExport()) {
        QMessageBox::information(this, windowTitle(),
                                 "The export has been completed");
    }
    else {
        QMessageBox::information(this, windowTitle(),
                                 "Could not open " + fileName);
    }

    delete exporter;
}

void MainWindow::on_btnAbout_clicked()
{    
    aboutDialog->show();
}
