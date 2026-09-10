void AdvanceAltarData::get(_AdvanceAltarDbData& out) const
{
    out.m_update = m_update;                          // +0x517 = this[0xcc]
    // [b28d-solo-advancealtar_DB] P68 cast 形：ORIG 0x088996e4 先载 this→edx 再载
    //   out→eax 再压长度；无 cast 的 &out 先求值使 4.4 交错（本行 cast 试形）。
    std::memcpy((char*)&out.m_ridable, (const char*)this, 0x21);  // 前 0x21（_RidableBase 扁平区）

    int local_10 = 0;
    for (std::map<int,_Stage>::const_iterator it = m_stages.begin();
         it != m_stages.end() && local_10 <= 0x31; )
    {
        // [b28d-solo-advancealtar_DB] P157/P183：ORIG 0x0889972f 段每次
        //   `call ptEv; mov 0x4(%eax),%eax` 重发完整访问链，无具名引用槽
        //   （旧 `const _Stage& st` 多 -0x10 槽 + add 链）。
        if (it->second.m_field0 < 1)                  // value.m_field0 < 1 → 跳过
        {
            it++;   // P28：ORIG 增量段调 ppEi（后置）⇒ 源形必写 it++
        }
        else
        {
            // ORIG 目标地址 = &out + 0x21 + local_10*8 → m_stages[local_10]
            std::memcpy(&out.m_stages[local_10], &it->second, sizeof(_Stage));
            it++;
            ++local_10;
        }
    }

    // ORIG 目标基址 +0x1b1 / +0x229 → 具名数组成员（元素宽 10 / 12 与 pack(1) 布局一致）。
    getSlot(m_slots1, out.m_slots, 0xc);
    getSlot(m_slots2, out.m_slots, 0xc);
    getBuyItem(m_buyItems1, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems2, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems3, out.m_buyItems, 0x32);

    local_10 = 0;
    for (std::map<int,_Reward>::const_iterator it = m_rewards.begin();
         it != m_rewards.end() && local_10 <= 0x1d; )
    {
        if (it->second.m_field0 < 1)
        {
            it++;
        }
        else
        {
            // ORIG 目标地址 = &out + 0x481 + local_10*5 → m_rewards[local_10]
            std::memcpy(&out.m_rewards[local_10], &it->second, sizeof(_Reward));
            it++;
            ++local_10;
        }
    }
