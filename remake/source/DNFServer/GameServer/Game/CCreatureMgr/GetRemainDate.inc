// ORIG CCreatureItem::GetRemainDate(int) @0x833f016：
//   v = *(int*)(this+0x18)（m_expireTime）；
//   v <= 0 → 返回 v；
//   否则 r = v - nowSec；r == 0 → 返回 1；否则返回 r。
int CCreatureItem::GetRemainDate(int nowSec) const
{
    // ORIG 0x833f024：jle→尾岛 reload m；内层 cmpl0;jne→03f(return r)，
    // fall→038(mov$1) = return 1 在前、return r 尾岛形。
    if (m_expireTime > 0)
    {
        int r = m_expireTime - nowSec;
        if (r == 0)
            return 1;
        return r;
    }
    return m_expireTime;
}
