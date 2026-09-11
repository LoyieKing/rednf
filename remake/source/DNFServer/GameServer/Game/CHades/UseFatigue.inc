// ORIG 0x084b9552（UseFatigue.md）：needLogin 后计数 +1；累计 >1 时上报 cate=1 并清零。
void CHades::UseFatigue(unsigned int count)
{
    (void)count;
    needLogin();
    ++m_useFatigueCnt;
    if (m_useFatigueCnt > 1)
    {
        StdCateUpdate(1, 0);
        m_useFatigueCnt = 0;
    }
    else
    {
        return;
    }
}