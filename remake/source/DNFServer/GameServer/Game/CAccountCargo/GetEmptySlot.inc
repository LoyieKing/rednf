// ---- 查询 ----
int CAccountCargo::GetEmptySlot()
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828a580：28/28 全等；i486 路由
    // （Main 已落 dnf_game_cxx.sh）消三目 MIN_EXPR cmova，复现 ORIG
    // `cmp $0x38,%eax; jbe; mov $0x38,%eax`（P230）。p167 CLEAR（0 call）。
    int limit = (int)(m_capacity > 0x38 ? 0x38 : m_capacity);
    for (int i = 0; i < limit; ++i) {
        if (m_slots[i].m_addInfo == 0)
            return i;
    }
    return -1;
}