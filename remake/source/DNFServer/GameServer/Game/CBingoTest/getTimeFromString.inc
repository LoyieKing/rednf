void CBingoTest::getTimeFromString(std::string s)
{
    boost::posix_time::ptime pt = boost::posix_time::time_from_string(s);
    std::tm t = boost::posix_time::to_tm(pt);
    std::mktime(&t);
