#ifndef DNF_GAME_SKILLSHOPGUIDETEXTDATA_H_
#define DNF_GAME_SKILLSHOPGUIDETEXTDATA_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// ORIG W 符号：C1 0x0891cb4c / D1 0x0891cb62 / 拷贝 ctor 0x08932804 / operator= 0x089328e6。
//
// 布局证据（objdump 0x0891cb4c/0x08932804/0x089328e6 + importSkillShopGuideText 报告
// docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md）：
//   C1 仅调 string C1（+0x8）；D1 仅调 string D1。
//   拷贝 ctor/op= 逐字段：+0x0 int、+0x4 int、+0x8 string(C1ERKSs/aSERKSs)、
//   +0xc/+0xd/+0xe 三个 byte（movzbl 拷贝）。
//   new_allocator::construct 以 0x10 分配 ⇒ sizeof = 0x10。
// 字段语义（importSkillShopGuideText 段键处理）：
//   [level range] → ScanInt 写 +0x0/+0x4；[text] → ScanStr 写 +0x8；
//   [color] → ScanInt 三次写 +0xc/+0xd/+0xe（推断）。

#include <string>

class SkillShopGuideTextData
{
public:
    // ORIG 四件套全为 W 弱符号（C1/C2 0x0891cb4c W 0x16、C1ERKS_/C2ERKS_ 0x08932804
    // W 0x5d、D1/D2 0x0891cb62 W 0x16、aSERKS_ 0x089328e6 W 0x60）⇒ 原码是头内 inline
    // 定义，由 ODR 使用点发射弱副本（P233/P204；我方原 .cpp 外联定义发射 T = 判据㉓ 缺口）。
    // 函数体与 ORIG 外联版逐条同（尺寸不变），仅绑定 T→W。
    SkillShopGuideTextData() {}
    SkillShopGuideTextData(SkillShopGuideTextData const& o)
        : m_levelMin(o.m_levelMin), m_levelMax(o.m_levelMax), m_text(o.m_text),
          m_colorR(o.m_colorR), m_colorG(o.m_colorG), m_colorB(o.m_colorB) {}
    ~SkillShopGuideTextData() {}
    SkillShopGuideTextData& operator=(SkillShopGuideTextData const& o)
    {
        m_levelMin = o.m_levelMin;
        m_levelMax = o.m_levelMax;
        m_text = o.m_text;
        m_colorR = o.m_colorR;
        m_colorG = o.m_colorG;
        m_colorB = o.m_colorB;
        return *this;
    }

    int m_levelMin;        // +0x00 [level range] 首值（推断）
    int m_levelMax;        // +0x04 [level range] 次值（推断）
    std::string m_text;    // +0x08 [text]（objdump 实证）
    unsigned char m_colorR;  // +0x0c [color] 首字节（推断）
    unsigned char m_colorG;  // +0x0d [color] 次字节（推断）
    unsigned char m_colorB;  // +0x0e [color] 第三字节（推断）
};

#endif  // DNF_GAME_SKILLSHOPGUIDETEXTDATA_H_
