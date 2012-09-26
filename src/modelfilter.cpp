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

#include "modelfilter.h"

ModelFilter::ModelFilter(const QStringList& values) : list(values)
{
}

ModelFilter::~ModelFilter()
{
}

bool ModelFilter::match(Exiv2::ExifData &exifData)
{
    QString model;
    try {
        model = QString(exifData["Exif.Image.Model"].toString().c_str());
    }
    catch(Exiv2::AnyError& e) {
        return false;
    }

    if (list.count() > 0) {
        foreach(QString item, list) {
            if (model.indexOf(item, 0, Qt::CaseInsensitive) != -1) {
                return (except ? false : true);
                break;
            }
        }
    }

    return (except ? true : false);
}
