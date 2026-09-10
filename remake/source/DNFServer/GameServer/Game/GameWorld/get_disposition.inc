bool GameWorld::get_disposition(int vill, int area,
                                pvp_assault::CDisposition* disposition)
{
    // ORIG 0x086c4xxx：正向 test;je 早跳 + 尾部 return false（! 形态会多出 xor 物化）
    if (check_valid_area(vill, area))
    {
        return (*(bool (**)(void*, void*))*(void**)disposition)(
            disposition, (void*)&m_pVill[vill].getArea(area)->m_mapAreas);   // Area +0x7c
    }
    return false;
