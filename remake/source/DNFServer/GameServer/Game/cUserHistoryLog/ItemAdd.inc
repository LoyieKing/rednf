// ORIG 0x8682e84：reason∈{1,2} 且交易中（m_traderCharac[0]!=0）→ 交易格式；
// **否则**（reason∉{1,2}）走常规格式（含 getItemXPos/YPos(m_user+0x79700) 网格坐标）。
// 14 个 hex 字节 = item+0x25..0x32（ChronicleRandomOptionInfo 区）。
//
// [P329 + REAL-BUG 本泳道 2026-09-09] ndiff 589 → 9（orig=371 ours=372）。两处改动，
// 改前 / 改后代码对照如下（"提前具名化 ⇒ 整块重排"的因果只有代码对照才能说明）：
//
// ① REAL-BUG：XPos 段漏了一层 `else`。
//    改前（我方）：if (reason==1||reason==2) { if (m_traderCharac[0]) { LogHistory(交易格式); }
//                                     LogHistory(常规格式); }        // ← 顺序执行，无 else
//    改后：      if (reason==1||reason==2) { if (m_traderCharac[0]) { LogHistory(交易格式); } }
//                                     else { LogHistory(常规格式); }   // ORIG 0x08683144 起
//    可观察后果：reason∉{1,2}（绝大多数常规拾取）时**一条日志都不打**；
//    reason∈{1,2} 时**多打一条**带网格坐标的 Item+ 行。p167 全 CLEAR 抓不到
//    （两分支 callee 多重集本就相同）——只有分支树对照能发现。
//
// ② P329：具名局部毁掉变参右→左求值 ⇒ 整块重排。
//    改前：unsigned int reseal = item.GetReSealCount();
//          unsigned int upgrade = item.GetUpgrade();
//          cUserHistoryLog* posSrc = &m_user->m_historyLog;
//          unsigned short xPos = posSrc->getItemXPos();
//          unsigned short yPos = posSrc->getItemYPos();
//          m_user->LogHistory(fmt, ..., xPos & 0xffff, yPos & 0xffff, ..., upgrade, ...,
//                             reseal, Print()(guid));
//    改后：全部表达式就地内联回实参位（见下方函数体）：
//          m_user->LogHistory(fmt, ..., m_user->m_historyLog.getItemXPos() & 0xffff,
//                             m_user->m_historyLog.getItemYPos() & 0xffff, ...,
//                             item.GetUpgrade(), ..., item.GetReSealCount(), Print()(guid));
//    机理：ORIG(GCC 4.1 -O0) 对变参调用**自末实参起**逐个求值入槽（GUID→reseal→off0→
//    b32..b25→fieldb→upgrade→acct→YPos→XPos），随后紧跟一段纯 mov 装载块；提前具名化
//    把这 4 个 getter 顶到语句边界，装载块与求值块整体错位 ⇒ 单块重排即数百行 diff。
//
// [2026-09-09 N102 翻案后] 残差仅 N107 ×2：ORIG 单条 `lea 0x15(%eax),%ebx` vs 4.4
// `add $0x15,%eax; mov %eax,%ebx`（成员常量偏移取址；tb4/tb5/tb6 共 16 形探针全 add）⇒ 封顶。
// [SweepHistoryLog2 2026-09-09 复核] 再试 12 形（/tmp/print_probe{,2,3,4}.cpp，c6root 4.4.7
// 与 -B cc1plus446bin 双编同结论）：数组成员退化/`&arr[0]`/`reinterpret_cast<Guid*>`/引用 cast/
// 解引用 cast/`static_cast<const void*>`/union 具名视图（m_guid）/具名左值/指针算术/三目/
// packed 非对齐偏移——**实参绑定「引用形参」时 4.4 恒发 `add $0x15,%eax; mov %eax,%ebx`**；
// 唯实参绑定「指针形参（const void*）」时 4.4 才发 lea（探针 p1/p2/p9 实证），而 ORIG
// `operator()(itemGloballyUniqueIdentifier_t const&)` 的 mangled 为 `clERK30…`（nm 0x084ebe82
// 实证，引用）⇒ 引用路径无源形可择 ⇒ N107 封顶。
void cUserHistoryLog::ItemAdd(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemAddReason reason)
{
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (*(0, m_traderCharac) != 0)
        {
            m_user->LogHistory(
                "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                item.GetUpgrade(), (unsigned)item.m_fieldb,
                item.m_random.m_random.m_fields[0].m_field0, item.m_random.m_random.m_fields[0].m_field1, item.m_random.m_random.m_fields[0].m_field2, item.m_random.m_random.m_fields[1].m_field0, item.m_random.m_random.m_fields[1].m_field1, item.m_random.m_random.m_fields[1].m_field2,
                item.m_random.m_random.m_fields[2].m_field0, item.m_random.m_random.m_fields[2].m_field1, item.m_random.m_random.m_fields[2].m_field2, item.m_random.m_random.m_seed9.m_field, item.m_random.m_random.m_fielda.m_field0, item.m_random.m_random.m_fielda.m_field1,
                item.m_random.m_random.m_fielda.m_field2, item.m_random.m_random.m_seedd.m_field,
                item.m_field0,
                item.GetReSealCount(),
                CItemGloballyUniqueIdentifierPrint()(
                    *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
        }
    }
    else
    {
        // ORIG：getItemXPos/YPos(m_user + 0x79700)（history 子对象 +0x40/+0x42 网格坐标）。
        // 全部实参就地内联 ⇒ GCC -O0 右→左求值，与 ORIG 装载序一致
        // （末实参 GUID 最先，XPos 最后）。
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            m_user->m_historyLog.getItemXPos() & 0xffff,
            m_user->m_historyLog.getItemYPos() & 0xffff,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            item.GetUpgrade(), (unsigned)item.m_fieldb,
            item.m_random.m_random.m_fields[0].m_field0, item.m_random.m_random.m_fields[0].m_field1, item.m_random.m_random.m_fields[0].m_field2, item.m_random.m_random.m_fields[1].m_field0, item.m_random.m_random.m_fields[1].m_field1, item.m_random.m_random.m_fields[1].m_field2,
            item.m_random.m_random.m_fields[2].m_field0, item.m_random.m_random.m_fields[2].m_field1, item.m_random.m_random.m_fields[2].m_field2, item.m_random.m_random.m_seed9.m_field, item.m_random.m_random.m_fielda.m_field0, item.m_random.m_random.m_fielda.m_field1,
            item.m_random.m_random.m_fielda.m_field2, item.m_random.m_random.m_seedd.m_field,
            item.m_field0,
            item.GetReSealCount(),
            CItemGloballyUniqueIdentifierPrint()(
                *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
    }
}