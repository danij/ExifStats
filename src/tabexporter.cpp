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

#include "tabexporter.h"

TABExporter::TABExporter(const QString& fileName)
{
    file.setFileName(fileName);
}

TABExporter::~TABExporter()
{
    if (file.isOpen()) {
        file.close();
    }
}

bool TABExporter::doExport()
{
    if ( ! file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream stream(&file);

    stream << "Exif Statistics\r\n";

    foreach(CollectorInfo info, collectorInfos) {
        stream << info.columnHeader << "\tPictures\t%\r\n";
        for(int i = 0; i < info.collector->count(); i++) {
            stream << (*info.collector)[i]->stringValue << "\t";
            stream << (*info.collector)[i]->pictures << "\t";
            stream << QString::number((*info.collector)[i]->percent, 'f', 2)
                   << "\r\n";
        }
    }

    return true;
}
