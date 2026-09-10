void break_away_prevent::CBreakAwayPreventSystem::loadDbData(
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data)
{
    memcpy(this, data, 0x2e4);   // 基类子对象区（原 m_base）
    m_rewardMap.clear();

    int count = data->m_field2e0;
    for (int i = 0; i < count; ++i)
    {
        // 保留裸形式说明（本函数当前 identical）：ORIG 0831c50b..0831c5c9 就是
        // `add %i*0x18 基址; add $0x10; movzbl 0x1(%eax)/0x2(%eax)/(%eax)` 与
        // `lea; add $0x14/$0x18/$0x1c/$0x20/$0x24; mov (%eax)` 两步形态
        // （break_away_prevent__CBreakAwayPreventSystem/loadDbData.md）。等价的
        // 具名成员 data->m_rewards[i].m_field1 等（SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER，
        // 基址 +0x10/stride 0x18 一致）在 gcc4.4 O0 会把 +0x10 折进单 disp，破坏
        // identical，故按 P30/P160 口径保留原转写形态（表寻址两步精确复现）。
        STRewardDataKey key;
        // [b28d-tu-GlobalData] P334 第二形修正：ORIG 0831c500/0831c50b 循环体首
        //   连发两个默认构造 `lea -0x37(key); call` + `lea -0x4c(reward); call`，
        //   之后才逐字段装载；我方原把 `STRewardData reward;` 声明写在 key 赋值
        //   之后 ⇒ reward ctor 被挪到第 3 个 key 读之后（构造站点错位）。
        //   声明上移（POD，无 ctor 提前副作用 ⇒ 仅移动 call 位次）。
        STRewardData reward;
        // [b28d-tu-GlobalData] 双判据真缺口修正（40 条指令 gap 的主体）：ORIG
        //   **每个字段访问都从形参 0x8(%ebp)+槽 -0xc(i) 完整重物化**
        //   （`mov -0xc,%edx; mov 0x8(%ebp),%ecx; 3x; shl $3; lea; add $0x10..;`
        //   10 行/字段，0831c4e3/50b/549/563/57d/597/5b1 七段 missing），我方
        //   原 `entry` 缓存 ⇒ 每字段只 4 行 ⇒ 少 40 条。删缓存、逐字段重算
        //   （P48 无引用缓存族），0x11/0x12/0x10 序 = ORIG 字节装载序。
        key.m_data[0] = *((const char*)data + i * 0x18 + 0x11);
        key.m_data[1] = *((const char*)data + i * 0x18 + 0x12);
        key.m_data[2] = *((const char*)data + i * 0x18 + 0x10);

        reward.m_field0  = *(int*)((const char*)data + i * 0x18 + 0x14);
        reward.m_field4  = *(int*)((const char*)data + i * 0x18 + 0x18);
        reward.m_field8  = *(int*)((const char*)data + i * 0x18 + 0x1c);
        reward.m_fieldc  = *(int*)((const char*)data + i * 0x18 + 0x20);
        reward.m_field10 = *(int*)((const char*)data + i * 0x18 + 0x24);

        std::map<STRewardDataKey, STRewardDataList>::iterator it =
            m_rewardMap.find(key);
        if (it != m_rewardMap.end())
        {
            it->second.m_list.push_back(reward);
        }
        else
        {
            STRewardDataList list;
            list.m_list.push_back(reward);
            m_rewardMap.insert(std::make_pair(key, list));
        }

        m_keys[i].m_data[0] = key.m_data[0];
        m_keys[i].m_data[1] = key.m_data[1];
        m_keys[i].m_data[2] = key.m_data[2];
    }
