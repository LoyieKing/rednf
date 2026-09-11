// MakeActionClearStateString —— 报告全文转写。ORIG T 0x081246d6。
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x081246d6；ORIG 多 std::string 临时
// [b28d-solo-APSystemOnlyOrig] 判词：**AE**（check_tu routed；137=137；P157 去 rec 引用缓存 + groupIndex vararg 走 unsigned 两段中转后全等）。
// 构造/析构（_ZNSsC1Ev/D1Ev×2）+ _Unwind_Resume，我方缺临时对象与 EH 清理路径。
void DB_UpdateActionPoint::MakeActionClearStateString(std::string& out,
                                                      const _ActionPoint* table,
                                                      unsigned int count)
{
    out.clear();
    // [b28d-27] ORIG 0x081246d6：out.clear() 后立即 `_ZNSsC1Ev`（具名 std::string
    // 局部），函数尾 D1Ev + _Unwind_Resume（EH 清理）；该局部在 ORIG 中未被赋值
    // （死局部，P171 照抄）。
    std::string tmp;
    (void)tmp;
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124704..0x081247d9：**无**行指针/
    // groupChk 具名局部（ORIG 每个字段各自重算 row*0xb 全链，共 7 次，P183
    // 地址重物化铁律）；缓存 rec/groupChk 每处省 6 条 ⇒ 97 vs ORIG 137。
    for (unsigned int row = 0; row < count; ++row)
    {
        if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(
                (_ActionGroupIndex)table[row].m_groupIndex))
        {
            continue;
        }
        char buf[128];
        sprintf(buf, "%d,%d,%d,%d,%d,%d,%d|",
                static_cast<int>(table[row].m_actionNo),
                static_cast<unsigned int>(table[row].m_groupIndex),
                static_cast<int>(static_cast<signed char>(table[row].m_days[0])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[1])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[2])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[3])),
                static_cast<int>(static_cast<signed char>(table[row].m_days[4])));
        out += buf;
    }
}