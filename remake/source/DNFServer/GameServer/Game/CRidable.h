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

private:
    std::map<int, void*> m_objects;   // +0x00（占位：真类型 DungeonRidableObject）
    char m_pad[0x8];                  // +0x18
};


#endif  // DNF_GAME_CRIDABLE_H_
