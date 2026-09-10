UdpThread::UdpThread() {}            // ORIG 0x857c354
// g_bUdp_TH_start —— ORIG BSS 0x943e240；dispatch 置 1 标记 UDP 线程已启动。
bool g_bUdp_TH_start = false;

// ---------------------------------------------------------------------------
// CPacketCounter<1000,10240> —— ORIG 由本 TU（NetworkThread.cpp）实例化
// （C1 0x0857f4dc / D1 0x0857f55a / IncrementPacketCount 0x0857f560 /
// Reset 0x0857f92e，均为 W 弱符号；布局按 Reset/C1 反汇编偏移恢复）。
// ---------------------------------------------------------------------------
template<int Lo, int Hi>
