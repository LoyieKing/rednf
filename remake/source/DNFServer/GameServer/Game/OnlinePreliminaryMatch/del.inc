int TeamInfoSecond::del(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        /* ORIG @0855fe70 test;je skip / @0855fe7f cmp arg;jne skip：&& 短路
         * 正向嵌套 */
        if (m_seats[i].m_flag != 0 && m_seats[i].m_user == user)
        {
            m_seats[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}