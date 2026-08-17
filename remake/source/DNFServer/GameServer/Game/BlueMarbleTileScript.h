#ifndef GAME_BLUEMARBLE_TILESCRIPT_H_
#define GAME_BLUEMARBLE_TILESCRIPT_H_

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

    int m_type;    // +0x00
    char m_char4;  // +0x04
    int m_value;   // +0x08
};

#endif  // GAME_BLUEMARBLE_TILESCRIPT_H_
