// ORIG T 0x0859979c _ZN8Sanicova4CPad19isTimeAttackSeccessEv：取全局当前秒
// （结果存局部未参与判定），恒返回 true。
bool CPad::isTimeAttackSeccess()
{
    int curTime = (int)GlobalData::s_systemTime_.getCurSec();
    return true;
}