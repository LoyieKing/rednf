void break_away_prevent::CBreakAwayPreventSystem::loadDbData(
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data)
{
    // [G7-gd] 语义修正（ORIG 0x0831c4a6 `movl $0x0,-0x10(%ebp)`）：count 是
    //   **去重后的插入计数**，声明在 memcpy 之前并预清 0；仅在新键插入路径
    //   （else 分支）写 m_keys[count] 三字节并 `++count`
    //   （ORIG 0x0831c6f2..0831c743：`mov -0x10,%edx … add $0x2f0,%eax;
    //   mov %cl,0xe/0xc/0xd(%eax); addl $0x1,-0x10`）。旧写把 m_keys[i] 三行放在
    //   if/else **之外**且下标用 i ⇒ 重复键时写入位置与 ORIG 不同（真 bug）。
    int count = 0;
    memcpy(this, data, 0x2e4);   // 基类子对象区（原 m_base）
    m_rewardMap.clear();

    // [G7-gd] ORIG 0x0831c773 循环条件每次都从 **this** 重读计数
    //   （`mov 0x8(%ebp),%eax; mov 0x2e0(%eax),%eax; cmp -0xc(%ebp),%eax; setg`），
    //   无 `count = data->m_field2e0` 的具名副本；memcpy 已把 data 拷入 this，
    //   语义等价（基址 this 与 ORIG 一致）。
    for (int i = 0;
         i < ((const SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER*)this)->m_field2e0;
         ++i)
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
        // [G7-gd] 基址改 this（ORIG 0x0831c4fc/518/534/…/5b7 全部
        //   `mov 0x8(%ebp),%ecx`）：memcpy(this,data,0x2e4) 之后两级源
        //   （this+… 与 data+…）内容相同，但 ORIG 用 this ⇒ 与 ORIG 同形。
        // [G7-gd] 三字节读改「元素指针 + 具名成员」形（P30 两步寻址）：ORIG
        //   0x0831c508..0x0831c54b 为 `lea (%ecx,%eax,1); add $0x10; movzbl
        //   0x1/0x2/0x0(%eax)`——0x10 属**指针算术**、字段偏移走 disp；
        //   `*((char*)this + i*0x18 + 0x11)` 会被 4.4 折成单 `add $0x11`
        //   （探针 fA 实证）⇒ 改视图形后 4.4 与 ORIG 同出 add $0x10 + disp。
        //   （int 字段保持裸形：ORIG 是 `add $0x14`+`mov (%eax)`，裸形已同。）
        key.m_data[0] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field1;
        key.m_data[1] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field2;
        key.m_data[2] = ((const SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER*)
                         ((const char*)this + i * 0x18 + 0x10))->m_field0;

        reward.m_field0  = *(int*)((const char*)this + i * 0x18 + 0x14);
        reward.m_field4  = *(int*)((const char*)this + i * 0x18 + 0x18);
        reward.m_field8  = *(int*)((const char*)this + i * 0x18 + 0x1c);
        reward.m_fieldc  = *(int*)((const char*)this + i * 0x18 + 0x20);
        reward.m_field10 = *(int*)((const char*)this + i * 0x18 + 0x24);

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
            // [G7-gd] ORIG 0x0831c6f2..0831c743：写序 [2]→[0]→[1]，
            //   下标 = count（本分支专用计数），写完 `++count`。
            m_keys[count].m_data[2] = key.m_data[2];
            m_keys[count].m_data[0] = key.m_data[0];
            m_keys[count].m_data[1] = key.m_data[1];
            ++count;
        }
    }
    // [G7-sub] 收口（本件唯一真语义缺口已修，余为常量拆分伪影）：
    //   改后 ORIG 243 / 我方 243、p167 仅 STRewardDataKey/STRewardData/
    //   STRewardDataList/pair 的 C1↔C2 / D1↔D2 别名（0 真缺口）。
    //   三字节读已回 ORIG 形（0x0831c508 `lea (%ecx,%eax,1); add $0x10;
    //   movzbl 0x1(%eax)`）；m_keys 写 ORIG 0x0831c705
    //   `add $0x2f0,%eax; mov %cl,0xe(%eax)` 与我方 `add $0x2f2,%eax;
    //   mov %cl,0xc(%eax)` 同址 0x2fe、仅大常量拆分不同（P30/N94 后端选择）；
    //   memcpy 实参装载序属 P23/N76 族 ⇒ 封顶。
