CCompoundAvatar::tagCompoundInfo CCompoundAvatar::_MakeCompoundInfo(
    const CUser* user, const MSG_COMPOUND_AVATAR* msg) const
{
    tagCompoundInfo info;                        // ORIG C1 0x08334344
    info.clear();                                // ORIG clear 0x0833424a

    // msg 字段读取（MSG_COMPOUND_AVATAR.h；偏移/宽度与 ORIG movzwl/mov/movzbl 一致）：
    //   m_optionCheckFlag→+0xd6 / m_optionIdx→+0xd0 / m_slot→+0xd4 /
    //   m_avatarSlot→+0x90 / m_materialSlotA→+0x08 / m_materialSlotB→+0x48 /
    //   m_materialItemIdA/B→find_item / m_type→+0xe8
    info.m_fieldD6 = (msg->m_optionCheckFlag == 1);
    info.m_fieldD0 = msg->m_optionIdx;
    info.m_slot = msg->m_slot;
    info.m_field90 = msg->m_avatarSlot;

    // 三步形（0x08331f64..0x08332043）：临时槽 + 逐字段拷贝；每次重取
    // getCurCharacInvenR（P183/P48 禁缓存 inv）。
    // [b28d-solo-CompoundDeathLCB] ORIG 三步**复用同一 -0x88 临时槽**（0x08331fxx/
    // 0x083320xx 三次 GetInvenSlot 的 sret 目标同址；帧 0x8c）⇒ 用块作用域同名
    // 局部让 4.4 复用槽（三个不同名局部会各占 0x3d 字节，帧 0x11c，实测 +0x90）。
    info.m_avatar = *&user->getCurCharacInvenR()->GetInvenSlot(
        1, static_cast<int>(static_cast<short>(msg->m_avatarSlot)));

    info.m_field08 = msg->m_materialSlotA;
    info.m_materialA = *&user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotA)));

    info.m_field48 = msg->m_materialSlotB;
    info.m_materialB = *&user->getCurCharacInvenR()->GetInvenSlot(
        2, static_cast<int>(static_cast<short>(msg->m_materialSlotB)));

    info.m_scriptA = G_CDataManager()->find_item(
        static_cast<int>(msg->m_materialItemIdA));
    info.m_scriptB = G_CDataManager()->find_item(
        static_cast<int>(msg->m_materialItemIdB));

    if (info.m_scriptA == 0 || info.m_scriptB == 0)
    {
        info.m_errorFlag = true;                 // ORIG +0xd7
        return info;
    }
    // ORIG 0x08332133/0x0833215c：msg 两 itemIdx 必须与槽内物品 addInfo
    //（Inven_Item+0x02，即 info+0x0c / info+0x4c）一致，任一不符 → errorFlag。
    // [b28d-solo-CompoundDeathLCB] 两侧同为 unsigned int ⇒ 裸比较（ORIG
    // 0x08332201 `mov 0x11(%eax),%edx` 单段直读）；冗余 `(int)` cast 令 4.4
    // 多一条 `mov %eax,%edx` 中转（P41/P254 变体）。
    if (msg->m_materialItemIdA != info.m_materialA.m_addInfo ||
        msg->m_materialItemIdB != info.m_materialB.m_addInfo)
    {
        info.m_errorFlag = true;
        return info;
    }
    // ORIG 0x08332153：job 0..10 匹配循环 —— 两脚本同 job 且均可用时
    // m_field00=job；无匹配保持 clear 哨兵 0xb → 直接返回（不设 error）。
    for (int job = 0; job <= 10; ++job)
    {
        // [b28d-solo-CompoundDeathLCB] ORIG 0x08332269 首段为 `cmp %al,%dl;
        // setne %al; test; jne <continue>` ⇒ 源码是否定早返 `if (A != B)
        // continue;`（P196/P133 极性判据；写 `==` 会产 sete/je 反形）。
        // 第二段 `mov$1/mov$0; test %al,%al; je` 是**无槽**寄存器汇聚岛（P0）
        // ⇒ 用 P226 语句表达式包住短路条件（具名 bool 会多 store+cmpb 两条）。
        if (info.m_scriptA->check_job_type(static_cast<char>(job)) !=
            info.m_scriptB->check_job_type(static_cast<char>(job)))
        {
            continue;
        }
        if (({ info.m_scriptA->check_job_type(static_cast<char>(job)) != 0 &&
               info.m_scriptB->check_job_type(static_cast<char>(job)) != 0; }))
        {
            info.m_field00 = job;        // ORIG +0x00
            break;
        }
    }
    if (info.m_field00 == 0xb)                   // ORIG 0x0833221b：无匹配
        return info;

    unsigned int found = 0;                      // -0x2c：匹配到的 rec 下标
    // [b28d-solo-CompoundDeathLCB] ORIG 0x083322fb 每次迭代重发 `m_tables[f00]`
    // 完整取址（f00*0xc + this，P183 禁缓存；引用局部会占槽并把帧撑大）。
    for (unsigned int idx = 1; idx < m_tables[static_cast<unsigned int>(info.m_field00)].m_avatars.size();
         ++idx)  // -0x24
    {
        const tagCompoundAvatar* rec =
            m_tables[static_cast<unsigned int>(info.m_field00)].getCompoundAvatarInfo(
                static_cast<int>(idx));
        if (rec == 0)
            continue;

        // ORIG 4 个类型分支**各自完整展开**一遍迭代器遍历（begin/end/operator!=/operator++/operator-> 各 4 组，槽 -0x30/-0x34…-0x48/-0x4c），
        // 不是提公共 vector 指针后单循环（P165 物理展开 / N-C）。
        // [b28d-solo-CompoundDeathLCB] ORIG 0x08332329 为线性比较链
        //（`test %al,%al; jne` / `cmp $0x1,%al; jne` / `cmp $0x3,%al; jne`）
        // ⇒ 原码是 if-else-if 链而非 switch（P17；switch 令 4.4 发决策树
        // cmp/jg/je 共 +10 条）。
        if (msg->m_type == 0)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailA.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailA.end();
                // [b28d-solo-CompoundDeathLCB] ORIG 0x08332370..0x083323b7 循环体形：
                // 命中直接 `found = idx; break;`（无 cand 中转槽；四个 case 的 cand
                // 各占 4 字节 ⇒ 帧多 0x10）。
                while (it != last)
                {
                    // ORIG 0x08332370 `mov 0x94(%eax),%esi` 单段直读 ⇒ 两侧同为
                    // unsigned（`(int)` cast 会令 4.4 多一条 eax→esi 中转）。
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                // ORIG 0x083323b7：每个分支循环后各有一份 `cmpl $0,-0x28(%ebp);
                // je <下一 else-if>; jmp <跳出>`（= if (found != 0) break;）
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 1)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailB.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailB.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 3)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_tailC.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_tailC.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else if (msg->m_type == 4)
        {
                // 声明即初始化：ORIG 各分支把 begin()/end() 的 sret 目标**直接**
                // 指到本组迭代器槽（0x083323xx 区无 normal_iterator 默认构造调用），
                // 故不可先 `iterator it;` 再赋值；found 经 cand 变量传递（ORIG 用
                // 条件里的 `uVar8 = local_24` 逗号项），避免二次 operator!= 调用。
                std::vector<std::pair<int, int> >::const_iterator it =
                    rec->m_cube.m_vec.begin();
                std::vector<std::pair<int, int> >::const_iterator last =
                    rec->m_cube.m_vec.end();
                while (it != last)
                {
                    if (info.m_avatar.m_addInfo ==
                        static_cast<unsigned int>(it->first))
                    {
                        found = idx;
                        break;
                    }
                    ++it;
                }
                if (found != 0)
                    break;
        }
        else
        {
            continue;                        // 其它类型跳过本轮
        }
    }
    info.m_field04 = static_cast<unsigned int>(found);  // ORIG +0x6cd
    info.m_type = msg->m_type;                   // ORIG +0x6d9
    return info;
