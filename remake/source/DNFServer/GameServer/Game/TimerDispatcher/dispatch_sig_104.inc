int TimerGeneratePowerUpNotiTime::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x0863835a：size/GetPowerUpRemainTime/GetPowerUpTimerPeriod 均由 fresh
    // G_CDataManager()/s_power_manager 直取（P48 去缓存 pm/table）。两 bool 不可合并：
    // bVar1 门内块（IsPowerWarEventOn && cnt!=0，register 态即用），bVar2（槽 -0xd）
    // 门尾 registNextTimer，仅内块 cnt<=period+1 清 0——不受事件门影响（原实现误用
    // 单 active 使事件关时漏 registNextTimer，语义 bug）。
    const int cnt =
        static_cast<int>(G_CDataManager()->m_powerParameter.m_vec20.size());
    int remain = GlobalData::s_power_manager->GetPowerUpRemainTime();
    // [b28d-30] ORIG 086383e9 `mov -0x14(%ebp),%ebx` = 具名槽 -0x14 **且** 4.1 自行
    // 升格 %ebx 跨 G_CDataManager() call；实测纯 register 删槽致 cnt/remain 整体上移
    // （P248 条件②：4.4 不自行升格）⇒ 保槽形，寄存器缓存差记 N7/N82 封顶。
    // [S1 实验] ORIG 086383e9 `mov -0x14(%ebp),%ebx` 在 G_CDataManager() call 之前
    // 装载（实参跨 receiver call 保活）；P290 方向：局部改 int 令下标处发生
    // int→size_t 转换而升格 callee-saved。改前读数 79/76 gap3 ndiff17。
    int period = GlobalData::s_power_manager->GetPowerUpTimerPeriod();
    bool bVar2 = true;
    // [b28d-30] ORIG 086383a2..086383cb：门条件只有 IsPowerWarEventOn && cnt!=0
    // （test;je + cmpl;je 双 je 同落 +0x107），`period < cnt` 是门内**第一层嵌套**
    // （+0x6b cmp;jge 跳过整块）——旧码并入 && 使门少一条 test、嵌套层丢失。
    if (GlobalData::s_power_manager->IsPowerWarEventOn() && cnt != 0)
    {
        // [S1 已试并回滚] 否定式 `!(period >= cnt)` 与正写产物全等（4.4 恒折
        // `cnt > period` 的 jg）⇒ 残余 jl↔jg 属比较方向 fold（P196），记 N18b。
        if (period < cnt)
        {
            remain -= 5;
            GlobalData::s_power_manager->SetPowerUpRemainTime(remain);
            const int notiSec =
                G_CDataManager()->m_powerParameter.m_vec20[period];
            // [b28d-solo-TimerDispatcher] ORIG 08638446 `mov -0xc(%ebp),%eax;
            // cmp -0x18(%ebp),%eax; jl` ⇒ 源形左操作数 notiSec（`notiSec >= remain`）。
            if (notiSec >= remain)
            {
                send_noti_time(notiSec);
                // ORIG 08638455 `addl $0x1,-0x14(%ebp)`（内存自增）⇒ ++period 语句。
                ++period;
                GlobalData::s_power_manager->SetPowerUpTimerPeriod(period);
                if (cnt <= period)
                {
                    // ORIG 0863846a `movl $0x0,-0x14(%ebp)`（period=0 写后未读照抄，P171）。
                    period = 0;
                    bVar2 = false;
                    GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
                }
            }
        }
    }
    if (bVar2)
        registNextTimer(5);
    return 1;
