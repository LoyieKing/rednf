bool CUserPremium::CheckPremium(int type) const
{
    // ORIG（W，汇编实证）：直接 this+type*0x14 读 m_type，仅判非 0；
    // 不查 state（state==3 已过期条目仍返回 true），无边界检查。
    // ORIG 0x80e5fe6：test;je;mov$1;jmp;mov$0 分支物化；原 !=0 折 setne（P22/P101）。
    if (m_entries[type].m_type != 0)
        return true;
    return false;
}