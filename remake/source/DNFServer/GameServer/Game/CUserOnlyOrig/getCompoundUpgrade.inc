// >>> W7-BLOCKED CUser::get_pvp_exp_of_week —— STPvpResult/getPVPResultRef 视图与 CServerProxyMgr 调用形态未建模
// ---------------------------------------------------------------------------
// 32. CUser::get_pvp_exp_of_week() W 0x08230100【报告缺失·objdump】
// int CUser::get_pvp_exp_of_week()
// {
//     STPvpResult* r = getPVPResultRef();                          // CUserCharacInfo::getPVPResultRefR
//     if (r->m_weekEndExp10 == -1) {
//         return r->m_weekStartExp8;
//     }
//     return r->m_weekStartExp8 - getPVPResultRef()->m_weekEndExp10;   // P48 双调
// }
//
// 33. CUser::get_skill_count() T 0x0866af86
// int CUser::get_skill_count()
// {
//     return ((SkillSlot*)getCurCharacSkillR())->get_skill_count();
// }
//
// 34. CUser::get_SoloPlay_mode() T 0x0868dd44
// int CUser::get_SoloPlay_mode()
// {
//     short v = GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1b);   // CGameOption*
//     return (int)v;
// }
//
// 35. CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const&) T 0x0868dd76
// int CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const& msg)
// {
//     char v = (char)*(const unsigned char*)((const char*)&msg + 0xf);   // movzbl+movsbl
//     if (v == 1) return 0;
//     if (v < 2) {
//         if (v == 0) return 1;
//     }
//     else {
//         if (v == 2) return 2;
//         if (v == 3) return 3;
//     }
//     return 4;
// }
// 【字段建议】MSG_REQ_TO_PEER+0xf：客户端 SoloPlay 请求子类型 byte。
//
// 36. CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE) W 0x085bfd76【报告缺失】
// int CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE type)
// {
//     return m_premium.GetAddExpBonus((int)type);
// }
//
// 37. CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE) W 0x0869733e【报告缺失】
// int CUser::getAddQuestItemDropRateFromPremium(ENUM_PREMIUM_TYPE type)
// {
//     return m_premium.GetAddQuestItemDropRate((int)type);
// }
//
// 38. CUser::GetChangedGiftFatigueQuantity() const W 0x086975ee【报告缺失】
// unsigned char CUser::GetChangedGiftFatigueQuantity() const
// {
//     return (unsigned char)m_field8ebb0;                          // movzbl 直返（P9）
// }
//
// 39. CUser::getCharacAntibotSerialNum() const W 0x0823089c【报告缺失】
// int CUser::getCharacAntibotSerialNum() const
// {
//     return m_field8ec24;                                         // movl 直返
// }
//
// 40. CUser::getCharacSlotCountDefaultInfo() T 0x0868b4fc
// int CUser::getCharacSlotCountDefaultInfo()
// {
//     int v1 = (*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x1a8));
//     if (v1 == 1) {
//         return 1;
//     }
//     if (v1 == 2) {
//         return 3;
//     }
//     int v2 = (*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378));                    // P48 第二次独立调用
//     if (v2 == 5) {
//         return 0x18;
//     }
//     if (v2 == 0xb) {
//         return 0x18;
//     }
//     return 0x11;
// }
//
// 41. CUser::GetChildrensDayEventTargetItemPeriod(int) W 0x08328262【报告缺失】
// int CUser::GetChildrensDayEventTargetItemPeriod(int idx)
// {
//     if (idx > 9) {
//         return -1;
//     }
//     return *(int*)((char*)this + 0x8e0c4 + idx * 4);   /*periods 表 @0x8E0C4*/                     // 基址 0x23830*4+4=0x8E0C4
// }
//
// 42. CUser::GetChildrensDayEventTargetItemSlot(int) W 0x08328240【报告缺失】
// int CUser::GetChildrensDayEventTargetItemSlot(int idx)
// {
//     if (idx > 9) {
//         return -1;
//     }
// 43. CUser::getCompoundUpgrade(CStackableItem const*, stEquipCondition) const T 0x08682082
// ORIG 实测（objdump）：test 用 cond+2（m_amplifySelect）选表组，位移用 cond+1（m_tableIdx）
// <<7 选表行；表基址 = G_CDataManager() 基址 + 行<<7 + 0x8d0/0x18d0/0x28d0/0x38d0
// （CDataManager 数据区裸表，32×int 累加随机分布）。
// [b28d-22 N82] ORIG 0x08682082：p167=CLEAR，gap=-6；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08682082：N82 封顶——JMP/SLOT/ALU 残差
// （p167 CLEAR，gap -6）。
int CUser::getCompoundUpgrade(const CStackableItem* item, stEquipCondition cond) const
{
    // [A06 2026-09-10 负结论·已回滚] ORIG 四分支的地址算术均为**两段常量**
    // （`shl $0x7,%edx; add $0x28c0,%edx; add %edx,%eax; add $0x10,%eax`，0x38c0/0x8c0/
    // 0x18c0 同形）：即「this + (idx*0x80 + 表基) + 表内 +0x10」。试形
    // `tableAddr = (unsigned)(reinterpret_cast<const int*>((unsigned)G_CDataManager() +
    // (idx<<7) + 0x28c0) + 4);` 实测被 4.4 前端折回单条 `add $0x28d0,%eax`
    // （137 条不变，N1 定性）⇒ 回滚。正解=按 ORIG 形把该表建成 **CDataManager 的具名
    // 成员数组**（0x80 字节元素、rates 数组在元素 +0x10），使 4.4 走
    // `idx*stride + 成员偏移` 的折叠路径；属**头改动**，按 R2 只报告不动。
    int upgrade = 0;                                              // -0x1c 默认0
    unsigned int tableAddr = 0;                                   // -0x18
    if (item->IsUse2ndUpgradeTableRecipe()) {
        if (cond.m_amplifySelect == 0) {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x28d0;
            // [b28d-22 修法 P37/P221] ORIG 0x086820d5 `call get_index; mov %eax,%ebx;
            // …构造 cMyTrace; mov %ebx,0x8(%esp)`（实参跨 ctor 保活）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x86e5, 0)(
                "RECIPE LOG : (Item:%u)UpgradeTable2nd", item->get_index());
        }
        else {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x38d0;
            cMyTrace(__PRETTY_FUNCTION__, 0x86eb, 0)(
                "RECIPE LOG : (Item:%u)AmplifyTable2nd", item->get_index());
        }
    }
    else if (cond.m_amplifySelect == 0) {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x8d0;
        cMyTrace(__PRETTY_FUNCTION__, 0x86f4, 0)(
            "RECIPE LOG : (Item:%u)UpgradeTable", item->get_index());
    }
    else {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x18d0;
        cMyTrace(__PRETTY_FUNCTION__, 0x86fa, 0)(
            "RECIPE LOG : (Item:%u)AmplifyTable", item->get_index());
    }
    if (tableAddr != 0) {
        int rnd = get_rand_int(0x2710);                           // 10000
        // [L6 判据⑨重核] ORIG 0x08682286-0x08682295 累加为读-加-写三段
        // （`mov acc,%eax; lea (%edx,%eax,1),%eax; mov %eax,acc`）、比较点
        // `mov acc,%eax; cmp rnd,%eax; jle` 为寄存器形；P322② 的 volatile 累加器
        // 试形实测**反劣**（ndiff 24→62、帧平移 +4、upgrade 初值发射点漂移）⇒ 回滚，
        // 归 P30/N18a 结构性残差（4.1 读-加-写三段 vs 4.4 `add mem` 折形）。
        int acc = 0;                                              // -0x10
        int i = 0;                                                // -0xc
        while (i <= 0x1f) {
            // [H6B 2026-09-11 负结论·已回滚] 试交换操作数 `acc + *p`（欲令 4.4 出 ORIG
            // 0x08682286-95 的读-加-写三段 `mov (%eax),%edx; mov -0x10(%ebp),%eax;
            // lea (%edx,%eax,1),%eax; mov %eax,-0x10(%ebp)`）：实测 **137→137 中性**
            // （4.4 两操作数序同折 `mov (%eax),%eax; add %eax,-0x10(%ebp)`）⇒ 回滚。
            acc = *reinterpret_cast<const int*>(tableAddr + i * 4) + acc;
            // ORIG 0x08682292 `cmp -0x14(%ebp),%eax`（acc 左操作数）+ jle 跳过 ⇒ 原码
            // 形式为 acc > rnd（4.4 对 `rnd < acc` 载入 rnd 出 jge 反向）。
            if (acc > rnd) {
                upgrade = i;                                      // -0x1c = i
                break;
            }
            i++;
        }
    }
    return upgrade;
}