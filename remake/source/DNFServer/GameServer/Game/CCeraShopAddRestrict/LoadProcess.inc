void CerashopAddRestrict::Manager::LoadProcess(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, CerashopAddRestrict::paramDaily*>& dailyMap,
    SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig)
{
    idxMap.clear();
    // [b28d-solo P291/P214①] 元素地址必须走 SIG 具名数组成员视图
    //（SigTypes_B.h `m_items[1]`）：`&sig->m_items[i]` 让 4.4 产 ORIG 的
    // `... shl $0x2; add sig; add $8`（f16 探针实测）；`(char*)sig + i*0xc + 8`
    // 恒折 `add $8; lea (sig,%eax,1)`（多 1 条 + 寄存器序反）。
    for (unsigned int i = 0; sig->m_count > i; ++i)
    {
        // [b28d-solo P90/P299] 同 InputBuyInfoDetail：ORIG 0x080e1447 的
        // `call _Znwj` 后立即内联 3 条清零、pd 存槽在后 ⇒ value-init 形。
        paramDaily* pd = new paramDaily();
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE::Elem* elem = &sig->m_items[i];
        pd->m_ipgNo = elem->m_ipgNo;
        pd->m_count = elem->m_count;
        pd->m_nextDate = elem->m_nextDate;
        // [b28c-4] ORIG pair 首参照 `mov -0x24(%ebp),%eax`（=&pd->m_ipgNo）⇒
        // 实参是 pd->m_ipgNo 左值（值与 elem[0] 同、地址不同）。
        dailyMap.insert(
            std::map<unsigned int, paramDaily*>::value_type(pd->m_ipgNo, pd));
    }
