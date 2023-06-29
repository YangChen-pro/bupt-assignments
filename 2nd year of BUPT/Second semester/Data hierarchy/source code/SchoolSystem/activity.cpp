
#include "activity.h"

Activity::Activity(string name, int start_week, int end_week, int day, int start_period, int end_period, string location, int activity_type)
    : name(name), start_week(start_week), end_week(end_week), day(day), start_period(start_period), end_period(end_period), location(location), activity_type(activity_type)
{

}

string Activity::getName() const
{
    return this->name;
}

int Activity::getType() const
{
    return this->activity_type;
}
