// ============================================================================
// CUser::UseQuestItem(int itemIdx)  ORIG T 0x08668d1a (0x16e4)。
// 报告 docs/class_func_reports/CUser/UseQuestItem.md（2005 行，汇编+反编译 C
// 全文已读）。觉醒/转职道具链：state==3 门控→交易中拒绝(0x20/0x13)→
// GetInvenSlot(1,itemIdx) 取当前道具，ORIG 二叉判定树内联转录（2026-09-05
// CUserC8：grow/subType/keepSkill/elixir 四元组逐节点 cmp 序照抄）→ grow 边界与职业守卫
// （失败发 0x20/0x11 错误包）→ 经验钳制到等级区间→扫描槽 3..8 与 0x99..0xc8
// 收集可删道具→0x12 包→CSkillChanger/CCalcCharacStat 技能与属性重算→
// set_grow_type/_postCheckForceChangeGrowType/双 SendNotiPacket。
// ============================================================================
// [b28d-22 N82] ORIG 0x08668d1a：p167=CLEAR，gap=-493；语义完成、byte 层帧槽/物化族封顶。
// [b28d-BigUseQuestItem 复核 2026-09-09] **旧判 N89 作废**：本函数 p167 差集只有
// C1↔C2 / D1↔D2 同址别名，ORIG 1279 条内**无任何 std::/__gnu_cxx:: 模板 call**
// （N89 机判条件不成立）。gap -493 的真因 = 判定树/叶块被归纳表查替代：
// ①判定树+叶块 ≈790 条 vs 表查 ≈40 条；②curIdx/id/cnt 具名槽诱发 cmpl mem
// 71 处（ORIG 全寄存器 cmp）；③outsideAll bool 物化 9 条 vs ORIG 1 条 ja；
// ④doChangeGrow char（cmpb）vs ORIG bool（xor $1/test）。按 ORIG 叶块地址序
// 恢复 75-case switch（4.4.7 lowering 与 4.1 逐条同构，探针 /tmp/probe_switch.cpp）
// + 删具名中间量 + bool 化 + `= {0}` rep stos 后：our 1278 / ORIG 1279（gap -1），
// p167 CLEAR，TU 四数 427/862/0/59/251 零回退。残余 = 帧槽基址差（sub 0xb8c vs
// 0xb7c，N82/N126）、grow 界错误包内联 vs 远跳（块序）、经验钳制 setb/seta 物化（P225）。
void CUser::UseQuestItem(int itemIdx)
{
    if (get_state() != 3)
        return;

    PacketGuard packet;
    if (CheckInTrade())
    {
        packet.clear();
        SendCmdErrorPacket((ENUM_CMDPACKET)0x20, 0x13, packet);
        return;
    }

    const Inven_Item cur = getCurCharacInvenR()->GetInvenSlot(1, itemIdx);

    // P0：ORIG 无 curIdx 具名局部槽，判定树/守卫全程 `mov -0x97(%ebp),%eax`
    // 重读 sret 对象字段 ⇒ 重建一律写 cur.m_addInfo 表达式（4.4 内存直比会
    // 破坏 159 条寄存器 cmp 形，实测 cmpl mem 71 条 vs ORIG 5 条）。
    char grow;                        // local_4d：目标职业档位（每 case 赋值）
    int  subType;                     // local_48：二转/子类（每 case 赋值）
    int  keepSkill = 0;               // local_4c：保留技能包标志（部分 case 置 1）
    bool doChangeGrow = false;        // local_41：换职标记（P248 档②：ORIG
                                      // `movzbl -0x41; xor $1; test; je` ⇒ bool）

    // [b28d-solo 复核] 旧判 N89 作废：本函数 p167 差集仅 C1/C2、D1/D2 同址别名，
    // 无任何 std::/__gnu_cxx:: 模板 call；gap 主体是「4.1 switch 二分判定树」
    // 被表查替代。4.4.7 对同 case 集合 lowering 与 ORIG 逐条同构（探针
    // /tmp/probe_switch.cpp 前 12 条 cmp/je/ja 全等），故按 ORIG 叶块地址序
    // （+0x49a..+0xad6；P46/P324 叶块物理序=书写序）恢复 switch；
    // default = LAB_08669808 未知道具错误包。
    switch (cur.m_addInfo)
    {
    case 0xfc9: grow = 0; subType = 1; break;
    case 0xfca: grow = 0; subType = 2; break;
    case 0xfcb: grow = 0; subType = 3; break;
    case 0xfcc: grow = 0; subType = 4; break;
    case 0xfce: grow = 1; subType = 1; break;
    case 0xfcd: grow = 1; subType = 2; break;
    case 0xfcf: grow = 1; subType = 3; break;
    case 0xfd0: grow = 1; subType = 4; break;
    case 0xfd1: grow = 2; subType = 1; break;
    case 0xfd3: grow = 2; subType = 2; break;
    case 0xfd2: grow = 2; subType = 3; break;
    case 0xfd4: grow = 2; subType = 4; break;
    case 0xfef: grow = 3; subType = 1; break;
    case 0xff1: grow = 3; subType = 2; break;
    case 0xff0: grow = 3; subType = 3; break;
    case 0xff2: grow = 3; subType = 4; break;
    case 0x1045: grow = 4; subType = 1; break;
    case 0x1046: grow = 4; subType = 2; break;
    case 0x1047: grow = 4; subType = 3; break;
    case 0x1048: grow = 4; subType = 4; break;
    case 0xff7: grow = 0; subType = 1; doChangeGrow = 1; break;
    case 0xff8: grow = 0; subType = 2; doChangeGrow = 1; break;
    case 0xff9: grow = 0; subType = 3; doChangeGrow = 1; break;
    case 0xffa: grow = 0; subType = 4; doChangeGrow = 1; break;
    case 0xffc: grow = 1; subType = 1; doChangeGrow = 1; break;
    case 0xffb: grow = 1; subType = 2; doChangeGrow = 1; break;
    case 0xffd: grow = 1; subType = 3; doChangeGrow = 1; break;
    case 0xffe: grow = 1; subType = 4; doChangeGrow = 1; break;
    case 0xfff: grow = 2; subType = 1; doChangeGrow = 1; break;
    case 0x1001: grow = 2; subType = 2; doChangeGrow = 1; break;
    case 0x1000: grow = 2; subType = 3; doChangeGrow = 1; break;
    case 0x1002: grow = 2; subType = 4; doChangeGrow = 1; break;
    case 0x1003: grow = 3; subType = 1; doChangeGrow = 1; break;
    case 0x1005: grow = 3; subType = 2; doChangeGrow = 1; break;
    case 0x1004: grow = 3; subType = 3; doChangeGrow = 1; break;
    case 0x1006: grow = 3; subType = 4; doChangeGrow = 1; break;
    case 0x1f20: grow = 5; subType = 1; doChangeGrow = 1; break;
    case 0x1f22: grow = 5; subType = 2; doChangeGrow = 1; break;
    case 0x1f21: grow = 5; subType = 3; doChangeGrow = 1; break;
    case 0x1f23: grow = 5; subType = 4; doChangeGrow = 1; break;
    case 0x1f28: grow = 6; subType = 1; doChangeGrow = 1; break;
    case 0x1f29: grow = 6; subType = 2; doChangeGrow = 1; break;
    case 0x1f50: grow = 7; subType = 1; doChangeGrow = 1; break;
    case 0x1f52: grow = 7; subType = 2; doChangeGrow = 1; break;
    case 0x1f51: grow = 7; subType = 3; doChangeGrow = 1; break;
    case 0x1f53: grow = 7; subType = 4; doChangeGrow = 1; break;
    case 0x105c: grow = 1; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x105d: grow = 1; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x105e: grow = 1; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x105f: grow = 1; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1060: grow = 2; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1061: grow = 2; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1062: grow = 2; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1063: grow = 2; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1064: grow = 3; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1065: grow = 3; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1079: grow = 3; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x107a: grow = 0; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x107b: grow = 0; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x107c: grow = 0; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x107d: grow = 0; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f1c: grow = 3; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f1d: grow = 4; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f1e: grow = 4; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f1f: grow = 4; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f24: grow = 5; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f26: grow = 5; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f25: grow = 5; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f27: grow = 5; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f2a: grow = 6; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f2b: grow = 6; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f54: grow = 7; subType = 1; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f56: grow = 7; subType = 2; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f55: grow = 7; subType = 3; keepSkill = 1; doChangeGrow = 1; break;
    case 0x1f57: grow = 7; subType = 4; keepSkill = 1; doChangeGrow = 1; break;
    default:
        packet.clear();                                     // LAB_08669808
        packet.put_header(1, 0x20);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.finalize(true);
        Send(packet);
        return;
    }

    // ORIG：0x20/0x11 错误包体在四失败点各自内联一份（clear=6/Send=5 实证），
    // 4.1 无 helper 合并。外层区间守卫 → grow 界守卫 → 职业守卫，逐份展开。
    // P0：ORIG 无 outsideAll 具名槽（链尾 `ja` 直跳，无 bool 物化）⇒ 内联。
    if (!((cur.m_addInfo < 0x105c || 0x1065 < cur.m_addInfo) &&
          (cur.m_addInfo < 0x1079 || 0x107d < cur.m_addInfo) &&
          (cur.m_addInfo < 0x1f1c || 0x1f1f < cur.m_addInfo) &&
          (cur.m_addInfo < 0x1f24 || 0x1f27 < cur.m_addInfo) &&
          (cur.m_addInfo < 0x1f2a || 0x1f2b < cur.m_addInfo) &&
          (cur.m_addInfo < 0x1f54 || 0x1f57 < cur.m_addInfo)))
    {
        if (getCurCharFirstGrowType() == 0)
        {
            packet.clear();
            packet.put_header(1, 0x20);
            packet.put_byte(0);
            packet.put_byte(0x11);
            packet.finalize(true);
            Send(packet);
            return;
        }
    }
    // [CUserOnlyOrig 2026-09-09 复核] ORIG 0x086699b0/0x086699ba 的 `js`/`jg` 目标
    // **不是**紧随其后的包体，而是函数尾部 0x0866a357 的独立包体；职业守卫
    // （0x086699da `je` 跳过）的包体在 0x086699dc。即 ORIG 确为**两份独立包体**
    // （clear/Send 计数 4 份吻合），grow 界那份被 4.1 布局重排到函数末尾 ⇒ 属
    // 块布局 N 类，源码保持两条守卫原形（合并会丢一份包体、gap 2→24）。
    if (grow < 0 || grow > 10)
    {
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.finalize(true);
        Send(packet);
        return;
    }
    if (grow != get_charac_job())
    {
        packet.clear();
        packet.put_header(1, 0x20);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.finalize(true);
        Send(packet);
        return;
    }

    {   // 经验钳制到当前等级区间
        // ORIG 0x08669??：exp 两读（else 分支重取 getCurCharacExp，orig call 计数 2）。
        const int levelLow = G_CDataManager()->get_level_exp(get_charac_level());
        const int levelHigh = G_CDataManager()->get_level_exp(get_charac_level() + 1);
        unsigned int exp = (unsigned int)getCurCharacExp();
        if (exp < (unsigned int)levelLow)
            setCurCharacExp(levelLow);
        else
        {
            exp = (unsigned int)getCurCharacExp();
            if ((unsigned int)levelHigh < exp)
                setCurCharacExp(levelHigh - 1);
        }

        int n = 0;                                     // local_3c（ORIG 先于清零）
        // ORIG +0x0dd0：两遍 `rep stos`（0x138 dwords，常量内联展开；P214③/P312
        // `T arr[N]={0}` 复现）。
        int slots[312] = {0};                          // local_68c（ORIG 0x138 个 int）
        int idxs[312] = {0};                           // local_b6c

        for (int s = 3; s < 9; ++s)
        {
            const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, s);
            if (keepSkill == 0)
            {
                if (!((it.m_addInfo > 0xfc8 && it.m_addInfo < 0xfd5) ||
                      (it.m_addInfo > 0xfee && it.m_addInfo < 0xff3) ||
                      (it.m_addInfo > 0xff6 && it.m_addInfo < 0x1007) ||
                      (it.m_addInfo > 0x1044 && it.m_addInfo < 0x1049) ||
                      (it.m_addInfo > 0x1f1f && it.m_addInfo < 0x1f24) ||
                      (it.m_addInfo > 0x1f27 && it.m_addInfo < 0x1f2a) ||
                      (it.m_addInfo > 0x1f4f && it.m_addInfo < 0x1f54)))
                    continue;
            }
            else
            {
                if (!(it.m_addInfo > 0x105b && it.m_addInfo < 0x1066 &&
                      (it.m_addInfo < 0x1079 || it.m_addInfo > 0x107d) &&
                      (it.m_addInfo < 0x1f1c || it.m_addInfo > 0x1f1f) &&
                      (it.m_addInfo < 0x1f24 || it.m_addInfo > 0x1f27) &&
                      (it.m_addInfo < 0x1f2a || it.m_addInfo > 0x1f2b) &&
                      (it.m_addInfo < 0x1f54 || it.m_addInfo > 0x1f57)))
                    continue;
            }
            slots[n] = s;
            idxs[n] = it.m_addInfo2;
            ++n;
        }
        for (int s = 0x99; s < 0xc9; ++s)
        {
            const Inven_Item it = getCurCharacInvenR()->GetInvenSlot(1, s);
            if (keepSkill == 0)
            {
                if (!((it.m_addInfo > 0xfc8 && it.m_addInfo < 0xfd5) ||
                      (it.m_addInfo > 0xfee && it.m_addInfo < 0xff3) ||
                      (it.m_addInfo > 0xff6 && it.m_addInfo < 0x1007) ||
                      (it.m_addInfo > 0x1044 && it.m_addInfo < 0x1049) ||
                      (it.m_addInfo > 0x1f1f && it.m_addInfo < 0x1f24) ||
                      (it.m_addInfo > 0x1f27 && it.m_addInfo < 0x1f2a) ||
                      (it.m_addInfo > 0x1f4f && it.m_addInfo < 0x1f54)))
                    continue;
            }
            else
            {
                if (!(it.m_addInfo > 0x105b && it.m_addInfo < 0x1066 &&
                      (it.m_addInfo < 0x1079 || it.m_addInfo > 0x107d) &&
                      (it.m_addInfo < 0x1f1c || it.m_addInfo > 0x1f1f) &&
                      (it.m_addInfo < 0x1f24 || it.m_addInfo > 0x1f27) &&
                      (it.m_addInfo < 0x1f2a || it.m_addInfo > 0x1f2b) &&
                      (it.m_addInfo < 0x1f54 || it.m_addInfo > 0x1f57)))
                    continue;
            }
            slots[n] = s;
            idxs[n] = it.m_addInfo2;
            ++n;
        }

        if (n > 0)
        {
            packet.clear();
            packet.put_header(1, 0x12);
            packet.put_byte(1);
            packet.put_byte(0);
            packet.put_byte(n);
            for (int i = 0; i < n; ++i)
            {
                getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slots[i],
                                                  idxs[i], (eItemDelReason)10,
                                                  true);
                packet.put_short(slots[i]);
                packet.put_int(idxs[i]);
            }
            packet.finalize(true);
            Send(packet);
        }

        WongWork::CSkillChanger changer;
        if (!doChangeGrow)
        {
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot,
                get_charac_job());
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot2,
                get_charac_job());
            changer.SkillInitialize(this, 0, true);
        }
        else
        {
            changer.CountElixer(this);
            // ORIG 0x08669f?? 系：getCurCharacSkillW 每使用点重调（else 区 4 次 +
            // 本支 1 次 = orig call 计数 5）；addSkill 实参先 job 后 W。
            ((SkillSlot*)getCurCharacSkillW())
                ->debugCheckGrowTypeSkill(get_charac_level(), get_charac_job(),
                                          subType, keepSkill);
        }

        WongWork::CCalcCharacStat::InitializeStat(
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
            get_charac_job());
        WongWork::CCalcCharacStat::CalcStat(
            reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
            get_charac_job(), subType, keepSkill, get_charac_level());

        int* needElixer = changer.GetNeedElixerCount();
        int* curElixer = changer.GetElixerCount();
        int* ability = changer.GetNeedElixerAbility();
        for (int a = 1; a <= *ability; ++a)
        {
            if (curElixer[a] < needElixer[a])
            {
                WongWork::CCalcCharacStat::ApplyElixerItem(
                    reinterpret_cast<_Additioal_info*>(getCurCharacAddInfoW()),
                    ability[a], needElixer[a] - curElixer[a]);
            }
        }

        set_grow_type((unsigned char)(subType & 0xff),
                      (unsigned char)(keepSkill & 0xff), 0,
                      (eChangeGrowTypeReason)1);
        _postCheckForceChangeGrowType(this, subType, keepSkill);
        SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
        send_skill_info();
        SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    }
}