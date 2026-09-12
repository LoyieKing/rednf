// [R21bool/T1] 返回类型 int→bool（声明侧 SECRET_SHOP_DATA.h 同步）。
bool SECRET_SHOP_DATA::IsCompleteBuy()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_buying[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}