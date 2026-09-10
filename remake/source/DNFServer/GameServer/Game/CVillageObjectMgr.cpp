// CVillageObjectMgr.cpp
#include "CVillageObjectMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"
#include "PacketBuf.h"   // put_header/get_index/put_byte/finalize

namespace village_object
{

// ORIG 0x86b6f30：仅默认构造成员 std::map<Zone, list<IObject*>> m_objects。
CVillageObjectMgr::CVillageObjectMgr()
    : m_objects()
{
}

Zone::Zone(unsigned char a, unsigned char b)
    : m_a(a)
    , m_b(b)
{
}

// ORIG 0x86b6f44：调用 std::map<...>::~map（成员自动析构）。
CVillageObjectMgr::~CVillageObjectMgr()
{
}

void CVillageObjectMgr::on_move_area(CUser* user)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x086b71da：帧/槽号整体平移（N82）+
    // map 迭代器与视图包临时对象槽向（N93）；Zone 查找/发包字段逐条等价。
    // ORIG 0x86b71da：Zone(curVill, curArea) 查找，命中则向该玩家发送 0xca 视图包。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xca);
    // [L14 round3 判据⑫] widthscan `clr_l orig=2 our=1`：ORIG 0x086b71da 有两个
    // int 零初始化（-0x10 @0x86b72b4、-0xc @0x86b731b）⇒ countIdx 是「声明即 0 +
    // 随后赋值」两句式（P198①），补上该初始化（深槽 -0x10 与 count 的 -0xc 同 ORIG）。
    int countIdx = 0;
    countIdx = packet.get_index();
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

bool CVillageObjectMgr::register_object(Zone zone, IObject* object)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x086b6f58：帧 0x50 vs 0x40 + ORIG 具名局部
    // `add $0x4,%eax; mov %eax,-0xc` 与迭代器槽阶梯（N86/N93）；插入/碰撞/返回
    // 分支逐条等价。
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
