
#ifndef PERSON_H
#define PERSON_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <QDebug>
#include "mymap.h"

using namespace std;

class Person {

public:
    Person();

    int things[20][7][16];
    vector<string> allName;

    void readUserAccounts();
    void readCourses();
    void readActivities();
    void readTempEvents();
};
#endif // PERSON_H
