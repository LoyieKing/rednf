// STComputeDamageVariable 权威布局（DspD1 批 D1 解阻产出）。
// 依据：ORIG ctor _ZN23STComputeDamageVariableC1Ev W @0x08890936 (size 0x479) 逐字段初始化
//      + computeHpDamageSDC T @0x08095c32 (size 0xac8) 内全部偏移访问。
// sizeof 证据：ctor 最后一次写入 0x1f8(float)+4 = 0x1fc；全二进制无 >0x1fb 访问；4 字节对齐。
// → sizeof(STComputeDamageVariable) == 0x1FC
// 构造初始化值均来自 0x08890936..0x08890dae 逐条 mov/movb/movl/fldz。
// 字段命名：能定语义的具名；不可辨的保持 m_field 前缀+偏移。
// 注意：0xf..0x2a 为 float[7]（[4..6] 是暴击率乘子，由 m_char0D3/0D4/0D5 门控）；
//       0xd6..0x1a5 为 float[2][26] 暴击矩阵（[0]=非暴击行@0xd6，[1]=暴击行@0x13e）。

#ifndef DNF_GAME_STCOMPUTEDAMAGEVARIABLE_H_
#define DNF_GAME_STCOMPUTEDAMAGEVARIABLE_H_
#include <string.h>  // memset（ctor 元素启用标志区）

// getAbilityConvertRateSDC 的第二参枚举（SDC 函数族用值 3..0xA）；防与他处权威定义重复。
#ifndef DNF_ENUM_CONVERT_TABLE_TYPE_DEFINED
enum ENUM_CONVERT_TABLE_TYPE
{
    CONVERT_TABLE_0 = 0,
    CONVERT_TABLE_3 = 3,
    CONVERT_TABLE_4 = 4,
    CONVERT_TABLE_5 = 5,
    CONVERT_TABLE_6 = 6,
    CONVERT_TABLE_7 = 7,
    CONVERT_TABLE_8 = 8,
    CONVERT_TABLE_9 = 9,
    CONVERT_TABLE_A = 0xA
};
#define DNF_ENUM_CONVERT_TABLE_TYPE_DEFINED 1
#endif

// 布局修正（Layout_D 2026-08-30）：ORIG 为 pack(1) 紧凑布局——ctor 实证 float
// m_f006@0x006（非 4 对齐）、critMatrix float@0x0d6、double m_d1AA@0x1aa（非 8 对齐），
// 无 pack 时自然对齐会把 m_f006 推到 0x008 致后续全错位 +2。
#pragma pack(push, 1)
class STComputeDamageVariable
{
public:
    // ORIG W 0x08890936（C1/C2 同址，size 0x479）：按下方字段注释逐条初始化。
    STComputeDamageVariable()
    {
        m_field000 = 0;
        m_char004 = 0;
        m_damageType = 0;
        m_f006 = 1.0f;
        memset(m_elemEnable, 0, 4);
        m_char00E = 0;
        m_f00F[0] = 0.0f;
        m_f00F[1] = 1.0f;
        m_f00F[2] = 1.0f;
        m_f00F[3] = 1.0f;
        // [b28d-solo-CreatureScript] ORIG 0x088909b0 循环 i<=2 写
        // 0xf+4*(i+4)=0x1f/0x23/0x27 ⇒ m_f00F[4..6] 为 3 次循环而非逐写。
        for (int i = 0; i < 3; ++i)
            m_f00F[i + 4] = 1.0f;
        m_f02B = 1.0f;
        m_field02F = 0;
        m_field033 = 0;
        m_f037 = 1.0f;
        m_f03B = 1.0f;
        m_f03F = 0.0f;
        m_f043 = 0.0f;
        m_f047 = 1.0f;
        m_f04B = 1.0f;
        m_f04F = 1.0f;
        m_f053 = 1.0f;
        m_f057 = 1.0f;
        m_f05B = 1.0f;
        m_f05F = 1.0f;
        m_field063 = 0;
        m_field067 = 0;
        m_f06B = 1.0f;
        m_f06F = 1.0f;
        m_field073 = 0;
        m_f077 = 1.0f;
        m_field07B = 0;
        for (int i = 0; i < 4; ++i)
            m_elemAbility[i] = 0;
        m_f08F = 1.0f;
        m_field093 = 0;
        m_field097 = 1;
        m_field09B = 0;
        m_field09F = 0;
        m_field0A3 = 1;
        m_field0A7 = 0;
        m_f0AB = 0.0f;
        m_f0AF = 0.0f;
        m_f0B3 = 1.0f;
        m_physMDefBase = 0;
        m_magicMDefBase = 0;
        m_physMDefAdd = 0;
        m_magicMDefAdd = 0;
        m_field0CF = 0;
        m_char0D3 = 0;
        m_char0D4 = 0;
        m_char0D5 = 0;
        // [b28d-solo-CreatureScript] ORIG 0x08890bd8 单循环 i<=0x19 同轮写两行
        // （0x6+4*(i+0x34)=0xd6 与 0x6+4*(i+0x4e)=0x13e）⇒ 非嵌套二维循环。
        for (int i = 0; i < 26; ++i)
        {
            m_critMatrix[0][i] = 1.0f;
            m_critMatrix[1][i] = 1.0f;
        }
        m_f1A6 = 1.0f;
        m_d1AA = 0.0;
        m_objTypeByte = 0;
        m_char1B3 = 1;
        m_char1B4 = 1;
        m_char1B5 = 0;
        m_char1B6 = 0;
        m_char1B7 = 0;
        m_char1B8 = 0;
        m_char1B9 = 0;
        m_field1BA = 0;
        m_attackerObjType = 0;
        m_defenderObjType = 0;
        m_field1C6 = 0;
        m_uid = 0;
        m_isCritical = 0;
        m_char1CF = 0;
        m_critDamage = 0;
        m_char1D4 = 0;
        m_field1D5 = 1;
        m_randSeed = 0;
        m_attackElem = 0;
        m_char1DE = 0;
        m_char1DF = 0;
        m_char1E0 = 0;
        m_field1E1 = 0;
        m_char1E5 = 0;
        m_expectedHp = 0;
        m_char1EA = 0;
        m_field1EB = 0;
        m_char1EF = 0;
        m_f1F0 = 1.0f;
        m_f1F4 = 0.0f;
        m_f1F8 = 0.0f;
        // [b28d-solo-CreatureScript] ORIG 0x08890d94/0x08890d9f：两 reduce 率
        // 在 ctor **最末**写 0xbf/0xc3（非中间）。
        m_physReduceRate = 0.0f;
        m_magicReduceRate = 0.0f;
    }

