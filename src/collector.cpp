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

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "collector.h"

using namespace std;

bool compareByValue(Entry e1, Entry e2)
{
    if (e1.value != e2.value) {
        return e1.value < e2.value;
    }
    else {
        return (e1.stringValue < e2.stringValue);
    }
}

bool compareByCount(Entry e1, Entry e2)
{
    return (e1.pictures > e2.pictures);
}

Collector::Collector()
{
    pictures = 0;
}

Collector::~Collector()
{

}

void Collector::addValue(float value)
{
    addValue(static_cast<qint64>(round(value * 10)));
}

void Collector::addValue(qint64 value)
{
    map<qint64, int>::iterator iterator = entryMap.find(value);

    if (iterator != entryMap.end()) {
        entryVector[iterator->second].pictures++;
    }
    else {
        Entry entry;
        entry.value = value;
        entry.pictures = 1;
        entry.percent = 0;

        if ((value % 10) == 0) {
            entry.stringValue = QString::number(static_cast<int>(value) / 10);
        }
        else {
            entry.stringValue = QString::number(value / 10.0, 'f', 1);
        }

        entryVector.push_back(entry);
        entryMap.insert(make_pair(value, entryVector.size() - 1));
    }
    pictures++;
}

void Collector::addExposureValue(float value)
{
    qint64 intValue = static_cast<qint64>(round(value * 1e6));

    map<qint64, int>::iterator iterator = entryMap.find(intValue);

    if (iterator != entryMap.end()) {
        entryVector[iterator->second].pictures++;
    }
    else {
        Entry entry;

        entry.value = intValue;
        entry.pictures = 1;
        entry.percent = 0;

        if (value >= 1) {
            int intValue2 = static_cast<int>(value * 10);

            if ((intValue2 % 10) == 0) {
                entry.stringValue = QString::number(intValue2 / 10);
            }
            else {
                entry.stringValue =
                    QString::number(static_cast<float>(intValue2) / 10, 'f', 1);
            }
        }
        else {
            entry.stringValue = "1/" +
                QString::number(static_cast<int>(round(1 / value)));
        }
        entryVector.push_back(entry);
        entryMap.insert(make_pair(intValue, entryVector.size() - 1));
    }
    pictures++;
}

void Collector::addValue(QString value)
{
    map<QString, int>::iterator iterator = stringEntryMap.find(value);

    if (iterator != stringEntryMap.end()) {
        entryVector[iterator->second].pictures++;
    }
    else {
        Entry entry;
        entry.value = 0;
        entry.pictures = 1;
        entry.percent = 0;
        entry.stringValue = value;

        entryVector.push_back(entry);
        stringEntryMap.insert(make_pair(value, entryVector.size() - 1));
    }
    pictures++;
}

int Collector::count()
{
    return entryVector.size();
}

int Collector::pictureCount()
{
    return pictures;
}

Entry* Collector::operator [](size_t index)
{
    if (index >= entryVector.size()) {
        return 0;
    }
    return &entryVector[index];
}

void Collector::clear()
{
    entryVector.clear();
    entryMap.clear();
    stringEntryMap.clear();
    pictures = 0;
}

void Collector::sortByValue()
{
    stable_sort(entryVector.begin(), entryVector.end(), compareByValue);
}

void Collector::sortByCount()
{
    stable_sort(entryVector.begin(), entryVector.end(), compareByCount);
}

void Collector::calculatePercentage()
{
    for (size_t i = 0; i < entryVector.size(); i++) {
        entryVector[i].percent =
            static_cast<float>(entryVector[i].pictures * 100) / pictures;
    }
}
