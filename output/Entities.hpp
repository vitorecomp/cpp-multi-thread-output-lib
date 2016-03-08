#ifndef MessegeBox_H
#define MessegeBox_H

//includes
#include <iostream>
#include <string>
#include <list>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <string>

//files
#include "BaseTypes.hpp"

//defines
typedef unsigned int uint;

//namespace
using namespace std;

class MessageBox : public Screen{
    string name;
    uint start_x, start_y, size_x, size_y, endx;

    list<string> msg_list;

    void add_on_list(string);
    void printlist();
public:
    MessageBox(string, uint, uint, uint, uint, uint);

    void print(Message&);
};

class ProgressBar : public Screen{
public:
    enum DIRECTION {VERTICAL, HORIZONTAL};

private:
    string name;
    uint start_x, start_y, size_x, size_y, endx;

    double max, min;
    DIRECTION dir;

    void printHorizontalGraph(double porcentage);
    void printHorizontalGraphArrange();
public:

    ProgressBar(string, uint, uint, uint, uint, DIRECTION, double, double, uint);

    void print(Message&);
};


#endif