    /* 0x000 */ float  m_field000;          // ctor: 0；ORIG C1 edx 中转形+调用点 flds 直读实证 float
    /* 0x004 */ unsigned char m_char004;    // ctor: 0；computeHpDamageSDC 读作 bool（攻击方等级基准开关）
    /* 0x005 */ unsigned char m_damageType; // ctor: 0；0=物理/1=魔法，switch 值
    /* 0x006 */ float  m_f006;              // ctor: 1.0f；final *= (m_f006*0.01f + 1)
    /* 0x00A */ unsigned char m_elemEnable[4];  // ctor: memset 0x0a..0x0d=0；元素启用标志
    /* 0x00E */ unsigned char m_char00E;    // ctor: 0；门控魔防/防御率路径
    /* 0x00F */ float  m_f00F[7];           // [0]=0.0f, [1..3]=1.0f, [4..6]=1.0f（暴击率表 0x1f/0x23/0x27 槽）
    /* 0x02B */ float  m_f02B;              // ctor: 1.0f
    /* 0x02F */ float  m_field02F;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x033 */ float  m_field033;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x037 */ float  m_f037;              // ctor: 1.0f；物理分支第二乘子 (m_f037*0.01f+1)
    /* 0x03B */ float  m_f03B;              // ctor: 1.0f；魔法分支第二乘子 (m_f03B*0.01f+1)
    /* 0x03F */ float  m_f03F;              // ctor: 0；物理力量 s2（CONVERT_TABLE_3）
    /* 0x043 */ float  m_f043;              // ctor: 0；魔法力量 s2（CONVERT_TABLE_5）
    /* 0x047 */ float  m_f047;              // ctor: 1.0f
    /* 0x04B */ float  m_f04B;              // ctor: 1.0f
    /* 0x04F */ float  m_f04F;              // ctor: 1.0f
    /* 0x053 */ float  m_f053;              // ctor: 1.0f
    /* 0x057 */ float  m_f057;              // ctor: 1.0f
    /* 0x05B */ float  m_f05B;              // ctor: 1.0f
    /* 0x05F */ float  m_f05F;              // ctor: 1.0f；魔防/防御率乘子
    /* 0x063 */ int    m_field063;          // ctor: 0；物理 final 附加
    /* 0x067 */ int    m_field067;          // ctor: 0；魔法 final 附加
    /* 0x06B */ float  m_f06B;              // ctor: 1.0f；dmg *= m_f06B
    /* 0x06F */ float  m_f06F;              // ctor: 1.0f
    /* 0x073 */ float  m_field073;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x077 */ float  m_f077;              // ctor: 1.0f；!=1.0f 时 dmg *= m_f077
    /* 0x07B */ int    m_field07B;          // ctor: 0；!=0 时 dmg += (float)m_field07B
    /* 0x07F */ int    m_elemAbility[4];    // ctor 循环 0；元素减益表输入
    /* 0x08F */ float  m_f08F;              // ctor: 1.0f；元素减益乘子
    /* 0x093 */ int    m_field093;          // ctor: 0
    /* 0x097 */ int    m_field097;          // ctor: 1
    /* 0x09B */ float  m_field09B;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x09F */ int    m_field09F;          // ctor: 0
    /* 0x0A3 */ int    m_field0A3;          // ctor: 1
    /* 0x0A7 */ float  m_field0A7;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x0AB */ float  m_f0AB;              // ctor: 0；物理力量 s1（CONVERT_TABLE_4）
    /* 0x0AF */ float  m_f0AF;              // ctor: 0；魔法力量 s1（CONVERT_TABLE_6）
    /* 0x0B3 */ float  m_f0B3;              // ctor: 1.0f；final 附加乘子
    /* 0x0B7 */ int    m_physMDefBase;      // ctor: 0；物理魔防基数
    /* 0x0BB */ int    m_magicMDefBase;     // ctor: 0；魔法魔防基数
    /* 0x0BF */ float  m_physReduceRate;    // ctor: 0；物理 dmg -= dmg*rate
    /* 0x0C3 */ float  m_magicReduceRate;   // ctor: 0；魔法 dmg -= dmg*rate
    /* 0x0C7 */ int    m_physMDefAdd;       // ctor: 0；物理魔防加值
    /* 0x0CB */ int    m_magicMDefAdd;      // ctor: 0；魔法魔防加值
    /* 0x0CF */ float  m_field0CF;          // ctor: 0；ORIG C1 edx 中转形实证 float
    /* 0x0D3 */ unsigned char m_char0D3;    // ctor: 0；门控 f[4]（0x1f）暴击率乘子
    /* 0x0D4 */ unsigned char m_char0D4;    // ctor: 0；门控 f[5]（0x23）
    /* 0x0D5 */ unsigned char m_char0D5;    // ctor: 0；门控 f[6]（0x27）
    /* 0x0D6 */ float  m_critMatrix[2][26]; // ctor 循环全 1.0f；[0][*]@0xd6，[1][*]@0x13e
    /* 0x1A6 */ float  m_f1A6;              // ctor: 1.0f；dmg *= m_f1A6
    /* 0x1AA */ double m_d1AA;              // ctor: 0.0
    /* 0x1B2 */ unsigned char m_objTypeByte;// ctor: 0；==0xE 清随机基、==4/5 跳过力量乘链
    /* 0x1B3 */ unsigned char m_char1B3;    // ctor: 1
    /* 0x1B4 */ unsigned char m_char1B4;    // ctor: 1
    /* 0x1B5 */ unsigned char m_char1B5;    // ctor: 0
    /* 0x1B6 */ unsigned char m_char1B6;    // ctor: 0
    /* 0x1B7 */ unsigned char m_char1B7;    // ctor: 0
    /* 0x1B8 */ unsigned char m_char1B8;    // ctor: 0
    /* 0x1B9 */ unsigned char m_char1B9;    // ctor: 0
    /* 0x1BA */ int    m_field1BA;          // ctor: 0
    /* 0x1BE */ int    m_attackerObjType;   // ctor: 0；isObjectType 门控
    /* 0x1C2 */ int    m_defenderObjType;   // ctor: 0；isObjectType 门控防御链
    /* 0x1C6 */ int    m_field1C6;          // ctor: 0；isObjectType(…, 0x111) 门控 critMatrix 行
    /* 0x1CA */ int    m_uid;               // ctor: 0；对象 uid
    /* 0x1CE */ unsigned char m_isCritical; // ctor: 0；checkIsCriticalSDC 写回
    /* 0x1CF */ unsigned char m_char1CF;    // ctor: 0；!=0 进入暴击扣血分支
    /* 0x1D0 */ int    m_critDamage;        // ctor: 0；(int)(dmg*1.1f - dmg)
    /* 0x1D4 */ unsigned char m_char1D4;    // ctor: 0
    /* 0x1D5 */ int    m_field1D5;          // ctor: 1；dmg *= (float)m_field1D5 * 0.01f
    /* 0x1D9 */ int    m_randSeed;          // ctor: 0；SDC_Rand 种子
    /* 0x1DD */ unsigned char m_attackElem; // ctor: 0；攻击元素（==4 特判）
    /* 0x1DE */ unsigned char m_char1DE;    // ctor: 0
    /* 0x1DF */ unsigned char m_char1DF;    // ctor: 0；门控 dmg*0.25 回加
    /* 0x1E0 */ unsigned char m_char1E0;    // ctor: 0；门控 dmg += (float)m_field1E1
    /* 0x1E1 */ int    m_field1E1;          // ctor: 0
    /* 0x1E5 */ unsigned char m_char1E5;    // ctor: 0
    /* 0x1E6 */ int    m_expectedHp;        // ctor: 0；computeHpDamageSDC 输出写入点
    /* 0x1EA */ unsigned char m_char1EA;    // ctor: 0；被读取但结果未用
    /* 0x1EB */ int    m_field1EB;          // ctor: 0
    /* 0x1EF */ unsigned char m_char1EF;    // ctor: 0；门控 dmg *= m_f1F0
    /* 0x1F0 */ float  m_f1F0;              // ctor: 1.0f
    /* 0x1F4 */ float  m_f1F4;              // ctor: 0；dmg += m_f1F4
    /* 0x1F8 */ float  m_f1F8;              // ctor: 0；dmg += dmg*m_f1F8/100.0f
};
#pragma pack(pop)

#endif  // DNF_GAME_STCOMPUTEDAMAGEVARIABLE_H_
