#ifndef DNF_GAME_CRIDABLE_H_
#define DNF_GAME_CRIDABLE_H_

// 布局依据 ORIG：ctor 0x82a4406 构造 map<int,DungeonRidableObject> @ +0x00，
// dtor 0x82a441a 析构该 map；CBattle_Field 成员偏移 +0xd0..+0xf0（0x20 字节）。

#include <map>
#include <utility>
#include <vector>

#include "CDungeon.h"  // RidableObject 完整类型（DungeonRidableObject::operator= 形参）

class CParty;
class PacketGuard;
#ifndef DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
#define DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
enum eRidableObjectState { eRidableObjectState_0 = 0 };
#endif

// DungeonRidableObject（map 值类型；ORIG 0x830e9ba ctor：前 6 int 走 RidableObject
// ctor（全 -1），state=0；operator=(RidableObject) W 0x830e9d8 拷贝前 6 字段）。
struct DungeonRidableObject
{
    DungeonRidableObject() : m_x(-1), m_y(-1), m_field0c(-1), m_field10(-1),
                             m_field14(-1), m_field18(0xc8), m_state(0) {}

    DungeonRidableObject& operator=(RidableObject other);  // ORIG 0x830e9d8

    int m_x;        // +0x00
    int m_y;        // +0x04
    int m_field0c;  // +0x08（SetRidableObject 的 map key）
    int m_field10;  // +0x0c
    int m_field14;  // +0x10
    int m_field18;  // +0x14
    int m_state;    // +0x18（eRidableObjectState）
};
class CRidable
{
public:
    CRidable();
    ~CRidable();

    void Clear();                                            // ORIG 0x82fe6bc
    int GetObjectCnt();                                      // ORIG 0x830ea0a（W，非 const：_ZN8CRidable12GetObjectCntEv）
    void GetObjectPos(std::vector<std::pair<int, int> >& out);  // ORIG 0x82fec2c 非const
    void SetRidableObject(RidableScript& script);            // ORIG 0x82fe6d0
    int SetRidableObjectState(int idx, eRidableObjectState state);   // ORIG 0x82feb5e（返回 0/1，Dispatcher_OperateRidableObject 0x08226eed 消费）
    void CheckAppearRidableObject(CParty* party, int a, int b, PacketGuard* p);  // ORIG 0x82fe9c8
    void SendSpawnRidableObject(PacketGuard* p, int a, int b, int c, int d);     // ORIG 0x82fece2

private:
    std::map<int, DungeonRidableObject> m_objects;   // +0x00（本工具链 sizeof=0x18）
    unsigned char m_flag18;                          // +0x18（SetRidableObject 0x82fe6d0 写字节；CheckAppear 状态3条件）
    char m_pad19[3];                                 // +0x19..+0x1b
    int m_count1c;                                   // +0x1c（SetRidableObject 写入脚本对象数）
};


#endif  // DNF_GAME_CRIDABLE_H_
