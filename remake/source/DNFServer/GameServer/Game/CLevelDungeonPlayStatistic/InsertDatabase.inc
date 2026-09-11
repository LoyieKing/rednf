// [b28c-32] verdict=DIFF：N18a（ORIG it->second 成员读发 8 字节两段载入，4.4 恒折单条）。
void CLevelDungeonPlayStatistic::InsertDatabase()
{
    if (m_levelPlayMap.size() == 0)
        return;
    // ORIG 栈槽（低→高）：end@-0x18、it@-0x14、guard@-0x10；
    // gcc4.4 按声明序从高地址往下分配，故声明序 guard → it → endIt。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x569),
                       true);
    **guard << 0x107;
    **guard << -1;
    **guard << (int)m_levelPlayMap.size();
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.begin();
    std::map<int, stDungeonPlayInfo>::iterator endIt = m_levelPlayMap.end();
    while (it != endIt)
    {
        // pair 起始即 key；value（stDungeonPlayInfo）@+4，故 value+4=m_userCount、
        // value+8=m_playCount（ORIG 经 iterator::operator-> 读 +0/+4/+8）。
        // [N6S3 2026-09-11 封顶] ORIG 0x0860ef00-0x0860ef06 与 0x0860ef2a-0x0860ef30：
        //   `mov 0x8(%eax),%edx; mov 0x4(%eax),%eax` 后 `mov %eax,%ebx`（取低 32）
        //   / `mov %edx,%ebx`（取高 32）＝ 把 value（pair+4，8 字节）**整体两段
        //   载入**再截半；base 直用 operator-> 返回值（无取址物化）。
        //   我方两个 int 成员（.h:17-18）4.4 恒折单条 `mov 0x4(%eax),%ebx` /
        //   `mov 0x8(%eax),%ebx`（gap=+4 的来源）。函数体内任何 8 字节双关
        //   （`*(unsigned long long*)&it->second`，含先绑引用）都被 -O0 物化
        //   &second：实测多出 `add $0x4,%eax` + 引用入槽/dead 低半 = 每次 +3 条
        //   （134/131，gap=-3，反劣化）⇒ 已回滚。要复现须 .h 级 8 字节视角
        //   （union { struct {int m_userCount; int m_playCount;}; unsigned long long m_pack; }），
        //   禁改 .h ⇒ 上报父泳道 N6Free231to250。
        **guard << it->first;
        **guard << it->second.m_userCount;
        **guard << it->second.m_playCount;
        ++it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_levelPlayMap.clear();
}