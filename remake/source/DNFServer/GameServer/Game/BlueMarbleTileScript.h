#ifndef GAME_BLUEMARBLE_TILESCRIPT_H_
#define GAME_BLUEMARBLE_TILESCRIPT_H_


#include <string>

// ============================================================================
// df_game_r BlueMarbleTileScript（G2-4 大富翁格子脚本，尺寸 0xc）
// 布局依据 ORIG clear（0x088d507c）/MapScript::importScript 拷贝序列：
//   +0x00 int m_type     +0x04 char m_char4（“是否可用”标记）
//   +0x08 int m_value
// 构造/clear/importScript/toType 由 BlueMarbleTileScript 批次 TU 交付；
// 本头仅提供声明与布局（本批各 TU 需要 new/delete 与字段直读）。
// ctor 标记 nothrow：ORIG 反汇编中 MapScript::importScript 的 new 表达式
// 没有 _ZdlPv 异常清理（88d55c8 C1 调用无 catch），GCC 4.4 仅在 ctor 声明
// nothrow 时省略该清理，进而保持帧布局（2 个被保存寄存器/0x20）逐字节一致。
// ============================================================================

class BlueMarbleTileScript
{
public:
    enum Type
    {
        Type_0 = 0
    };

    BlueMarbleTileScript() __attribute__((nothrow));
    void clear();
    // ORIG T 0x088d509c（本体待 TileScript 批次；initTileList 经此加载每个格子）。
    // ORIG T 0x088d520a：脚本类型名→枚举（empty=0..kick=0xd，未识别 0）。
    int toType(std::string type);
    // b28d-168a①：ORIG initTileList 调用点为 `call; xor $1,%eax; test %al,%al`，
    // 理想形需 callee bool 化；本件定义属主非本泳道（R1）⇒ 保留 int，调用点封 N。
    bool importScript(const char* path);   // [b29 R20] ORIG initTileList 0x088d6381 调用后
                                           // `xor $1,%eax; test %al,%al; je` ⇒ bool（P248 档①）

    int m_type;    // +0x00
    // [b28d-solo-BlueMarble] char→bool：ORIG getRandomDungeon 0x080d74a5
    // `movzbl 0x4(%eax),%eax; xor $0x1,%eax; test %al,%al; je` ⇒ 字段 bool
    // + 裸 `if(!field)`（P292/P300）。sizeof 0xc / 全部偏移 / mangled 零变化；
    // 写入点仅 BlueMarbleTileScript.cpp clear()/importScript()（0/1 赋值合法）。
    bool m_char4;  // +0x04（“是否可用”标记）
    int m_value;   // +0x08
};

#endif  // GAME_BLUEMARBLE_TILESCRIPT_H_
