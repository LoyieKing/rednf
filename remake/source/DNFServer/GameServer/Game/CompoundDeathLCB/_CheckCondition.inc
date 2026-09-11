// ORIG 0x0833260c（const）。错误码：1 材料异常/条目缺失、0x11 通用条件失败、
// 0x13 有效期类别不一致、0x16 缺事件材料、0x17 组合约束失败、0x18 剩余天数过短；
// 0 通过。
//
// 【batch-11 验收标注（§6.2 口径）】orig 611 / ours 616（gap 5）。
//   ① 调用点差集：ORIG direct 52 + indirect 10 ↔ ours direct 52 + indirect 10，
//      **归零**（仅 __normal_iterator C2×2 ↔ 4.4 发射 C1×2 的同体符号名对称差）；
//   ② 逐块补齐：CI_* 由 static helper 改宏内联（虚取址 7 insn 回到本帧，
//      indirect 0→10）、补 ORIG 0x08332792 的 `m_field04 == 0 → 0x17` 漏判、
//      getCurCharacInvenR ×7 / GetAvatarItemMgrR ×3 / get_grade ×6 /
//      get_charac_job ×2 全部改为逐点重取、迭代器改「默认构造 + 分支内
//      begin/end + 循环后 ==」形、opts 下标不再命名局部（gap 156 → 5）；
//   ③ 残余 5 insn 为 4.1 的 `cmp $imm; sete; test` 布尔物化岛与统一出口
//      jmp/nop 布局（N3/N76/setcc-fold 族）⇒ 判**语义完成、byte 层封顶**。
// 【b28e-M1S2 本波复核】读数 orig 611 / ours 609（残余 2，较旧记录 5 已再降）。
//   逐 hunk 定位残余：① `call __normal_iterator::operator->` 后 ORIG 2 条
//   （`mov (%eax),%eax; mov %eax,%edx`）被 4.4 copy-prop 折成 1 条
//   `mov (%eax),%edx`；② 同族另一处（共 2 条）。二者皆 4.4 折叠产物。
//   新钥匙逐条核过：P339/P331（本件差 2 条、非「整体平移/槽互换」⇒ 不属该域）、
//   P226/P231（CI_* 宏属 TU 级实体禁改；18 处间接虚调用指令数已对齐，仅
//   ORIG 函数指针→%edx / 4.4→%eax 的 N7 寄存器差）、P216/§6.6.2（无 landing 差）
//   ⇒ 判 N7+copy-prop 封顶，无语义缺口（p167 CLEAR 52/52 direct + 10/10 indirect）。
// 物化口径（P183/P167 实证）：ORIG 对每个判据都重发完整取数链 ——
//   getCurCharacInvenR ×7（0x08332c27/0c6a/0cad/0cee + 日期段 0x0723/0x0752/0x0781/0x07ab）
//   GetAvatarItemMgrR ×3、get_grade ×6、get_charac_job ×2、
//   两个 vector 迭代器各显式构造（normal_iterator C2 ×2）+ begin/end ×2 + 循环后 ==。
// 重建端禁缓存（一处缓存即整条链消失，本函数 gap 156 的主体即此）。
int CCompoundAvatar::_CheckCondition(const CUser* user,
                                     const tagCompoundInfo& info) const
{
    // 【b28d-solo-CompoundDeathLCB 判词】DIFF 封顶（§6.2）：ORIG 611 / ours 615、
    // p167 CLEAR(52/52)。本轮修法：table 引用内联（P183）、m_field04 检查前移至
    // CI_UsableType1 之前、while+break 拆 &&（P225）、尾段正极性包裹（P214①）。
    // 残余 = 间接虚调用目标寄存器 N7 ×18（4.4.7 恒 %eax / ORIG %edx；本 TU 换
    // 4.4.6 后端实测 n7 50→10 但 TU 整体 ae/near/diff 变差 ⇒ 不路由）+ 尾段
    // 块序/槽号（N82/N93 族）。
    // 分离模式(type4)仅允许指定物品 0x2898c4。
    if (info.m_type == 4 && info.m_avatar.m_addInfo != 0x2898c4)
    {
        return 0x11;
    }
    if (info.m_slot > 99)                                       // u16 cmp $0x63
    {
        return 0x11;
    }
    if (info.m_errorFlag != 0)
    {
        return 0x11;
    }
    if (info.m_field00 == 0xb)                                  // 表索引无效
    {
        return 0x17;
    }
    // [b28e] ORIG 0x08332687/0x08332691 为 `test a; je L11` + `test b; jne NEXT`
    // —— **两跳同落同一个 `mov $0x11` 共享块 L11**（P272 双跳同落 then 形），
    // 两个独立 if-return 会各发一份 `mov $0x11` 岛。
    if (info.m_materialA.m_addInfo == 0 || info.m_materialB.m_addInfo == 0)
    {
        return 0x11;
    }

    // 任一材料的 uid 是临时 key → 1（0x083326a8 / 0x083326ba 两次实调 +
    // mov$1/mov$0 汇聚岛）。
    if (CAvatarItemMgr::IsTempKey(info.m_materialA.m_addInfo2) ||
        CAvatarItemMgr::IsTempKey(info.m_materialB.m_addInfo2))
    {
        return 1;
    }

    if (!CI_IsAvatarItem(info.m_scriptA) ||
        !CI_IsAvatarItem(info.m_scriptB))
    {
        return 0x11;
    }
    if (CI_SubType(info.m_scriptA) != CI_SubType(info.m_scriptB))
    {
        return 0x17;
    }
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08332792 的 `m_field04 == 0 → 0x17`
    // 位于 CI_UsableType1 虚检查（0x0833292f）**之前**（历史注释位置有误）。
    if (info.m_field04 == 0)
    {
        return 0x17;
    }
    if (!(CI_UsableType1(info.m_scriptA) && CI_UsableType1(info.m_scriptB)))
    {
        throw static_cast<ENUM_ERROR>(0x17);                    // __cxa_throw
    }

    // [b28d-solo-CompoundDeathLCB] ORIG 0x083327a5 直接把 `m_tables[m_field00]`
    // 取址内联进调用点（`mov 0x4(%eax),%ecx` 先物化实参、后算表地址）；
    // 具名 table 引用会占槽（P183 禁缓存）。
    const tagCompoundAvatar* rec =
        m_tables[info.m_field00].getCompoundAvatarInfo(info.m_field04);  // -0x1c

    if (!(info.m_scriptA->get_rarity() < 2 &&
          info.m_scriptB->get_rarity() < 2))
    {
        return 0x17;
    }
    if (info.m_scriptA->get_grade() != info.m_scriptB->get_grade())
    {
        return 0x17;
    }

    // 非“加攻标记”时才检查选项池里存在用户请求的 option 对：
    // grade == rec->m_idx 用 m_optsC[sub]（+0x194），否则 grade 必须 ==
    // rec->m_flag2dc（+0x2dc）再用 m_optsB[sub]（+0xf4）。
    if (info.m_fieldD6 != 1)
    {
        // ORIG 两个迭代器先默认构造（-0x20/-0x24，normal_iterator C2 ×2），
        // 再在两分支内各自 begin()/end() 赋值；循环用 != 条件 + 循环后 ==。
        // ORIG 每个 begin()/end() 前都重发一次 CI_SubType 虚调用（0x0833292f
        // 与 0x0833297b 各 12/13 insn；else 支 0x083329f3/0x08332a3f 同）⇒
        // 下标不命名局部（P183/P36）。
        std::vector<std::pair<int, int> >::const_iterator it;
        std::vector<std::pair<int, int> >::const_iterator last;
        if (info.m_scriptA->get_grade() == rec->m_idx)
        {
            it = rec->m_optsC[CI_SubType(info.m_scriptA)].begin();
            last = rec->m_optsC[CI_SubType(info.m_scriptA)].end();
        }
        else
        {
            // [b28e] ORIG 0x083329cc 段为 `cmp; sete %al; test %al,%al; je
            // <0x08332a8d: mov $1; jmp>`（相等 → 直落 begin/end，不等 → 跳出到
            // 返回 1 岛）⇒ 原码是 **正极性 if(==){…} else {return 1;}** 的
            // if/else 形，而非 `if(!=) return 1;` 早返（早返令 4.4 产
            // `setne; test; je 越过 mov$1` 的镜像块序）。
            if (info.m_scriptA->get_grade() == rec->m_flag2dc)
            {
                it = rec->m_optsB[CI_SubType(info.m_scriptA)].begin();
                last = rec->m_optsB[CI_SubType(info.m_scriptA)].end();
            }
            else
            {
                return 1;
            }
        }
        // [b28d-solo-CompoundDeathLCB] ORIG 循环体为 `cmp; sete %al; test %al,%al;
        // jne <找到出口>` 的 bool 物化形（P225）⇒ 拆成 while(条件) + if break，
        // 合并 && 会令 4.4 直产 je/jne 直跳（无 setcc）。
        while (it != last)
        {
            if (it->first == info.m_fieldD0)
            {
                break;
            }
            ++it;
        }
        if (it == last)
        {
            return 1;
        }
    }

    // grade 必须都 >= 所选 recipe 下标，且不超出表内上限 flag2dc。
    if (info.m_scriptA->get_grade() < info.m_field04 ||
        info.m_scriptB->get_grade() < info.m_field04)
    {
        return 0x17;
    }
    // [b28e] ORIG 0x08332b58/0x08332b6a 先 `call get_grade` 再装
    // `rec->m_flag2dc`（`mov -0x1c(%ebp),%edx; mov 0x2dc(%edx),%edx; cmp
    // %edx,%eax; jg/jle`）= 左操作数是 get_grade() 的 `>` 书写形；写
    // `flag2dc < get_grade()` 会令 4.4 先把 flag2dc 缓存进 %ebx 再 call（求值序反）。
    if (info.m_scriptA->get_grade() > rec->m_flag2dc ||
        info.m_scriptB->get_grade() > rec->m_flag2dc)
    {
        return 0x17;
    }

    // 角色职业两件均可用（两臂各重取 job，0x08332ba8 / 0x08332bd2）。
    if (!(info.m_scriptA->check_job_type(
              static_cast<char>(user->get_charac_job())) == 1 &&
          info.m_scriptB->check_job_type(
              static_cast<char>(user->get_charac_job())) == 1))
    {
        return 0x17;
    }

    // 按类型验证事件材料消耗单（ORIG if/else-if 链，每分支重取
    // getCurCharacInvenR，0x08332c27/0c6a/0cad/0cee）。
    if (info.m_type == 0)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailA) != 1)
        {
            return 0x16;                                        // +0x2b8
        }
    }
    else if (info.m_type == 1)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailB) != 1)
        {
            return 0x16;                                        // +0x2c4
        }
    }
    else if (info.m_type == 3)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_tailC) != 1)
        {
            return 0x16;                                        // +0x2d0
        }
    }
    else if (info.m_type == 4)
    {
        if (user->getCurCharacInvenR()->CheckNeedItemList(rec->m_cube.m_vec)
                != 1)
        {
            return 0x16;                                        // +0x20c
        }
    }

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const int remainA =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetRemainDate(info.m_materialA.m_addInfo2, curSec);
    const int remainB =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetRemainDate(info.m_materialB.m_addInfo2, curSec);
    const bool noExpireA =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetExpireDate(info.m_materialA.m_addInfo2) == 0;
    const bool noExpireB =
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
            ->GetExpireDate(info.m_materialB.m_addInfo2) == 0;

    if (noExpireA != noExpireB)
    {
        return 0x13;
    }
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08332dcx 尾段为 `jg <继续>` +
    // `mov $0x18,%eax; jmp`（早返岛）⇒ 正极性包裹改写（P214①；负极性
    // `if(cond){...} return 0x18;` 会让 4.4 产 jle 反形）。
    if (!(noExpireA || (remainA > 5 && remainB > 5)))
    {
        return 0x18;
    }
    // 分离升级位标记禁拆者不允许操作。
    if (info.m_materialA.m_upgradeSep.IsTradeRestriction() ||
        info.m_materialB.m_upgradeSep.IsTradeRestriction())
    {
        return 0x17;
    }
    return 0;
}