void DisJoint::_GetAdditionalItem(const CItem* item, int& outIdx, int& outCount)
{
    // [b26N1] m_additionalWeight[rarity] 大偏移寻址：ORIG(4.1) 拆 add $0x150 进
    // scaled idx + disp 0x10；4.4 恒折 add $0x154 单步（N1 族折叠方向不可翻），
    // 其余全等，ndiff 12 封顶。
    outIdx = _GetAdditionalItemInfo(item->get_rarity());
    // ORIG 0x084732de `test %eax;setne %al;test %al;je`：outIdx!=0 物化为
    // bool 局部（P194 setne 形）。[b28c-21] 清 register asm 存量绑定（P0/N96），
    // 改普通 bool 自然形。
    bool nonzero = (outIdx != 0);
    if (nonzero)
    {
        // ORIG：m_item 先落 -0x18 再乘 100 落 -0x14，随后才 call get_rand_int。
        int w = m_additionalConst[item->get_rarity()].m_item;
        int weight = w;
        weight *= 100;
        if (get_rand_int(10000) < weight)
        {
            // ORIG 中间 float 槽：fstps/flds 往返后 fistpl 截断。
            const float v = (float)item->get_grade() /
                            m_additionalWeight[item->get_rarity()];
            outCount = (int)v;
        }
        else
        {
            const float v = (float)item->get_grade() /
                            m_additionalRate[item->get_rarity()];
            outCount = (int)v;
        }
        if (outCount < 1)
        {
            outCount = 1;
        }
    }
