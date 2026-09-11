// ORIG _ZN13user_creature14COverSkillTime9checkTimeEv T 0x0833d65e：
// (float)(getCurSec()-m_time) ∈ (0.0, 20.0)（0x8c2f468 处常量 = 20.0f）→ 1。
// [b28d-23 N·N85] x87 比较 lowering（两处 fucompp/fnstsw）⇒ N。ORIG 0x0833d65e
// [b28d-solo-CCreatureMgr AE] TU 路由 -march=i486 消 x87 比较 lowering；ORIG 0x833d65e
bool COverSkillTime::checkTime()
{
    int curSec = GlobalData::s_systemTime_.getCurSec();
    float diff = static_cast<float>(curSec - m_time);
    return diff > 0.0f && diff < 20.0f;
}
