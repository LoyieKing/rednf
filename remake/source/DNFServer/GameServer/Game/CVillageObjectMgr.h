// CVillageObjectMgr.h
// Reconstruction of village_object::{Zone, IObject, CVillageObjectMgr}
// (ORIG _ZN14village_object19CVillageObjectMgr12on_move_areaEP5CUser,
//       _ZN14village_object19CVillageObjectMgr15register_objectENS0_4ZoneEPNS0_7IObjectE).
#ifndef GAME_CVillageObjectMgr_H
#define GAME_CVillageObjectMgr_H

class CUser;

namespace village_object
{

struct Zone
{
    Zone(unsigned char a, unsigned char b);

    unsigned char m_a;
    unsigned char m_b;
};

class IObject
{
public:
    virtual ~IObject() {}
};

class CVillageObjectMgr
{
public:
    void on_move_area(class CUser* user);
    void register_object(village_object::Zone zone, village_object::IObject* object);

private:
    char m_pad[0x10];
};

}  // namespace village_object

#endif  // GAME_CVillageObjectMgr_H