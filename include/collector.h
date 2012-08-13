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

#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <QString>
#include <vector>
#include <map>

typedef struct {
    QString stringValue;
    qint64 value;
    int pictures;
    float percent;
} Entry;

class Collector {
public:
    Collector();
    ~Collector();
    void addValue(float value);
    void addValue(qint64 value);
    void addValue(QString value);
    void addExposureValue(float value);
    int count();
    int pictureCount();
    void clear();
    void sortByValue();
    void sortByCount();
    void calculatePercentage();
    Entry* operator[](size_t index);

private:
    std::vector<Entry> entryVector;    
    std::map<qint64, int> entryMap;
    std::map<QString, int> stringEntryMap;
    int pictures;
};

#endif // COLLECTOR_H
