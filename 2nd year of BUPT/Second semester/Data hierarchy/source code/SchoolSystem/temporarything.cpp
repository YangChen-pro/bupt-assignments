
#include "temporarything.h"

TemporaryThing::TemporaryThing(string name, int week, int day, int hour, int minute, string location)
 : name(name), week(week), day(day), hour(hour), minute(minute), location(location)
{
}

// 判断临时事物的时间是否在有效范围内
bool TemporaryThing::isValid() const
{
    if (hour < 6 || hour > 22)
    {
        return false;
    }
    return true;
}

// 判断两个临时事物是否重叠
bool TemporaryThing::isOverlapping(const TemporaryThing& other) const
{
    if (week != other.week || day != other.day || name != other.name || hour != other.hour || minute != other.minute)
    {
        return false;
    }
    return true;
}
