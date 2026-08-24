#ifndef DNF_INSTANCE_RENTAL_SYSTEM_H
#define DNF_INSTANCE_RENTAL_SYSTEM_H

// InstanceRentalSystem 唯一声明点（§7 合并）。
// ORIG 为类：_ZN20InstanceRentalSystemC2Ev / 10getMachineEi 0x08270348 T；
// 内嵌类型 RentalInfo(0x82719f6)/RentOutput(0x8231c4e)/DeletedRentItem
// (0x8271a3a)/RentalSystemImpl(0x8271a6c)。
// 此前 CUser.h/PacketTypes.h 以 namespace 形式声明，与全局类名冲突，已并入本头。

#include <map>
#include <vector>

class IRentalMachine;

class InstanceRentalSystem
{
public:
    struct RentalInfo
    {
        // ORIG 实测偏移（CUser::removeRentalItemInfoFromInvenBuf @0x8677584：
        //   读 `*(int*)(rental+4)` 物品索引、写 `*(bool*)(rental+10)` 是否已装备）。
        RentalInfo() {}
        ~RentalInfo() {}
        int m_field0;      // +0x00
        int m_itemIndex;   // +0x04  租借物品索引
        char m_pad8;       // +0x08
        bool m_inEquip;    // +0x0a  该租借物品当前位于装备位
        char m_padB;       // +0x0b
    };  // 0xc

    struct DeletedRentItem
    {
        char m_pad[0x10];  // 布局未还原，占位（推断）
    };

    // ORIG RentOutput ctor 0x8231c4e：+0x00 vector<RentalInfo>、+0x0c vector<DeletedRentItem>。
    struct RentOutput
    {
        RentOutput();
        ~RentOutput();
        std::vector<RentalInfo> m_infos;        // +0x00
        std::vector<DeletedRentItem> m_deleted; // +0x0c
    };
    struct RentInElem;  // 元素布局待 pc_room 域批次（Rent 遍历用）

    // ORIG RentInput（InstanceRentalSystem::Rent 0x8270288 实证 +0x0c 取机器键、
    // PcRoom::Rent 实证 +0x10 字节为 log 开关；元素表区布局待租赁批次）。
    struct RentInput
    {
        unsigned char m_pad00[0xc];   // +0x00..0x0b（元素表区，未还原）
        int m_machineKey;             // +0x0c（getMachine 键）
        unsigned char m_logFlag;      // +0x10（非 0 逐物品 log）
    };

    struct RentalSystemImpl
    {
        std::map<int, IRentalMachine*> m_machines;
    };

    InstanceRentalSystem();
    ~InstanceRentalSystem();
    void registerRentalMachine(int idx, IRentalMachine* m);  // ORIG T 0x08270320
    IRentalMachine* getMachine(int idx);                     // ORIG T 0x08270348
    bool Ready();                                            // ORIG T 0x082701dc
    bool Rent(CUser&, const RentInput&, RentOutput&);        // ORIG T 0x08270288


private:
    RentalSystemImpl* m_impl;  // +0x00（ORIG auto_ptr）
};

// ============================================================================
// IRentalMachine 抽象（权威 vtable 序：ORIG _ZTV14IRentalMachine @0x8bfa008、
// 具体类 RentalMachinePcRoom _ZTV @0x8bfb060 实证）：
//   槽0/1 虚析构 D1/D0（0x826fa1a / 0x826fa40）
//   槽2 Rent(CUser&, const InstanceRentalSystem::RentInput&, RentOutput&)
//        —— vptr+8（InstanceRentalSystem::Rent 经此槽分发）
//   槽3 Ready() —— vptr+0xc（InstanceRentalSystem::Ready 经此槽轮询，返回 1=就绪）
//   槽4 CheckRentableItem(unsigned int) —— vptr+0x10
// ============================================================================
class IRentalMachine
{
public:
    virtual ~IRentalMachine() {}

    virtual bool Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output) = 0;   // 槽2
    virtual bool Ready() = 0;                                          // 槽3
    virtual bool CheckRentableItem(unsigned int itemIdx) = 0;          // 槽4
};

#endif  // DNF_INSTANCE_RENTAL_SYSTEM_H
