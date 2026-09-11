// ORIG W 0x080fdd2c：返回 this（条件缓冲即对象首 0x84c）。
stInGameADExposeCondition* CAdvertisementExposeInfo::getExposeCondition()
{
    return &m_condition;
}