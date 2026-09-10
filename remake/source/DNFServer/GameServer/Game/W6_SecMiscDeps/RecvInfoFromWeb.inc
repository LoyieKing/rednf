bool WongWork::CWebEvent::RecvInfoFromWeb(unsigned int p1, unsigned int p2,
                                          unsigned int p3, unsigned int p4,
                                          unsigned int p5, unsigned int p6)
{
    // ORIG T 0x086c11d0 (size 0x4e)：p3==999 直接返回 0；否则转发
    // ReqSaveEventItem(...,false) 并透传其结果（ORIG 无布尔规范化）。
    if (p3 == 999)
    {
        return false;
    }
    return ReqSaveEventItem(p1, p2, p3, p4, p5, p6, false);
