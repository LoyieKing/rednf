// [b28d-solo-CUserPremium] P225 修法：ORIG 0x086ae834 的 mask[0]==-1 分支是
// `mov $1,%eax; jmp end`（return true 直返，非 ok=true 落槽）；循环为
// while+break 形（体首 mask==-1 je 出口、尾测 `cmpl $6; setle; test; jne`），
// 旧 for&& 形使 4.4 折成 `jg` 直跳（缺 setle 物化）。
bool CUserPremium::_CheckApply(int type, tm t)
{
    // ORIG 0x86ae834：结果落 bool 槽 local_9（命中不早退，继续迭代）；
    // mask 每判定独立全链（无 def 缓存）；+0x18 即 tm_wday。
    bool ok = false;
    if (g_SPremiumInfo[type].m_applyDayMask[0] == -1)
        return true;
    int i = 0;
    while (i < 7)
    {
        if (g_SPremiumInfo[type].m_applyDayMask[i] == -1)
            break;
        if (g_SPremiumInfo[type].m_applyDayMask[i] == t.tm_wday)
            ok = true;
        ++i;
    }
    return ok;
}