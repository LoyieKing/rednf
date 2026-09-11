// ---- AvatarRoulette::getAvatarList ORIG T 0x08911db0 ----
// 汇编形状（range 0x4d）：*isOk = 0；job<0 或 job>10 直接返回；
// 否则 out = m_avatars[job]（vector::operator=，PLT call）后 *isOk = 1。
// 地址计算 = this + job*0xc + 0x14（P30 两步寻址：*3 shl$2 后 +0x10 再 +0x4）。
void AvatarRoulette::getAvatarList(
    bool& isOk, ENUM_CHARACTERJOB job,
    std::vector<std::vector<std::pair<unsigned long, int> > >& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_avatars[job];
        isOk = true;
    }
    return;
}