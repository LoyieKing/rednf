// ORIG execute 0x084a9de8：以当前系统秒为起点设置 PC 房 premium 区间
// [now, now+dur]，交 WongWork::CHandlePremium::handleSetUserPCRoom 生效。
void CPcroomPremiumCmd::execute()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int end = m_duration + curSec;
    register int regEnd = end;
    register int regCur = curSec;
    WongWork::CHandlePremium::handleSetUserPCRoom(GetUser(), regCur, regEnd);
}