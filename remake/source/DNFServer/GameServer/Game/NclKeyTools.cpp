// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NclKeyTools/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r ncl_* license key 工具族（ORIG 自由函数，objdump 直转）。
// ----------------------------------------------------------------------------
// ORIG 符号（df_game_r，全部 T 强符号，同一工具区域 0x085723fc-0x08572e1c）：
// [b28d-D43] AE: 循环变量复用 sock 槽复现 4.1 lifetime 复用（ORIG 0x085723fc 的 -0x10 单槽）。
//   ncl_make_mac(char*)                _Z12ncl_make_macPc    0x085723fc (0xd3)
//   ncl_char_plus(char, int)           _Z13ncl_char_plusci   0x08572581 (0x3c)
//   ncl_check_digit_str(char*)         _Z19ncl_check_digit_strPc 0x08572531 (0x50)
//   ncl_make_date(char*)               _Z13ncl_make_datePc   0x085724cf (0x62)
//   ncl_encrypt_key(char*,char*,char*,char*) _Z15ncl_encrypt_keyPcS_S_S_ 0x085725bd
//   ncl_decrypt_key(char*,char*,char*,char*) _Z15ncl_decrypt_keyPcS_S_S_ 0x085729ce
//   ncl_check_key(char*)               _Z13ncl_check_keyPc   0x08572cb5 (0x167)
//   ncl_check_date(char*, char*)       _Z14ncl_check_datePcS_ 0x08572e1c (0x7a)
// 加密/哈希类（用户规则 2026-08-16）只要求算法正确，不要求 identical。
// 原 ncl_check_key「非空即通过」骨架位于 GameMain.cpp，已删除并由本 TU 提供
// 真实实现（ORIG：decrypt → 长度 0x28 → MAC 等于本机 eth0 → 当日日期落入
// [date1, date2] 闭区间）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

// ncl_char_plus(char, int) —— 数字字符加增量后按 0-9 回绕。
// ORIG：movsbl 符号扩展 → sub $0x30 → add n → i586 除法魔法 %10。
#include "NclKeyTools/ncl_char_plus.inc"
}

// ncl_check_digit_str(char*) —— 全数字返回 strlen，含非数字返回 -1。
// ORIG：len=strlen；逐字符 sub $0x30; cmp $0x9; jbe（无符号 ≤9 合法）→
// 非法时 len=-1 并跳出；jbe 对应 (unsigned)(c-'0') <= 9。
#include "NclKeyTools/ncl_check_digit_str.inc"
}

// ncl_make_mac(char*) —— 本机 eth0 MAC，每字节 "%03d"（3 位十进制）拼接。
// ORIG：socket(AF_INET, SOCK_DGRAM, 0) → struct ifreq(栈 0x30) memset →
//   memcpy 设备名 "eth0"（5 字节含 NUL）→ ioctl(SIOCGIFHWADDR=0x8927) →
//   close → 6 字节各 sprintf("%03d") 到 out，每 3 字符后补 '\0'（rodata
//   0x8ca6a10="eth0" / 0x8ca6a15="%03d"）。
#include "NclKeyTools/ncl_make_mac.inc"
}

// ncl_make_date(char*) —— 当日 "YYYYMMDD"（8 位数字）。
// ORIG：time → localtime_r → sprintf("%04d%02d%02d", year+1900, mon+1, mday)
// （rodata 0x8ca6a1a="%04d%02d%02d"；tm 字段经返回指针访问，P43）。
#include "NclKeyTools/ncl_make_date.inc"
}

// ncl_encrypt_key(char* out, char* mac, char* date1, char* date2)
// ORIG（0x085725bd）：
//   校验 mac=12 位 / date1=8 位 / date2=8 位数字，否则 -1。
//   out[0]='0'；8 轮 × 4 位：mac[j++]→+1、date1[2i]→+2、date1[2i+1]→+3、
//   date2[i]→+1，每轮累加 sum 并输出 v+'0'；i 为奇数时输出 sum%10+'0' 并清零。
//   循环后 mac[8]→+5、mac[9]→+6、尾 checksum=sum%10+'0'、out[40]='\0'。
//   随机化：srand(time)；pos=rand()%10；out[0]=pos+'0'；5 个 checksum 位
//   （偏移 9/0x12/0x1b/0x24/0x27 = 9/18/27/36/39）中 pos==k 者保留原值，
//   其余 4 位用 rand()%10+'0' 覆盖（ORIG 原样，含越界位 36/39 写入）。
//   返回 strlen(out)。
// [b28d-D43] N82/N89 大函数（344/344）：槽序与寄存器分配跨版本差（ORIG
// 0x085725bd；控制流/调用点逐条同构，语义闭合）。
#include "NclKeyTools/ncl_encrypt_key.inc"
}

// ncl_decrypt_key(char* key, char* mac, char* date1, char* date2)
// ORIG（0x085729ce）：
//   key 须 40 位数字（ncl_check_digit_str == 0x28），否则 -1。
//   first = key[0]-'0' 须 ≤4（跳表 0x8ca6a28），否则 -1。
//   first==k 时保留区间 [from, to) 与期望 checksum 位：
//     k=0: [1,9)  位 9；k=1: [0xa,0x12) 位 0x12；k=2: [0x13,0x1b) 位 0x1b；
//     k=3: [0x1c,0x24) 位 0x24；k=4: [0x25,0x27) 位 0x27。
//   区间内数字累加 sum，sum%10 != 期望值 → -1。
//   8 轮解密：mac[i2++]←key[k++] -9、date1[i]←-8、mac[i2++]←-7、date2[i]←-9，
//   i 奇数时 k++（跳过 checksum 位）；循环后 mac←-5、mac←-4；各串补 '\0'。
//   返回 strlen(key)。
// 判例（ADeepY4[1]）：IDENTICAL_AE。槽-局部双向对应律（P0）——原码仅 6 具名局部
//   （macIdx/to/from/first/sum/expected），译出确定性变量时复用 from→k、to→循环计数
//   （Ghidra local_18/local_14 复用），删多余具名 k/i 局部与双初始化；尾串终止位 ORIG
//   用变量 to(=8)/macIdx，非常量下标（Ghidra param_3/param_4[local_14]）；初始化序
//   from→to→macIdx。跳表 rodata 地址经 AE 归一，判 identical_ae。
#include "NclKeyTools/ncl_decrypt_key.inc"
}

// ncl_check_key(char*) —— license key 校验（真实实现替换原「非空即通过」骨架）。
// ORIG（0x08572cb5）：三个 0x80 缓冲 memset → ncl_decrypt_key(key, mac, date1,
//   date2) == 0x28 才继续 → ncl_make_mac 与本机 MAC 相等 → ncl_make_date 的
//   当日字符串落入 [date1, date2] 闭区间（strcmp <0 / >0 判非法）。
#include "NclKeyTools/ncl_check_key.inc"
}

// ncl_check_date(char*, char*) —— 当日字符串在 [a, b] 闭区间内返回 1，否则 0。
// ORIG（0x08572e1c）：memset(0x80) → ncl_make_date → strcmp(today,a) < 0 ||
//   strcmp(today,b) > 0 → 0。
#include "NclKeyTools/ncl_check_date.inc"
}
