// ORIG W 0x081010ae (0x45)：objdump 转写（无独立报告）——
// status+0=pool.Remain、+4=pool.Size、+8=reqQueue.size。
// [b28d-solo-W6_IPGDeps] 修法：字段改 int（P211/P248 档③）+ 删三个冗余 cast——
// 前 2 store 与 Remain/Size() 的 int 返回同型 ⇒ LHS-first；第 3 store 与
// queue::size() 的 size_t 异型 ⇒ RHS-first+edx 中转。三 store 装载序与 ORIG 同形。
void CIPGMgr::getIPGStatus(stIPGStatus_t& status)
{
    status.m_field00 = m_pool.Remain();
    status.m_field04 = m_pool.Size();
    status.m_field08 = m_reqQueue.size();
}
