// ===========================================================================
// _checkCreateEventCharacter — 报告 @0x084013b2
// 硬编码 2009-11-20..2010-01-03(+1s) 事件窗；窗内要求非事件角色标志为 0。
// [b22判词·N族块布局] 31/31 助记符集与语义逐条对齐：窗口双 getCurSec 右值内联
// （无 nowSec/inWindow 槽，P0/P183）、flag 判定 0x24、rc4=4@0x5344 两条独立
// return-false 路径。残余差异仅 return false/return true 三尾块的物理排列序
// （ORIG: flag!=0-rc4 → 窗外-rc4 → flag==0-ret1；我们: flag!=0-rc4 → ret1 →
// 窗外-rc4），系编译器块布局 N 族，同指令数、语义等价，结构不可复现。
// ===========================================================================
bool DB_CreateCharac::_checkCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）；
    // getCurSec 每边独立调用（ORIG 两次 call）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        if (sig->m_eventCharacFlag != 0)
        {
            sig->m_resultCode = 4;
            return false;
        }
    }
    else
    {
        sig->m_resultCode = 4;
        return false;
    }
    return true;
}