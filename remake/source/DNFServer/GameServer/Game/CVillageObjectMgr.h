// CVillageObjectMgr.h
// Reconstruction of village_object::{Zone, IObject, CVillageObjectMgr}
// 布局依据 ORIG：IObject vtable 双纯虚（slot0=reset、slot1=put_object_info，
// 见 CDisjointMachine::{reset 0x85d3e1e, put_object_info 0x85d1cc2}）；
// CVillageObjectMgr 对象即 std::map<Zone, std::list<IObject*>> 本体
// （register_object/on_move_area 的 find/end 直接以 this 调用）。
#ifndef GAME_CVillageObjectMgr_H
#define GAME_CVillageObjectMgr_H

#include <list>
#include <map>

class CUser;
class PacketGuard;

namespace village_object
{

struct Zone
{
    Zone(unsigned char a, unsigned char b);

    unsigned char m_a;
    unsigned char m_b;
};

inline bool operator<(const village_object::Zone& x, const village_object::Zone& y)
{
    if (x.m_a != y.m_a)
        return x.m_a < y.m_a;
    return x.m_b < y.m_b;
}

class IObject
{
public:
    virtual void reset() = 0;                          // vtable +0
    virtual bool put_object_info(PacketGuard& p) = 0;  // vtable +4
protected:
    ~IObject() {}
};
class CVillageObjectMgr
{
public:
    CVillageObjectMgr();                       // ORIG 0x86b6f30
    ~CVillageObjectMgr();                      // ORIG 0x86b6f44
    void on_move_area(class CUser* user);   // ORIG 0x86b71da
    int register_object(village_object::Zone zone, village_object::IObject* object);
                                           // ORIG 0x86b6f58，返回 1
    int unregister_object(village_object::Zone zone, village_object::IObject* object);
                                           // ORIG T 0x86b70d0：map find→std::find→erase，命中返 1

private:
    std::map<village_object::Zone, std::list<village_object::IObject*> > m_objects;
};

}  // namespace village_object

#endif  // GAME_CVillageObjectMgr_H
