bool ConvertWeekdayStringToInt(std::string s, unsigned int& out)
{
    if (s.compare("SUN") == 0) out = 0;
    else if (s.compare("MON") == 0) out = 1;
    else if (s.compare("TUE") == 0) out = 2;
    else if (s.compare("WED") == 0) out = 3;
    else if (s.compare("THU") == 0) out = 4;
    else if (s.compare("FRI") == 0) out = 5;
    else if (s.compare("SAT") == 0) out = 6;
    else if (s.compare("ALL") == 0) out = 7;
    else return false;
    return true;
