#ifndef DNF_GAME_CRIDABLE_H_
#define DNF_GAME_CRIDABLE_H_

// 布局依据 ORIG：ctor 0x82a4406 构造 map<int,DungeonRidableObject> @ +0x00，
// dtor 0x82a441a 析构该 map；CBattle_Field 成员偏移 +0xd0..+0xf0（0x20 字节）。

#include <map>
#include <utility>
#include <vector>

class RidableScript;
class CParty;
class PacketGuard;
#ifndef DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
#define DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
enum eRidableObjectState { eRidableObjectState_0 = 0 };
#endif

// DungeonRidableObject（map 值类型；偏移自 CheckAppearRidableObject 0x82fe9c8 /
// SetRidableObjectState 0x82feb5e 访问实证；完整布局后续批次细化）。
struct DungeonRidableObject
{
    DungeonRidableObject() : m_x(0), m_y(0), m_field0c(0), m_field10(0),
                             m_field14(0), m_field18(0), m_state(0) {}

    int m_x;        // +0x00
    int m_y;        // +0x04
    int m_field0c;  // +0x08
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
    int GetObjectCnt() const;                                // ORIG 0x830ea0a
    void GetObjectPos(std::vector<std::pair<int, int> >& out) const;  // ORIG 0x82fec2c
    void SetRidableObject(RidableScript& script);            // ORIG 0x82fe6d0
    void SetRidableObjectState(int idx, eRidableObjectState state);   // ORIG 0x82feb5e
    void CheckAppearRidableObject(CParty* party, int a, int b, PacketGuard* p);  // ORIG 0x82fe9c8
    void SendSpawnRidableObject(PacketGuard* p, int a, int b, int c, int d);     // ORIG 0x82fece2

private:
    std::map<int, DungeonRidableObject> m_objects;   // +0x00
    int m_flag18;                                    // +0x18（state==3 出现条件）
    char m_pad1c[4];                                 // +0x1c
};


#endif  // DNF_GAME_CRIDABLE_H_
