// b28d-solo-GmMacro verdict=AE ORIG 0x084a8924 orig/ours=97/97，p167 CLEAR 18/18。
// 修法：P222 正向包裹 + P171 外层已判后的死重测照抄 + 三支 Acquire 内联直传；
// 本轮 p167 命中 1 处重载选错：operator<<(GetUID()) 原码选 operator<<(int)
// （ORIG +0x104 call 0x080c8c56；不 cast 会选 0x080e2292 unsigned 版），
// 实参处 static_cast<int> 后调用点差集清零（线格式同为 4 字节，位模式不变）。
void CMacro::SendPacket(char const* data, int len)      // 0x084a8924
{
    int total = len + 8;
    CStreamGuard guard(true);
    // P0：ORIG 无 Stream* 槽——Set(pool->Acquire(...)) 三支各自内联直传。
    // 第二分支首条 `cmpl $0xfff/-0xc; jle big` 为外层已判后的死重测（P171 照抄）；
    // 正向包裹形（P222/探针 /tmp/bl5/probeB.cpp f3）使中段 `cmpl $0x3fff; jg→big岛、
    // fall-through mid(0xf1)` 与 ORIG 块序一致。
    if (total < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("GameMaster.cpp", 0xed));
    }
    else if (total >= 0x1000 && total <= 0x3fff)
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0xf1));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("GameMaster.cpp", 0xf5));
    }
    // ORIG 每 << 先 operator*（+0xc7/+0xf0），uid 经 %ebx 跨 call 直传无槽。
    // CStreamGuard::operator* 返回 this 指针（CStreamGuard.h:27-28）；链式显式调用
    // 令 op<< 的 this=operator* 返回值（%eax→(%esp)），对齐 ORIG +0xf0/+0x10f。
    guard.operator*()->operator<<(0);
    // P167：ORIG +0x104 call 0x080c8c56 = operator<<(int)（GetUID 返回 unsigned int，
    // 原码在实参处显式转 int）；不 cast 会选 operator<<(unsigned int) 0x080e2292。
    guard.operator*()->operator<<(static_cast<int>(m_user->GetUID()));
    guard.operator->()->put_binary(const_cast<char*>(data), len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}