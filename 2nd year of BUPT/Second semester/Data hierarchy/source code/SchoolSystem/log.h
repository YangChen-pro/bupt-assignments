#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <QDebug>
#include "CLOCK_TIME.h"


using namespace std;
class logTXT
{
public:
    logTXT(const std::string& filename,CLOCK_TIME *clk_time) {
    this->filename = filename;
    this->clk_time = clk_time;
    output_file.open(filename, ios::out);
    output_file << "This is our log." <<endl;
    output_file.close();
    }
    ~logTXT(){

    }
    void log(std::string str)
    {
        output_file.open(this->filename, ios::app);
        output_file <<"["+to_string(this->clk_time->getWeekNum())+" "+to_string(this->clk_time->getWeekDay())+" "+to_string(this->clk_time->getHour())+"] :: "<< str<<endl;
        output_file.close();
    }
    CLOCK_TIME *clk_time;
    string filename;
private:
    ofstream output_file;


};


#endif // LOG_H
