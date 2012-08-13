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

#ifndef ENTRYREADER_H
#define ENTRYREADER_H

#include <QThread>
#include <QMutex>
#include <QStringList>
#include <map>
#include "collector.h"
#include "ifilter.h"

enum CollectorType {
    FOCALLENGTH,
    FOCALLENGTH35,
    APERTURE,
    ISO,
    EXPOSURETIME,
    CAMERA
};

class EntryReader: public QThread {
public:
    virtual void run();
    void addCollector(CollectorType type, Collector* collector);
    void setData(int* counter, const QStringList& files);
    void addFilter(IFilter* filter);
private:
    int* counter;
    QStringList files;
    std::map<CollectorType, Collector*> collectors;
    std::vector<IFilter*> filters;
};


#endif // ENTRYREADER_H
