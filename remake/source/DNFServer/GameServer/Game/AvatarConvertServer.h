#ifndef DNF_GAME_AVATARCONVERTSERVER_H_
#define DNF_GAME_AVATARCONVERTSERVER_H_

// df_game_r Game/ AvatarConvertServer + ARAD::SCRIPT::AvatarConvert。
//
// ORIG（elf32-i386）符号（nm 实证）：
//   ARAD::SCRIPT::AvatarConvert
//     ctor  _ZN4ARAD6SCRIPT13AvatarConvertC1Ev 0x088b1b0e (T)
//     dtor  _ZN4ARAD6SCRIPT13AvatarConvertD1Ev 0x088b1b22 (T)
//     loadScript       _ZN4ARAD6SCRIPT13AvatarConvert10loadScriptEv    0x088b1b36 (T)
//     clearScript      _ZN4ARAD6SCRIPT13AvatarConvert11clearScriptEv   0x088b1dc2 (T)
//     getConvertResult _ZN4ARAD6SCRIPT13AvatarConvert16getConvertResultEi 0x088b1e48 (T)
//     ConvertResult C1/D1（W 内联外置）：0x088b1f42 / 0x088b1f8e
//     GetInfo D1（W trivial）：0x088b3148
//   AvatarConvertServer（全局命名空间）
//     ctor C1 0x081921b6 / dtor D1 0x081921ec / Test 0x0819220c /
//     Load 0x081930ba / Destroy 0x0819312c
//     （CheckCondition 0x08192212 / Convert 0x081926de /
//       StatisticsAvatarDisjoint 0x08193136 /
//       StatisticsEmblemCreate 0x081931dc 尚未还原，见
//       docs/class_func_reports/AvatarConvertServer/）
//
// 类形态：AvatarConvert 为第一个声明的非多态基类，ScriptInterface 多态基占 +0
// （ORIG C1 先构造 +4 脚本子对象再调 ScriptInterface 基——g++ 声明序实证）。
// 布局：+0 vptr；+4 AvatarConvert(0x18)；合计 0x1c。

#include <map>
#include <vector>

#include "Arad_DataManager.h"   // ARAD::ScriptInterface / ARAD::Singleton<T>

namespace ARAD
{
namespace SCRIPT
{

class AvatarConvert
{
public:
    // ORIG：POD 聚合（无用户构造/析构；trivial D1/D0 弱符号由编译器产生）。
    //   +0x00 int   ScanTypeOrInt 返回的数值键
    //   +0x04 short ScanInt #1（物品 index）
    //   +0x06 short ScanInt #2（权重/数量，累加进 m_cumWeights）
    struct GetInfo
    {
        int m_key;          // +0x00
        short m_itemIndex;  // +0x04
        short m_count;      // +0x06
    };

    class ConvertResult
    {
    public:
        ConvertResult();    // ORIG W 0x088b1f42：仅构造两个 vector（标量由调用方 memset）
        ~ConvertResult();   // ORIG W 0x088b1f8e（析构两个 vector）

        int m_grade;                    // +0x00（map key；ScanInt #1）
        int m_totalWeight;              // +0x04（= m_cumWeights.back()）
        std::vector<GetInfo> m_infos;      // +0x08（12B → +0x14）
        std::vector<int> m_cumWeights;     // +0x14（累计和 → +0x20）
        bool m_flag;                    // +0x20（ScanInt()==1；pad +0x21..+0x23）

    private:
        ConvertResult(const ConvertResult&);
        ConvertResult& operator=(const ConvertResult&);
    };

    AvatarConvert();    // ORIG 0x088b1b0e：map ctor
    ~AvatarConvert();   // ORIG 0x088b1b22：仅 map dtor（不 delete 条目，ORIG 如此）

    // 返回：0=成功；-1=loadRDARScriptFile 失败；-2=重复 grade（printf 后返回，
    // ORIG 此路径泄漏该 ConvertResult——忠实保留）。
    int loadScript();       // ORIG 0x088b1b36 "Etc/EmblemRenewal/" avatar_convert_jpn.etc
    void clearScript();     // ORIG 0x088b1dc2：判空 delete ConvertResult* + map.clear()
    ConvertResult* getConvertResult(int grade);   // ORIG 0x88b1e48：未命中 0

private:
    std::map<int, ConvertResult*> m_convertMap;   // +0x00（std::map=0x18，size 0x18）

private:
    AvatarConvert(const AvatarConvert&);
    AvatarConvert& operator=(const AvatarConvert&);
};

}  // namespace SCRIPT

}  // namespace ARAD

class AvatarConvertServer : public ARAD::SCRIPT::AvatarConvert,
                           public ARAD::ScriptInterface
{
public:
    AvatarConvertServer();    // ORIG 0x081921b6：脚本基(+4) → ScriptInterface(+0) → vtable
    ~AvatarConvertServer();   // ORIG 0x081921ec：vtable + 脚本基 dtor

    virtual bool Load();      // ORIG 0x081930ba：cMyTrace + return loadScript()==0
    virtual void Destroy();   // ORIG 0x0819312c：return 1（基类约定 void）
    void Test();              // ORIG 0x0819220c：空（非虚，vtable 仅 Load/Destroy 两槽）

private:
    AvatarConvertServer(const AvatarConvertServer&);
    AvatarConvertServer& operator=(const AvatarConvertServer&);
};

#endif  // DNF_GAME_AVATARCONVERTSERVER_H_
