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

#include "htmlexporter.h"

HTMLExporter::HTMLExporter(const QString& fileName)
{
    file.setFileName(fileName);
}

HTMLExporter::~HTMLExporter()
{
    if (file.isOpen()) {
        file.close();
    }
}

bool HTMLExporter::doExport()
{
    if ( ! file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream stream(&file);

    stream << "<html>\r\n";
    stream << "<style type=\"text/css\">\r\n";
    stream << "table {border-width: 1px; border-style: solid; border-collapse: collapse;}\r\n";
    stream << "th {width: 5em; border-width: 1px; border-style: solid;}\r\n";
    stream << "td {width: 5em; border-width: 1px; border-style: dotted;}\r\n";
    stream << "</style>\r\n";
    stream << "<h1>Exif Statistics</h1>\r\n";

    foreach(CollectorInfo info, collectorInfos) {
        stream << "<h2>" << info.name << "</h2>\r\n";
        stream << "<table><tr><th>" << info.columnHeader
               << "</th><th>Pictures</th><th>%</th></tr>\r\n";
        for(int i = 0; i < info.collector->count(); i++) {
            stream << "<tr><td><b>"
                   << (*info.collector)[i]->stringValue << "</b></td>\r\n";
            stream << "<td align=\"center\">"
                   << (*info.collector)[i]->pictures << "</td>\r\n";
            stream << "<td align=\"center\">"
                   << QString::number((*info.collector)[i]->percent,'f',2)
                   << "</td></tr>\r\n";
        }
        stream << "</table>\r\n";
    }

    stream << "</html>";

    return true;
}
