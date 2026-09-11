// ORIG 形态：if(q()){…;return true;} return false;（test %al; je 直测，P59）；
// 循环 i<=199（P2 setle）；无 tailPad（buf 恰 200*0x84c = 0x67b60）。
bool CInGameAdvertisementManager::LoadADExposeInfoAtApp()
{
    stInGameADExposeCondition buf[200];
    std::memset(buf, 0xff, 0x67b60);
    if (queryADExposeInfo(buf))
    {
        for (int i = 0; i <= 199; ++i)
        {
            if (buf[i].m_sentinel == -1)
            {
                break;
            }
            InsertADExposeInfo(buf[i]);
        }
        return true;
    }
    return false;
}