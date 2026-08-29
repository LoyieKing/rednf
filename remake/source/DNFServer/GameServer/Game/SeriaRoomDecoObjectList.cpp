// ============================================================================
// SeriaRoomDecoObjectList.cpp — seriaDecoObjectInfo 四方法 +
// importSeriaRoomDecoEventObjectList
//
// ORIG（df_game_r，elf32-i386）：
//   seriaDecoObjectInfo::clear()                    W 0x08962422
//   seriaDecoObjectInfo::seriaDecoObjectInfo()      W 0x08962656
//   seriaDecoObjectInfo::~seriaDecoObjectInfo()     W 0x089626e8
//   seriaDecoObjectInfo::seriaDecoObjectInfo(const&) W 0x08962912
//   importSeriaRoomDecoEventObjectList              T 0x0895e79b
//     （_Z34importSeriaRoomDecoEventObjectListP34seriaRoomDecoEventObjectListScriptPKc）
// 报告：docs/class_func_reports/_lt_global_gt_/importSeriaRoomDecoEventObjectList.md
// 语义：脚本 "Event/SeriaRoomDecoEventObjectList.evt"（推断路径，ORIG 无
// 直接调用者）：
//   [event object list] 段内每条 "[object index] <key>" 后跟
//   [stay animation]/[walk animation]/[run animation] 单行 ScanStr，
//   [ramdom animation] 后多行 ScanStr 全部 push 入 m_ramdomAnimations，
//   "[/object index]" 结束并把 (key, info) insert 入 map；EOF 返回 true，
//   打开失败/空指针返回 false。
// ============================================================================
#include "SeriaRoomDecoObjectList.h"

#include <utility>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr

// ---- seriaDecoObjectInfo ----
seriaDecoObjectInfo::seriaDecoObjectInfo() {}

seriaDecoObjectInfo::seriaDecoObjectInfo(const seriaDecoObjectInfo& other)
    : m_stayAnimation(other.m_stayAnimation),
      m_walkAnimation(other.m_walkAnimation),
      m_runAnimation(other.m_runAnimation),
      m_ramdomAnimations(other.m_ramdomAnimations)
{
}

seriaDecoObjectInfo::~seriaDecoObjectInfo() {}

void seriaDecoObjectInfo::clear()
{
    m_stayAnimation.clear();
    m_walkAnimation.clear();
    m_runAnimation.clear();
    m_ramdomAnimations.clear();
}

// ============================================================================
// importSeriaRoomDecoEventObjectList —— ORIG T 0x0895e79b
//   bool (seriaRoomDecoEventObjectListScript*, const char*)
// ============================================================================
bool importSeriaRoomDecoEventObjectList(seriaRoomDecoEventObjectListScript* s,
                                        const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string tmpStr;
    int unusedInt = 0;      // ORIG -0x10（movl $0 实证，未使用）
    bool unusedFlag = false;  // ORIG -0xa（movb $0 实证，未使用）

    while (ScanType(token, true))
    {
        if (token == "[event object list]")
        {
            while (ScanType(token, true))
            {
                seriaDecoObjectInfo info;
                int key = -1;

                if (token == "[object index]")
                {
                    key = -1;
                    info.clear();
                    ScanInt(&key);
                    bool inObject = true;
                    while (inObject)
                    {
                        if (!ScanType(token, true))
                            break;
                        if (token == "[stay animation]")
                        {
                            ScanStr(&info.m_stayAnimation);
                        }
                        else if (token == "[walk animation]")
                        {
                            ScanStr(&info.m_walkAnimation);
                        }
                        else if (token == "[run animation]")
                        {
                            ScanStr(&info.m_runAnimation);
                        }
                        else if (token == "[ramdom animation]")
                        {
                            while (ScanStr(&tmpStr))
                            {
                                std::allocator<char> alloc;
                                std::string tmp(tmpStr.c_str(), alloc);
                                info.m_ramdomAnimations.push_back(std::move(tmp));
                            }
                        }
                        else if (token == "[/object index]")
                        {
                            inObject = false;
                            if (key != -1)
                            {
                                s->m_map.insert(
                                    std::make_pair(key, info));
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
