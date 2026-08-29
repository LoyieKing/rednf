#ifndef GAME_TOWEROFDESPAIRMGR_H_
#define GAME_TOWEROFDESPAIRMGR_H_
// TowerOfDespair 域权威声明（TowerAPCLeafC3 批次按报告扩展为真实布局）。
// 依据 docs/class_func_reports/：
//   TOD_Layer/TOD_Layer.md             —— ctor ORIG T 0x085fe7b4（sizeof==2）
//   TowerOfDespairMgr/SendAPCInfo.md   —— ORIG T 0x08644338：转发 *(this+0x354)
//   TowerOfDespairMgr/SendMessageToCUser.md —— ORIG T 0x08644366（静态成员）
//   TowerOfDespair_APC_Mgr/*.md        —— mgr 唯一成员 map<TOD_Layer,SendingAPC_Info>
//                                         （Reset_APC_Info 直传 this 给 map::clear；
//                                          ctor 0x086444e6W 仅构造该 map）
//   SendingAPC_Info/*.md               —— sizeof 0x173f4：int@0、PacketBuf@+4、
//                                         _APCInfo@+0x17338（ctor memset 0xbc；
//                                         get_user_charac_name 返回 this+0x1733c）
// GlobalData::s_pTowerOfDespairMgr（0941f754，void*）持有本类实例指针。
//
// 注意：DataManagerInitDepends.h 另有一套【推断】指针容器模型
// （map<int, SendingAPC_Info*>，其 C1/D1 由 CtorProducers_1.cpp 定义）。
// 同一 TU 不得同时包含两套头文件；空 map 内存映像两者一致，
// 本域发送链按真实 ORIG 类型访问（见 TowerAPCLeafC3.cpp）。

#include <map>
#include <set>

#include "PacketBuf.h"
#include "TODLayer.h"      // TOD_Layer（原本头定义，抽出供 SigTypes_D.h 共用）

// ---- _APCInfo：TOD APC 数据块（POD，sizeof 0xbc，ORIG 无独立符号）--------
struct _APCInfo
{
    int m_userCharacNo;                    // +0x00
    char m_userCharacName[0x20];           // +0x04
    int m_lv;                              // +0x24
    int m_job;                             // +0x28
    int m_growType;                        // +0x2c
    char m_guildName[0x18];                // +0x30
    int m_pvpGrade;                        // +0x48
    int m_abilities[22];                   // +0x4c..+0xa3（Set_APC_Info 循环 22 项，
                                           //   末项读 info+0xa0，紧贴 creature_name）
    char m_creatureName[0x10];             // +0xa4
    int m_creatureId;                      // +0xb4
    char m_padB8[4];                       // +0xb8..+0xbb（set_data/memcpy 0xbc 尾部）
};                                          // sizeof 0xbc

class CUser;

class SendingAPC_Info
{
public:
    SendingAPC_Info();                     // ORIG T 0x085fe860
    ~SendingAPC_Info();                    // ORIG T 0x085fe8ca
    void Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info);  // ORIG T 0x085fe906
    void set_data(_APCInfo const& info);   // ORIG T 0x085fe8e0：memcpy(this+0x17338, &info, 0xbc)
    void Send_APC_Info(TOD_Layer const& layer, CUser* user);          // ORIG T 0x085febca

    int get_user_charac_no()               // ORIG W 0x085ff12c：*(int*)this
    {
        return m_userCharacNo;
    }
    char* get_user_charac_name()           // ORIG W 0x085ff136：this+0x1733c
    {
        return m_apcInfo.m_userCharacName;
    }

    int m_userCharacNo;                    // +0x00000
    PacketBuf m_buf;                       // +0x00004（sizeof 0x17334）
    _APCInfo m_apcInfo;                    // +0x17338（ctor memset 0xbc）
};                                          // sizeof 0x173f4

class TowerOfDespair_APC_Mgr
{
public:
    TowerOfDespair_APC_Mgr();     // 定义 CtorProducers_1.cpp
    ~TowerOfDespair_APC_Mgr();    // 定义 CtorProducers_1.cpp
    void Send_APC_Info(TOD_Layer const& layer, CUser* user);          // ORIG T 0x085feee4
    void Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info);  // ORIG T 0x085fed2e
    void Reset_APC_Info();                                            // ORIG T 0x085feed0
    const char* Get_APC_Name_byIndex(TOD_Layer const& layer);         // ORIG T 0x085ff020（LinkE2 补）

    std::map<TOD_Layer, SendingAPC_Info> m_APCInfoMap;   // +0x00（ORIG 唯一成员）
};

class CDungeon;

class TowerOfDespairMgr
{
public:
public:
    void ResetAPCInfo();   // ORIG T 0x0864431e（InterWave1 W7 批）
    void SetAPCInfo(unsigned int idx, const struct _APCInfo& info);  // ORIG T 0x086442ca
    bool IsTowerOfDespairDungeon_ByIndex(int idx) const;  // ORIG T 0x08644272
    TowerOfDespairMgr();                              // ORIG 0x08643ac4
    ~TowerOfDespairMgr();                             // ORIG 0x08643be2
    int LoadScript(const char* dir, const char* path); // ORIG 0x08643e7e（0=成功）
    void _Set_LayerByDungeonIndex(unsigned int layer, const CDungeon* dungeon); // 0x08643c6c
    void _Set_LayerByDungeonIndex(unsigned int layer, unsigned int idx);        // 0x08643c86
    void _Set_APCIndexByLayer(unsigned int layer);     // 0x08643cf4

    CDungeon* Get_CDungeonByLayer(unsigned int layer);          // ORIG T 0x08643d14
    unsigned int Get_DungeonIndexByLayer(unsigned int layer);   // ORIG T 0x08643d76
    int Get_APCIndexByLayer(unsigned int layer);                // ORIG T 0x08643d9c（不在表=−1）
    int GetBaseLayerIndex();                                    // ORIG T 0x086440a0
    int CheckEnterable(CUser* user, const CDungeon* dungeon);   // ORIG T 0x086440ee
    const char* Get_APCNameByLayer(const TOD_Layer& layer);     // ORIG T 0x08643e4a
    void SendAPCInfo(TOD_Layer const& layer, CUser* user);      // ORIG T 0x08644338
    static void SendMessageToCUser(CUser* user, char* msg);     // ORIG T 0x08644366
    void Reload_APC_Info();   // ORIG T 0x08644360（空函数：仅 prologue/epilogue）
    void _layerInfo(CUser* user);   // ORIG T 0x08644470（空函数：仅 prologue/epilogue）
    unsigned short GetMinLv() { return m_minLevel; }   // ORIG W 0x08643a1e：movzwl +0x34e

    CDungeon* m_layerDungeons[101];                    // +0x000
    unsigned int m_dungeonIndexOfLayer[101];           // +0x194
    std::set<int> m_layerSet;                          // +0x328
    std::vector<unsigned int> m_npcLayers;             // +0x340
    unsigned short m_enterableCount;                   // +0x34c
    unsigned short m_minLevel;                         // +0x34e
    unsigned short m_topLayer;                         // +0x350
    TowerOfDespair_APC_Mgr* m_apcMgr;                  // +0x354
    int m_field358;                                    // +0x358
};

#endif  // GAME_TOWEROFDESPAIRMGR_H_
