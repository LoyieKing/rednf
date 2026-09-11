// ORIG _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj @ 0x0865eb0c (T)
// cooltime 处理：CStackableItem+0x2d0(ms) 换算为秒累加当前秒，更新/插入
// CheckCoolTimeItem 所读的 cooltime 列表条目（GetCurCharacCoolTimeItemList）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865eb0c | _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj
void CUser::_processCoolTimeItem(CStackableItem* item, unsigned int itemIdx,
                                 unsigned int curSec)
{
    // ORIG 0x0865eb15..0x0865eb35：`mov 0x2d0(%eax),%ecx; imul $0x10624dd3;
    //   sar $6; sar $0x1f; sub` ⇒ **有符号** /1000（写 unsigned 会产 shr/div）。
    unsigned int expireSec = item->m_int2d0 / 1000 + curSec;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    // ORIG 0x0865eb46 movb $0,-0x11 / 0x0865ebc6 movzbl;xor $1;test;je ⇒ bool 局部
    //   （4.4 对 char 的 !found 折 cmpb，N122 判据）。
    bool found = false;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        // ORIG 0x0865eb?? `mov (%eax),%eax; cmp 0x10(%ebp),%eax; sete` = mem 直比；
        // 冗余 (int) cast 反致 4.4 `mov 0x10(%ebp),%eax; cmp %eax,%edx`（P162/P41 族）。
        if ((*cool)[i].m_itemIdx == itemIdx)
        {
            // ORIG 0x0865eb83 `mov 0x4(%eax),%eax; mov %eax,-0xc` 缓存旧值再比
            //   （P0：具名局部，勿直写两次 operator[]）。
            unsigned int oldExpire = (*cool)[i].m_expireTime;
            // ORIG 0x0865eb7d `mov -0x1c,%eax; cmp -0xc,%eax; jbe` ⇒ 源码写
            //   `expireSec > oldExpire`（写 old < expire 会反装载序）。
            if (expireSec > oldExpire)
                (*cool)[i].m_expireTime = expireSec;
            found = true;
        }
    }
    if (!found)
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        cool->push_back(info);
    }
    UpdateCoolTimeItemInfo(curSec);
}