// ============================================================================
// SeriaRoomDecoObjectList.h — seriaDecoObjectInfo 及其宿主脚本类权威布局
//
// 来源（df_game_r，elf32-i386，报告 _lt_global_gt_/importSeriaRoomDecoEventObjectList.md）：
//   seriaDecoObjectInfo::seriaDecoObjectInfo()            W 0x08962656
//   seriaDecoObjectInfo::seriaDecoObjectInfo(const&)      W 0x08962912
//   seriaDecoObjectInfo::~seriaDecoObjectInfo()           W 0x089626e8
//   seriaDecoObjectInfo::clear()                          W 0x08962422
//   importSeriaRoomDecoEventObjectList(script*, char const*) T 0x0895e79b
//     （_Z34importSeriaRoomDecoEventObjectListP34seriaRoomDecoEventObjectListScriptPKc）
// 布局证据：clear 0x08962422 依次 clear +0x00/+0x04/+0x08 三个 string 与
// +0x0c 一个 vector<string>；C1 仅默认构造 4 成员（无 clear 调用，与
// createChracScript::rewardInfo C1 先构造后 clear 不同）；D1 逆序析构
// vector + 3 string；C1ERKS_ 逐成员拷贝。宿主类仅含
// map<int, seriaDecoObjectInfo>（import 内 insert 于 this+0 实证），ORIG
// 无该类 ctor/dtor 符号 → 隐式生成。
// ============================================================================
#ifndef GAME_SERIAROOMDECOOBJECTLIST_H_
#define GAME_SERIAROOMDECOOBJECTLIST_H_

#include <map>
#include <string>
#include <vector>

// ---- seriaDecoObjectInfo（ORIG W：C1 0x08962656 / C1ERKS_ 0x08962912 /
//      D1 0x089626e8 / clear 0x08962422）----
class seriaDecoObjectInfo
{
public:
    seriaDecoObjectInfo();                       // ORIG W 0x08962656
    seriaDecoObjectInfo(const seriaDecoObjectInfo&);  // ORIG W 0x08962912
    ~seriaDecoObjectInfo();                      // ORIG W 0x089626e8
    void clear();                                // ORIG W 0x08962422

    std::string m_stayAnimation;                 // +0x00（[stay animation] ScanStr）
    std::string m_walkAnimation;                 // +0x04（[walk animation] ScanStr）
    std::string m_runAnimation;                  // +0x08（[run animation] ScanStr）
    std::vector<std::string> m_ramdomAnimations; // +0x0c（[ramdom animation] 多行 ScanStr push）
};  // sizeof == 0x18

// ---- seriaRoomDecoEventObjectListScript（ORIG 无 ctor/dtor 弱符号 → 隐式）----
class seriaRoomDecoEventObjectListScript
{
public:
    std::map<int, seriaDecoObjectInfo> m_map;    // +0x00

    friend bool importSeriaRoomDecoEventObjectList(
        seriaRoomDecoEventObjectListScript* s, const char* path);
};  // sizeof == 0xc

#endif  // GAME_SERIAROOMDECOOBJECTLIST_H_
