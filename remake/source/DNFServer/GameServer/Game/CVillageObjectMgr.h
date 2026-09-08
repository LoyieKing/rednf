// CVillageObjectMgr.h
// Reconstruction of village_object::{Zone, IObject, CVillageObjectMgr}
// 布局依据 ORIG：IObject vtable 双纯虚（slot0=include、slot1=put_object_info，
// 见 CDisjointMachine vtable 0x8cc0848：slot0=include 0x85d3de6、
// slot1=put_object_info 0x85d1cc2；reset 0x85d3e1e 为非虚普通成员）；
// CVillageObjectMgr 对象即 std::map<Zone, std::list<IObject*>> 本体
// （register_object/on_move_area 的 find/end 直接以 this 调用）。
#ifndef GAME_CVillageObjectMgr_H
#define GAME_CVillageObjectMgr_H

#include <list>
#include <map>
#include <algorithm>   // std::find_if（collision_detection::detect_collision）

class CUser;
class PacketGuard;

namespace village_object
{

struct Zone
{
    Zone(unsigned char a, unsigned char b);

    // ORIG W 0x086b744c：const 成员 operator<（mangled ZNK...ZoneltERKS0_）。
    bool operator<(const Zone& o) const
    {
        if (m_a < o.m_a)
            return true;
        if (m_a == o.m_a && m_b < o.m_b)
            return true;
        return false;
    }

    unsigned char m_a;
    unsigned char m_b;
};

class IObject
{
public:
    virtual bool include(IObject const& other) const = 0;  // vtable +0（碰撞检测回调）
    virtual bool put_object_info(PacketGuard& p) = 0;  // vtable +4
protected:
    // ORIG 无 IObject D1/D2 符号（派生类 D1 不调基类析构）：
    // 必须保持隐式 trivial 析构，禁止用户定义空体（会生成 IObjectD2 弱符号）。
};
class CVillageObjectMgr
{
public:
    CVillageObjectMgr();                       // ORIG 0x86b6f30
    ~CVillageObjectMgr();                      // ORIG 0x86b6f44
    void on_move_area(class CUser* user);   // ORIG 0x86b71da
    int register_object(village_object::Zone zone, village_object::IObject* object);
                                           // ORIG 0x86b6f58：碰撞检测通过后注册，命中返 1
    int unregister_object(village_object::Zone zone, village_object::IObject* object);
                                           // ORIG T 0x86b70d0：map find→std::find→erase，命中返 1

private:
    std::map<village_object::Zone, std::list<village_object::IObject*> > m_objects;
};

}  // namespace village_object

// ---- collision_detection 碰撞检测模板（ORIG 由 CVillageObjectMgr TU 实例化：
//      ctor 0x86b7a7a、operator() 0x86b8542、detect_collision 0x86b75d7；
//      std::find_if 弱实例 0x86b7a89 / __find_if 0x86b8035）----
// CCollisionDetection<T>：单成员 T m_target（sizeof 4，T=IObject*）；
// operator() 虚调用 m_target->include(*other)（IObject vtable slot0）。
namespace collision_detection
{

template <class T>
class CCollisionDetection
{
public:
    CCollisionDetection(T const& target) : m_target(target) {}
    bool operator()(T const& other) const
    {
        // [b28c-35] NEAR/N7：ORIG 0x086b8542 把 vtable 目标存 %ecx、实参存 %edx、
        // 间接 `call *%ecx`；4.4.7 恒目标 %eax、实参 %ecx。16=16 助记符全等仅寄存器
        // 角色互换（N7；4.4.6 后端可翻，本波勿自切）。
        return m_target->include(*other);
    }

private:
    T m_target;
};

// 缓存 begin/end/结果迭代器（ORIG 栈槽 -0x10/-0x14/-0x1c/-0xc 与两次 end 调用）。
template <class T>
bool detect_collision(std::list<T>& list, T& obj)
{
    CCollisionDetection<T> pred(obj);
    typename std::list<T>::iterator e = list.end();
    typename std::list<T>::iterator b = list.begin();
    typename std::list<T>::iterator f = std::find_if(b, e, pred);
    typename std::list<T>::iterator e2 = list.end();
    // [b28c-L29] ORIG 0x86b75d7：比较结果走 test/je/mov$1/jmp/mov$0 分支物化
    // （P56）——直写 return f!=e2 会被折叠为 call ne; leave; ret。
    if (f != e2)
        return true;
    return false;
}

}  // namespace collision_detection

#endif  // GAME_CVillageObjectMgr_H
