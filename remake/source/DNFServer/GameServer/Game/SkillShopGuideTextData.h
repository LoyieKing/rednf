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
    SkillShopGuideTextData();                                  // W 0x0891cb4c
    SkillShopGuideTextData(SkillShopGuideTextData const&);     // W 0x08932804
    ~SkillShopGuideTextData();                                 // W 0x0891cb62
    SkillShopGuideTextData& operator=(SkillShopGuideTextData const&);  // W 0x089328e6

    int m_levelMin;        // +0x00 [level range] 首值（推断）
    int m_levelMax;        // +0x04 [level range] 次值（推断）
    std::string m_text;    // +0x08 [text]（objdump 实证）
    unsigned char m_colorR;  // +0x0c [color] 首字节（推断）
    unsigned char m_colorG;  // +0x0d [color] 次字节（推断）
    unsigned char m_colorB;  // +0x0e [color] 第三字节（推断）
};

#endif  // DNF_GAME_SKILLSHOPGUIDETEXTDATA_H_
