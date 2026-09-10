// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameUtils/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ 工具函数实现（独立 TU，从 GameStubs.cpp 拆出）。
// 包含：trim / less_TCHAR / TimerEntry / SECOND_TO_MILISECOND / cMyTrace。
#include "CEnvironment.h"
#include "TimerQueue.h"
#include "CLog.h"
#include "GameBase.h"
#include "GlobalData.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _Z22CheckDailyScheduleTimeill 的前提，P338）
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

// ---- CheckDailyScheduleTime（每日调度时间检查，ORIG _Z22CheckDailyScheduleTimeill）----
// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _Z22CheckDailyScheduleTimeill ⇒ 该
// 函数定义须为本 TU 首个强符号（此前 trim 定义在前 ⇒ 键名错为 _GLOBAL__I__Z4trimPc）。
// 纯前置定义顺序（函数体/符号集合/构造序零变化）。
// ORIG：localtime_r 拆 lCurTime，复制 tm 后置 nScheduleTime:00:00，mktime；
// 若原时刻小时 < nScheduleTime 则回退一天；返回 lLastPlayTime <= 该时刻。
#include "GameUtils/CheckDailyScheduleTime.inc"
}

// ---- trim（CEnvironment::parse 用，ORIG 全局函数 _Z4trimPc @0x0829f0ef；返回入参）----
// ORIG 语义：跳过 0x01..0x20 的 ASCII 控制字符（保留 >=0x80 高位字节），
// 子串拷贝到 0x400 栈缓冲后 strcpy 回写（反汇编 0x0829f0ef 逐指令转写）。
char* trim(char* s)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0829f0ef 循环变量/条件（js 与 cmp 0x20 顺序）
    // 与 4.4 槽序镜像（P212/N98 族）；语义等价。
    char buf[0x400];
    int len = strlen(s);
    // [L13-round3 判据⑩] ORIG 0x0829f0ef：i@-0x14(深)/start@-0x10(浅)，`start=i`
    // 形为 `mov -0x14,%eax; mov %eax,-0x10`；我方槽互换（start@-0x14）。
    // 试按「先声明者深」交换声明序以复现 ORIG 槽位（语义不变：两者均 =0 初始化）。
    int i = 0;
    int start = 0;
    // ORIG do-while 形态：先存 start=i 再检查——跳出时 start 即第一个非空白
    // （我们原 while 先检后存，start 落最后一个空白——前导空白未被去除）。
    while (i < len)
    {
        start = i;
        if ((signed char)s[i] < 0 || s[i] > 0x20)
            break;
        ++i;
    }
    int end = len;
    i = len - 1;
    while (i >= 0 && end > start &&
           (signed char)s[i] >= 0 && s[i] <= 0x20)
    {
        --end;
        --i;
    }
    buf[end - start] = 0;
    memcpy(buf, s + start, end - start);
    strcpy(s, buf);
    return s;
}

// ---- trim(std::string&)（ORIG _Z4trimRSs @0x0829f068：字符集仅空格）----
#include "GameUtils/trim.inc"
}

// ---- less_TCHAR（CEnvironment map 比较器）----
#include "GameUtils/operator.inc"
}

// ---- SECOND_TO_MILISECOND（GameBase.h 声明）----
long long SECOND_TO_MILISECOND(int sec)
{
    return (long long)sec * 1000;
}

// ---- cMyTrace（GameTypes.h 声明，ORIG 0x854f718/0x854f746）----
// ORIG 3 参 0x854f718：m_field_c=1、m_field_d=0、m_name/m_line/m_flag 直存。
#include "GameUtils/cMyTrace.inc"
}
// ORIG 5 参 0x854f746：反汇编实测 0x18(%ebp)=第 5 参(bFieldC)→+0x0d、
// 0x1c(%ebp)=第 6 参(bFieldD)→+0x0c——**字段与形参名交叉**（m_field_c 存的是
// bFieldD，m_field_d 存的是 bFieldC）。冒烟实测 logInit 的 bTimestamp 因此差
// 异（ORIG 输出行首是否带CRLF），已按ORIG实测映射修正。
#include "GameUtils/cMyTrace_1.inc"
}
#include "GameUtils/operator_1.inc"
}

// ============================================================================
// 基础时间工具（ORIG-only 符号，被多个 TU 依赖；逐函数核对 ORIG 反汇编）
// ============================================================================

// ---- sleep_ext（ORIG _Z9sleep_extii，0x86b1b48，0x3f 字节）----
// ORIG：构造 {sec, usec} timeval 后 select(0,0,0,0,&tv)（sleep 的 select 实现）。
#include "GameUtils/sleep_ext.inc"
}

// ---- getPrevScheduleTime（ORIG _Z19getPrevScheduleTimeil，0x846c2e9，0x61 字节）----
// ORIG：t = time - hour*3600；localtime_r；置 hour:00:00；mktime。
// 返回 time 当日 hour 时刻的时间戳（未到 hour 则属前一天）。
#include "GameUtils/getPrevScheduleTime.inc"
}

// ---- getDailyInitializeTime（ORIG _Z22getDailyInitializeTimeiii，0x86b1f5a，0x66 字节）----
// ORIG：取 CSystemTime::getCurSec()，localtime_r，置 hour:min:sec，mktime；
// 若结果晚于当前时刻则回退一天（返回当日该时刻，已过则昨日）。
#include "GameUtils/getDailyInitializeTime.inc"
}

// ---- __GetCurrentDNFDate（ORIG _Z19__GetCurrentDNFDateR2tm，0x86b1fc0，0x5f 字节）----
// ORIG：cur = getCurSec()；若 localtime 后 tm_hour < CEnvironment+0x37c
// （m_serverEnvir.m_fatigueTime，即每日调度小时）则回退一天再 localtime。
#include "GameUtils/__GetCurrentDNFDate.inc"
}

// ---- ComputeDaysScheduleTime（ORIG _Z23ComputeDaysScheduleTimeill，0x846c144，0x1a5 字节）----
// ORIG：分别以 curTime/lastTime 求当日 hour:00:00 基准时刻（未到 hour 回退一天），
// 差值 /86400 取整天数，负数归 0。
#include "GameUtils/ComputeDaysScheduleTime.inc"
}

// ---- GetRandomRange（ORIG W 0x08539d90 _Z14GetRandomRangeii）----
// a<b: rand()%(b-a+1)+a；b<a: rand()%(a-b+1)+b（C 有符号取模语义）。
#include "GameUtils/GetRandomRange.inc"
}
