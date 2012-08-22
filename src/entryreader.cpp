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

#include "entryreader.h"
#include "exiv2/exiv2.hpp"

#include <QMessageBox>
#include <QDebug>

using namespace std;

void EntryReader::setData(int *counter, const QStringList &files)
{
    this->counter = counter;
    this->files = files;
    *counter = 0;
}

void EntryReader::addCollector(CollectorType type, Collector *collector)
{
    if (collectors.find(type) == collectors.end()) {
        collectors.insert(make_pair(type, collector));
    }
}

void EntryReader::addFilter(IFilter *filter)
{
    filters.push_back(filter);
}

float get35mmFocalMultiplier(const QString& make, const QString& model)
{
    if (make.indexOf(QString("OLYMPUS"), 0, Qt::CaseInsensitive) >= 0) {
        if (model.startsWith("E-",Qt::CaseInsensitive)) {
            return 2;
        }
    }

    if (make.indexOf(QString("CANON"), 0, Qt::CaseInsensitive) >= 0) {
        if (model.indexOf(QString("EOS"), 0 ,Qt::CaseInsensitive) >= 0) {
            QStringList canonSplit = QString(model).replace('-'," ").split(' ');
            if (canonSplit.size() > 2) {
                if (canonSplit[2].startsWith("5D", Qt::CaseInsensitive)) {
                    return 1;
                }
                if (canonSplit[2].startsWith("1Ds", Qt::CaseInsensitive)) {
                    return 1;
                }
                if (canonSplit[2].startsWith("1D", Qt::CaseInsensitive)) {
                    return 1.3;
                }
                return 1.6;
            }
        }
    }

    return 0;
}

void EntryReader::run()
{
    float focalLength;
    float focalLength35;
    float fNumber;
    float iso;
    float exposureTime;
    bool match;
    QString make;
    QString model;

    foreach(QString file, files) {
        qDebug() << file;

        try {
            Exiv2::Image::AutoPtr image =
                Exiv2::ImageFactory::open(file.toStdString());

            if (image.get() == 0) {
                continue;
            }

            image->readMetadata();
            Exiv2::ExifData &exifData = image->exifData();

            match = true;

            try {
                make = QString(exifData["Exif.Image.Make"].toString().c_str());
                model = QString(exifData["Exif.Image.Model"].toString().c_str());
            }
            catch(Exiv2::AnyError& e) {}

            if (filters.size() > 0) {
                foreach(IFilter* filter, filters) {
                    if ( ! filter->match(exifData)) {
                        match = false;
                        break;
                    }
                }
            }

            if ( ! match) {
                *counter += 1;
                continue;
            }

            focalLength = 0;

            try {
                focalLength = exifData["Exif.Photo.FocalLength"].toFloat();
            }
            catch (Exiv2::AnyError& e) {}

            focalLength35 = 0;

            try {
                if (exifData["Exif.Photo.FocalLengthIn35mmFilm"].size() > 0) {
                    focalLength35 =
                        exifData["Exif.Photo.FocalLengthIn35mmFilm"].toFloat();
                }
                if (focalLength35 < 0) {
                    focalLength35 = 0;
                }
            }
            catch (Exiv2::AnyError& e) {}

            if (focalLength35 < 1) {
                focalLength35 = focalLength *
                        get35mmFocalMultiplier(make, model);
            }

            fNumber = 0;

            try {
                fNumber = exifData["Exif.Photo.FNumber"].toFloat();
            }
            catch (Exiv2::AnyError& e) {}

            iso = 0;
            try {
                iso = exifData["Exif.Photo.ISOSpeedRatings"].toFloat();
            }
            catch (Exiv2::AnyError& e) {}

            exposureTime = 0;
            try {
                exposureTime = exifData["Exif.Photo.ExposureTime"].toFloat();
            }
            catch (Exiv2::AnyError& e) {}

            if (focalLength35 < 0.1) {
                focalLength35 = 0;
            }

            if ((focalLength > 0.1) && (fNumber > 0.1) &&
                    (iso > 0) && (exposureTime > 0)) {
                if (collectors.find(FOCALLENGTH) != collectors.end()) {
                    collectors[FOCALLENGTH]->addValue(focalLength);
                }
                if (collectors.find(FOCALLENGTH35) != collectors.end()) {
                    collectors[FOCALLENGTH35]->addValue(focalLength35);
                }
                if (collectors.find(APERTURE) != collectors.end()) {
                    collectors[APERTURE]->addValue(fNumber);
                }
                if (collectors.find(ISO) != collectors.end()) {
                    collectors[ISO]->addValue(iso);
                }
                if (collectors.find(EXPOSURETIME) != collectors.end()) {
                    collectors[EXPOSURETIME]->addExposureValue(exposureTime);
                }
                if (collectors.find(CAMERA) != collectors.end()) {
                    collectors[CAMERA]->addValue(make + " " + model);
                }
            }
        }
        catch (...) { }

        *counter += 1;
    }

    for (map<CollectorType, Collector*>::iterator iterator = collectors.begin();
         iterator != collectors.end(); iterator++) {
        iterator->second->calculatePercentage();
    }
}
