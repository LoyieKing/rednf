// CVillageObjectMgr.cpp
#include "CVillageObjectMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"
#include "PacketBuf.h"   // put_header/get_index/put_byte/finalize

namespace village_object
{

Zone::Zone(unsigned char a, unsigned char b)
    : m_a(a)
    , m_b(b)
{
}

// ORIG 0x86b6f30：仅默认构造成员 std::map<Zone, list<IObject*>> m_objects。
CVillageObjectMgr::CVillageObjectMgr()
    : m_objects()
{
}

// ORIG 0x86b6f44：调用 std::map<...>::~map（成员自动析构）。
CVillageObjectMgr::~CVillageObjectMgr()
{
}

void CVillageObjectMgr::on_move_area(CUser* user)
{
    // ORIG 0x86b71da：Zone(curVill, curArea) 查找，命中则向该玩家发送 0xca 视图包。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xca);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    for (std::list<IObject*>::const_iterator obj = it->second.begin();
         obj != it->second.end(); ++obj)
    {
        if ((*obj)->put_object_info(packet))
            count += 1;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
}

int CVillageObjectMgr::register_object(Zone zone, IObject* object)
{
    // ORIG 0x86b6f58：无该 Zone 则新建列表插入，否则先 detect_collision 碰撞
    // 检测（命中返 0），通过才 push_back；成功返 1。
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
    {
        std::list<IObject*> newList;
        newList.push_back(object);
        m_objects.insert(std::make_pair(zone, newList));
    }
    else
    {
        if (collision_detection::detect_collision(it->second, object))
            return 0;
        it->second.push_back(object);
    }
    return 1;
}

}  // namespace village_object
