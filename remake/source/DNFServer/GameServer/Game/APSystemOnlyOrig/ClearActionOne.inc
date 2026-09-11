// ClearActionOne —— 报告全文转写。ORIG T 0x08121f1c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x08121f1c：`GetActionPointEtcParameter(_IsGMOn)`
// （uchar 直传 bool 形参，ORIG 只 movzbl×2）->m_actionPointRefs 直读 +0x18，
// **非 const** find/end（_ZNSt3map…4findERS7_/3endEv）+ iterator→const_iterator
// 转换（C1ERKSt17_Rb_tree_iterator），end 侧表达式整段重写（P48 两次
// G_CDataManager/GetActionPointEtcParameter 调用）；旧版经 getActionPointRefs()
// [b28d-solo-APSystemOnlyOrig] 判词：**N18a 封顶**（唯一残差 = `mov;neg` 序 1 对；p167 CLEAR；非 const map find/end + `&action.m_actionNo` 直接取址 + 循环外缓存 size 还原后 145=145）。
// [b28d-round1-L24] 旧 N18a 判词作废：**DIFF→AE**（145=145）—— 同 GM_Reset，
// 取负写法提升到 `-(unsigned int)(signed char)x` 即复现 ORIG 0x0812207x
// `mov %edx,%ecx; neg %ecx`（探针 /tmp/probe_neg.cpp f6）。
// const getter ⇒ const 版 find/end（p167 MISSING 5）。
bool CActionPointManager::ClearActionOne(_ActionPoint& action, int itemIdx)
{
    if (itemIdx != 0)
    {
        std::map<unsigned short, const CActionPointEx*>::const_iterator it =
            G_CDataManager()
                ->GetActionPointEtcParameter(CActionPointManager::_IsGMOn)
                ->m_actionPointRefs.find(*reinterpret_cast<const unsigned short*>(&action.m_actionNo));
        if (it == G_CDataManager()
                      ->GetActionPointEtcParameter(CActionPointManager::_IsGMOn)
                      ->m_actionPointRefs.end())
        {
            return false;
        }
        const CActionPointEx* data = it->second;
        if (data == 0)
        {
            return false;
        }
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08122007：size() 循环外缓存一次
        // （-0x14），i 为 -0x10；逐次 `data->m_itemList[i]`（P157 不缓存引用）。
        bool found = false;
        std::size_t count = data->m_itemList.size();
        for (unsigned int i = 0; i < count; ++i)
        {
            if (data->m_itemList[i] == itemIdx)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            return false;
        }
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x0812206e：**两个独立 if**（零槽即
    // return false，非 break），day 不落槽（P0 表达式直读）；累加用**写回后的
    // 成员**重读（movzbl 0x6(...)+movsbl），非局部副本。
    for (int slot = 0; slot <= 4; ++slot)
    {
        if (action.m_days[slot] == 0)
        {
            return false;
        }
        if (static_cast<signed char>(action.m_days[slot]) < 0)
        {
            // ORIG 先按字节取负，再以有符号 char 累加（含 0x80 情形）。
            m_prevApSum = m_sig.m_apSum;
            action.m_days[slot] = static_cast<unsigned char>(
                -(unsigned int)static_cast<signed char>(action.m_days[slot]));
            m_sig.m_apSum += static_cast<signed char>(action.m_days[slot]);
            m_flag00 = 1;
            return true;
        }
    }
    return false;
}