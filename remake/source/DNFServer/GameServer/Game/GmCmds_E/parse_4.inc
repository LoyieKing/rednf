// ORIG parse 0x084b3d12：仅读 m_parser 判存（mov/test/ret），无副作用。
// 4.1 保留 dead test 无分支；探针实证 `if (p) return;` 形在 4.4 复现
// load/test/pop-ret 同形（/tmp/bl5/probe2.cpp k2）。
void CPcroomRewardReset::parse()
{
    if (m_parser != 0)
    {
        return;
    }
}