// ============================================================================
// STPassiveObjectAdditionalParticleInfo.h — 附加粒子信息子对象权威布局
//
// 来源（df_game_r，elf32-i386）：
//   STPassiveObjectAdditionalParticleInfo::clear()  W 0x08a3da5a
//   STPassiveObjectAdditionalParticleInfo::C1()     W 0x08a3dad0
// 布局证据：
//   C1 依次 call string::C1(+0x00/+0x04/+0x08) → call clear()（带 unwind 清理）；
//   clear 依次 3 × string::operator=(const char*) 赋 ""（0x8e1dc00 空串）→
//   movl $0 清 +0x0c/+0x10/+0x14/+0x18 → movb $0 清 +0x1c；
//   ImportPassiveObjectScript 中 `new` 分配 `movl $0x20,(%esp)` → sizeof == 0x20。
// 仅被 STPassiveObjectScript::m_particles（vector<指针>）引用，析构为编译器
// 隐式生成（nm 无 D1/D2，且该类从不整体拷贝/销毁于容器外），本头不声明。
// ============================================================================
#ifndef STPASSIVEOBJECTADDITIONALPARTICLEINFO_H_
#define STPASSIVEOBJECTADDITIONALPARTICLEINFO_H_

#include <string>

class STPassiveObjectAdditionalParticleInfo
{
public:
    STPassiveObjectAdditionalParticleInfo();  // ORIG W 0x08a3dad0（cpp 定义）
    void clear();                             // ORIG W 0x08a3da5a（cpp 定义）

    std::string m_s00;   // +0x00（clear=""）
    std::string m_s04;   // +0x04（clear=""）
    std::string m_s08;   // +0x08（clear=""）
    int m_i0c;           // +0x0c（clear=0）
    int m_i10;           // +0x10（clear=0）
    int m_i14;           // +0x14（clear=0）
    int m_i18;           // +0x18（clear=0）
    // [b28d SweepW8LTDBTask2 §8.3] +0x1c 为 bool：ORIG export 0x08a2b4a9
    // `movzbl 0x1c(%eax),%eax; test %al,%al; je`（无 setne 归一化，P73/P321-T3）；
    // uchar 时 4.4 多发 `setne; test` 一对。宽度 1 字节，sizeof/偏移不变。
    bool m_b1c;          // +0x1c（clear=0）
};  // sizeof == 0x20（new 分配 0x20 实证）

static_assert(sizeof(STPassiveObjectAdditionalParticleInfo) == 0x20,
              "STPassiveObjectAdditionalParticleInfo ABI drift (ORIG 0x20)");

#endif  // STPASSIVEOBJECTADDITIONALPARTICLEINFO_H_
