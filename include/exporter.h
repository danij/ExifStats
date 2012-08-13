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

#ifndef EXPORTER_H
#define EXPORTER_H

#include <QString>
#include <QList>
#include "collector.h"

typedef struct {
    QString name;
    QString columnHeader;
    Collector* collector;
} CollectorInfo;

class Exporter {
public:
    void addCollector(const QString& name,
                      const QString& columnHeader,
                      Collector* collector)
    {
        CollectorInfo info;
        info.name = name;
        info.columnHeader = columnHeader;
        info.collector = collector;

        collectorInfos.push_back(info);
    }

    virtual bool doExport() = 0;
protected:
    QList<CollectorInfo> collectorInfos;
};

#endif // EXPORTER_H
