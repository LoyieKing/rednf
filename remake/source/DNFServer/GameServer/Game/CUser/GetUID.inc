unsigned int CUser::GetUID() const
{
    // ORIG 0x80c8c9c movzwl 704a6,%eax；0x80c8ca3 cwtl；0x80c8ca4 movzwl %ax,%edx。
    // b28d-solo-CUser N封顶: 4.4 折叠 `(unsigned short)(short)` 中间 cwtl（探针 6 形：
    //   具名 rawHi→落槽、cast 链/语句表达式均无 cwtl），4.1 保留三段 = N18a 折叠族。
    // [CUserLane 2026-09-11 T1 实测无效] 形 `(unsigned short)((int)(short)f)` 仍无 cwtl
    //   （ndiff 1 不变）⇒ 复现第 7 形失败，维持 N18a 折叠族封顶。
    return (unsigned short)(short)m_field704a6 | ((int)(short)m_slotIDX << 16);
}