// CFatigueBatteryHandle.h
// Reconstruction of CFatigueBatteryHandle
// 报告目录 docs/class_func_reports/CFatigueBatteryHandle/（全部函数）：
//   ctor 0x84a7ff8(W 空体) / dtor 0x84a7ffe(W 空体)
//   ComputeFatigueBatteryCharging  0x84a8004
//   ChargingFatigueBattery         0x84a8094
//   ActiveFatigueBattery_EndLevel  0x84a81f4
//   ActiveFatigueBattery_NotEndLevel 0x84a829e
//   ActiveFatigueBattery           0x84a83da
// 常数表本体 ORIG 位于 CDataManager+0x628 = m_serverParameter(+0x68) 内偏移 +0x5c0，
// 即 ServerParameterScript::m_fatigueTable5c0（ServerParameterScript.h 以本结构为成员类型）。
#ifndef GAME_CFatigueBatteryHandle_H
#define GAME_CFatigueBatteryHandle_H

class CUser;

// 疲劳电池常数表（sizeof == 0x1ac；ctor ORIG 0x0836f2cc W）。
// 字段语义均由读取方汇编实证：
//   Compute(0x84a8004): flds +0x04(float 倍率)、movzbl +0x08(除数)、movzwl +0x00(上限)
//   Charging(0x84a8094): movzbl +0x09(最低等级)、+0x10(结束等级)
//   NotEndLevel(0x84a829e): movzwl +0x1a4/+0x1a6/+0x1a8（绝对 mgr+0x7cc/0x7ce/0x7d0）
struct stFatigueBatteryConstantTable_t
{
    stFatigueBatteryConstantTable_t();              // ORIG 0x0836f2cc W

    unsigned short getLimitGrownupBuffCntFatigueBattery(int level) const;  // ORIG 0x084a84b6 W

    unsigned short m_capLimit;        // +0x00 充电量上限（Compute 截断上限）
    // b28c-36: 匿名 union 双视角（P235）——ORIG getLimitGrownupBuffCntFatigueBattery
    // 0x084a84da 尾段 `add $0x8,%edx; movzwl 0x2(%eax,%edx,2),%eax` 为 scaled
    // 数组下标形；`&m_field2 + idx + 8` 指针算术在 4.4 恒 add/add/lea 四段。
    // union 大小 2 字节 ⇒ 后续偏移/sizeof 0x1ac 不变。越界索引照抄 ORIG（P171）。
    union {
        unsigned short m_field2;      // +0x02 limit 表基：元素 L 在本结构 +0x10+2L，
                                      //      相对 m_field2 的下标即 L+7
        unsigned short m_limitTable[1];  // +0x02 数组视角（索引由调用方 level 决定）
    };
    float m_chargeRate4;              // +0x04 充电倍率（ORIG init 为 dword 0 == 0.0f）
    unsigned char m_chargeDivisor8;   // +0x08 除数（ORIG init 0x28）
    unsigned char m_minChargeLevel9;  // +0x09 充电最低等级（低于则不充能）
    int m_fieldc;                     // +0x0c（未见读取方）
    unsigned char m_endLevel10;       // +0x10 结束等级（ORIG init 0x46；双重语义：
                                      //   Charging 满级跳过判定 & limit 表最大下标界）
    // +0x11..+0x1a3：grown-up buff 上限表存储区。元素 L（L>=1）位于绝对偏移
    // +0x10+2L，即 m_data11 下标 2L-1；ctor 仅 memset(this+0x12,0,0x191)。
    unsigned char m_data11[0x193];    // +0x11..+0x1a3
    unsigned short m_activeSwitch1a4;     // +0x1a4 ORIG init 1（NotEndLevel 开关）
    unsigned short m_bonusMultiplier1a6;  // +0x1a6 ORIG init 0（buff 增量倍率）
    unsigned short m_batteryThreshold1a8; // +0x1a8 ORIG init 0（激活门槛）
    unsigned short m_pad1aa;              // +0x1aa 尾垫（ORIG 未初始化）
};

static_assert(sizeof(stFatigueBatteryConstantTable_t) == 0x1ac,
              "stFatigueBatteryConstantTable_t ABI drift (ORIG size 0x1ac)");

class CFatigueBatteryHandle
{
public:
    CFatigueBatteryHandle();  // ORIG 0x84a7ff8（W 弱符号，空体）
    ~CFatigueBatteryHandle();  // ORIG 0x84a7ffe（W 弱符号，空体）
    short ChargingFatigueBattery(class CUser* user, int v);   // ORIG 0x84a8094
    short ActiveFatigueBattery(class CUser* user);            // ORIG 0x84a83da
    // ORIG 实际返回 eax：fistpll(RC=truncate) 低 32 位经 capLimit 无符号截断；
    // 高 32 位不入 ret，故按 int 建模（mangled 名不含返回类型，符号不变）。
    int ComputeFatigueBatteryCharging(int charge, int level, int specialCnt);  // ORIG 0x84a8004
    short ActiveFatigueBattery_NotEndLevel(class CUser* user);  // ORIG 0x84a829e
    short ActiveFatigueBattery_EndLevel(class CUser* user);   // ORIG 0x84a81f4（CUser 升级 0x55 调用）
};

// [b28d-BigGlobalInit] 空类（sizeof==1）：ORIG GlobalData::Init 0x829b033
// `movl $0x1,(%esp); call _ZnwjRKSt9nothrow_t` 证明 nothrow new 只分配 1 字节；
// ORIG C1/D1 0x84a7ff8/0x84a7ffe 均 5 字节空体且无 vptr。原 `char m_pad[8]`
// 使我方 new 分配 8 字节（Init gap 真因之一），已删。

#endif  // GAME_CFatigueBatteryHandle_H
