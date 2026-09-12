// 0x0866afa2：习得单技能主分支（master_new_skill.md）：
// skillIdx∈[0xc8,0xd0] 走公会技能（需会长/副会长）并经 guild proxy 上报；
// 否则普通 SkillSlot::master_new_skill。回包 header(1,0x1f)：失败字节0+错误码
// 映射{-1:0x15,-2:0x16,-3:0x12,-4:0x01,-5:0x5b,-6:0x5c}；成功 byte1+treeIdx+
// 剩余SP/SFP(short)+新槽位+skillIdx+等级。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0866afa2 | _ZN5CUser16master_new_skillER14stBuySkillInfob
// [b28d-GrpD 判词·结构化 N] ORIG 0x0866afa2：残差为 `skillIdx`/`isGuildSkill` 等具名量
//   槽化（ORIG 多次 `movzbl 0x1(%eax),%eax; cmp $0xc7/$0xd0,%al` 重读字段，我方
//   `cmpb $0xc7,-0xS`）+ 帧 0xac vs 0x8c；跳表常量岛（0x5c/0x5b/0x1/0x12/0x16）、
//   get_remain_sfp kind2/kind3 站点、全局 `mov 0x700(,%eax,4)` 索引全部同址
//   ⇒ 无语义缺口；可按 P183 逐点删具名收敛。
void CUser::master_new_skill(stBuySkillInfo& info, bool flag)
{
    (void)flag;
    // ORIG 0x0866afa2：treeIdx 仅在非 guild 路径求值（0x0866b1b9 首次调用），
    // guild 路径不调用 ⇒ 赋值下沉到 else 分支（§26.2 支配序复刻；槽位不变）。
    int treeIdx;
    unsigned char skillIdx = (unsigned char)info.m_skillIdx;
    int ret;
    // ORIG 0x0866b0da/+00e1/+00e8：三个出参槽（-0x36/-0x24/-0x34）**无预清零**，
    // lea 直传未初始化槽给 master_new_guild_skill（§26.3：不得补默认值）。
    int learnCnt;
    int needGold;
    short needGSP;
    bool isGuildSkill = (skillIdx >= 0xc8 && skillIdx <= 0xd0);
    // 2026-09-11 R5 负结论（P183「删具名缓存⇒逐使用点重读」实测**净劣化**，已回滚）：
    //   按「ORIG 0x0866afa2 段 9× `movzbl 0x1(%eax),%eax`（重读 info.m_skillIdx）」
    //   删 skillIdx/isGuildSkill 两具名 ⇒ 实测 `movzbl 0x1(%eax),%eax` 我方 1→**10**
    //   （ORIG 9），指令数 395→409（ORIG 399 ⇒ gap +4→**−10**），ndiff 仅 61→60
    //   ⇒ 净劣化（多出的 14 条为每使用点重载 `mov 0xc(%ebp),%eax` 取 info 地址，
    //   ORIG 侧 info 指针常驻寄存器 = N7/N18a 寄存器分配族，源码不可造）⇒ 维持具名缓存。

    if (isGuildSkill)
    {
        // 2026-09-11 CuA（P59/P222 守卫包裹形 + 去取反物化）：ORIG 0x866afd2..0x866affe
        //   `call IsGuildMaster; test %al,%al; jne +0x1e` / `call IsSubGuildMaster; test
        //   %al,%al; je +0x19` / `mov $0x1,%eax; jmp` / `mov $0x0,%eax; test %al,%al;
        //   je <尾部 return>` —— **无 `xor $0x1` 物化**（正向 `A||B` 到 al 再 test）。
        //   原写 `!A() && !B()`（德摩根反写）令 4.4 恒产 `xor $0x1,%eax`（每侧一条）+
        //   jne/je 极性镜像。探针 /tmp/P5/p_guard g.cpp：v1(`!A&&!B`)/v2(`!(A||B)`) 均产
        //   xor；v3(`A||B`)/w1(void 版 + else return) 与 ORIG 逐条同形。
        if (IsGuildMaster() || IsSubGuildMaster())
        {
        // ⚠ 内层缩进未调整（最小化 diff）：本 `if` 的正文即原 guild 块全部语句。
        // ORIG 0x0866b..：经 GetGuildDBInfo() 访问器取 guildDB(+0x42=m_guildSkillPoint)。
        unsigned short guildLevel =
            ((STGuildDBInfoOnly*)GetGuildDBInfo())->m_guildSkillPoint;
        ret = ((SkillSlot*)getCurCharacSkillR())->master_new_guild_skill(
            (int)skillIdx, get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            *reinterpret_cast<STGuildDBInfoOnly*>(GetGuildDBInfo()), (int)getCurCharacMoney(),
            needGold, learnCnt, needGSP, (int)guildLevel);
        if (ret >= 0)
        {
            // ORIG 0x0866b122：GetServerProxy 返回后**无判空**（0x0866b127..0x0866b148
            // 全为压参，无 test/je），结果直用作 receiver；实参求值顺序 guildkey
            // (0x0866b0ee) → characNo(0x0866b103) → receiver(0x0866b10a..0x0866b122)。
            GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
                ->SendBuyGuildSkill((unsigned int)get_charac_no(-1),
                                    (unsigned int)get_charac_guildkey(),
                                    (int)skillIdx, learnCnt, needGSP,
                                    (unsigned int)needGold);
        }
        }
        else
        {
            return;
        }
    }
    else
    {
        treeIdx = GetCurCharacSkillTreeIndex();
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (info.m_field0 == 1)
                return;
        }
        else if (info.m_field0 != 1)
        {
            return;
        }
        ret = ((SkillSlot*)getCurCharacSkillW())->master_new_skill(
            (int)skillIdx, get_charac_level(), get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            (int)info.m_count);
    }

    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(1, 0x1f);
    if (ret < 0)
    {
        pkt->put_byte(0);
        // ORIG 0x0866b0xx 跳表(0x8cef5b0)：每 ret 值一独立 put_byte 常量站点（6 case），
        // 非数组索引。展开以匹配 put_byte 调用多重集。
        switch (ret)
        {
            case -6: pkt->put_byte(0x5c); break;
            case -5: pkt->put_byte(0x5b); break;
            case -4: pkt->put_byte(0x01); break;
            case -3: pkt->put_byte(0x12); break;
            case -2: pkt->put_byte(0x16); break;
            case -1: pkt->put_byte(0x15); break;
            default: break;
        }
    }
    else
    {
        pkt->put_byte(1);
        // ORIG 0x0866b3b2..：put_byte/sp/sfpKind 判定各重调 GetCurCharacSkillTreeIndex()
        //（P44），sfp 按 (tree==-1||tree==0) 分 kind2/kind3 两独立 get_remain_sfp 站点。
        pkt->put_byte(GetCurCharacSkillTreeIndex());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex()));
        int treeForSfp = GetCurCharacSkillTreeIndex();
        if (treeForSfp == -1 || GetCurCharacSkillTreeIndex() == 0)
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)2));
        else
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)3));
        if (!isGuildSkill)
        {
            pkt->put_byte(ret);
            pkt->put_byte(skillIdx);
            int level = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
            pkt->put_byte(level);
        }
        else
        {
            pkt->put_byte(0xff);
            pkt->put_byte(skillIdx);
            pkt->put_byte(learnCnt);
        }
        // ORIG 0x0866b4b4：if/else 之后两分支共用一次 getCurCharacSkillR()+get_skill_level()
        // 再 WriteLearnSkill（原实现误置于 !isGuildSkill 内，公会技能分支漏记日志）。
        int learnLevel = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
        HistoryLog::WriteLearnSkill((FILE*)m_field796f8, (int)skillIdx, learnLevel);
    }
    pkt->finalize(true);
    Send(packet);
}