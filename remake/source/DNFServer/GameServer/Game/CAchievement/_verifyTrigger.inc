bool CAchievement::_verifyTrigger(unsigned int id, unsigned short t0,
                                  unsigned short t1, unsigned short t2)
{
    switch (id)
    {
    case 0x19c4:
    {
        time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm local;
        localtime_r(&now, &local);
        if (local.tm_hour < 0)
        {
            break;
        }
        if (local.tm_hour > 0x14)
        {
            break;
        }
        return 0;
    }
    case 0x198b:
    case 0x1983:
        if (m_user->get_charac_level() <= 0x45)
        {
            return 0;
        }
        break;
    }
    return 1;
