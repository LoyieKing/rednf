#ifndef DNF_INSTANCE_RENTAL_SYSTEM_H
#define DNF_INSTANCE_RENTAL_SYSTEM_H

// InstanceRentalSystem 唯一声明点（§7 合并）。
// ORIG 为类：_ZN20InstanceRentalSystemC2Ev / 10getMachineEi 0x08270348 T；
// 内嵌类型 RentalInfo(0x82719f6)/RentOutput(0x8231c4e)/DeletedRentItem
// (0x8271a3a)/RentalSystemImpl(0x8271a6c)。
// 此前 CUser.h/PacketTypes.h 以 namespace 形式声明，与全局类名冲突，已并入本头。

#include <map>
#include <memory>
#include <vector>

class IRentalMachine;

class CUser;   // Rent/CheckRentableItem 形参（前置即可）

class InstanceRentalSystem
{
public:
    struct RentalInfo
    {
        // ORIG C1 0x082719f6（报告缺失，objdump 直转）：清 +0 字节/+2 半字/+4 int/
        //   +8..+0b 四字节，+0xc=-1。removeRentalItemInfoFromInvenBuf 0x8677584
        //   实证 +0x04=物品索引、+0x0a=是否已装备。
        RentalInfo();
        ~RentalInfo() {}
        unsigned char m_flag0;      // +0x00（ProcessEquip 写 3 / ProcessInsert 写 0）
        unsigned char m_pad1;       // +0x01
        unsigned short m_slot;      // +0x02（插入/装备槽位）
        int m_itemIndex;            // +0x04 租借物品索引
        unsigned char m_field8;     // +0x08
        unsigned char m_field9;     // +0x09
        unsigned char m_inEquip;    // +0x0a 该租借物品当前位于装备位
        unsigned char m_fieldB;     // +0x0b
        int m_rentItemId;           // +0x0c（ctor 置 -1）
    };  // sizeof 0x10

    struct DeletedRentItem
    {
        // ORIG C1 0x08271a3a（报告缺失，objdump 直转）：清 +0 字节/+2 半字/+4 int。
        DeletedRentItem();
        unsigned char m_bEquip;          // +0x00
        unsigned char m_pad1;            // +0x01
        unsigned short m_slot;           // +0x02
        int m_itemId;                    // +0x04
    };  // sizeof 8

    // ORIG RentOutput ctor 0x8231c4e：+0x00 vector<RentalInfo>、+0x0c vector<DeletedRentItem>。
    struct RentOutput
    {
        RentOutput();
        ~RentOutput();
        std::vector<RentalInfo> m_infos;        // +0x00
        std::vector<DeletedRentItem> m_deleted; // +0x0c
    };
    // ORIG RentInElem 嵌套于 RentInput（nm：_ZN20InstanceRentalSystem9RentInput10RentInElem
    // C1Ev）；PcRoom/Dimension Rent 报告实证 elem+0/+1 下标、+2 非零走装备路径、
    // RentInput 首成员即元素表 vector。
    struct RentInput
    {
        // 元素：+0/+1 下标、+2 装备标志、+3 类型、+4 物品索引（缺省 -1）。
        struct RentInElem
        {
            RentInElem();             // ORIG C1 W 0x0822f02e（清 4 字节，+4=-1）
            unsigned char m_sectionIdx;   // +0x00 外层下标
            unsigned char m_itemIdx;      // +0x01 内层下标
            unsigned char m_bEquip;       // +0x02 非 0 → 装备区
            unsigned char m_type;         // +0x03 Dimension::GetRentItemInfo ==1 走 GiveList
            int m_rentItemId;             // +0x04 缺省 -1
        };  // sizeof 8

        // ORIG RentInput：首成员即 vector<RentInElem>（ORIG begin/end 直接作用于
        // input+0）；InstanceRentalSystem::Rent 0x8270288 实证 +0x0c 取机器键；
        // PcRoom::Rent 报告实证 +0x10 字节为 log 开关。
        RentInput();                   // ORIG C1 W 0x0822f058
        std::vector<RentInElem> m_elems;   // +0x00
        int m_machineKey;                  // +0x0c（getMachine 键）
        unsigned char m_logFlag;           // +0x10（非 0 逐物品 log）
        unsigned char m_field11;           // +0x11（ORIG C1 清零；PcRoom::Rent 读 ==1 判定）
    };

    typedef RentInput::RentInElem RentInElem;  // 兼容既有调用处（别名不产生符号）

    struct RentalSystemImpl
    {
        std::map<int, IRentalMachine*> m_machines;
    };

    InstanceRentalSystem();
    ~InstanceRentalSystem();
    void registerRentalMachine(int idx, IRentalMachine* m);  // ORIG T 0x08270320
    IRentalMachine* getMachine(int idx);                     // ORIG T 0x08270348
    bool Ready();                                            // ORIG T 0x082701dc
    int Rent(CUser&, const RentInput&, RentOutput&);         // ORIG T 0x08270288
    bool CheckRentableItem(int type, unsigned int itemIdx);   // ORIG T 0x082702dc（EJEnchanter 调用；定义 InstanceRentalSystem.cpp）


private:
    std::auto_ptr<RentalSystemImpl> m_impl;  // +0x00（ORIG auto_ptr；C1 调 auto_ptr C1EPS1_）
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

    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                     InstanceRentalSystem::RentOutput& output) = 0;   // 槽2
    virtual bool Ready() = 0;                                          // 槽3
    virtual bool CheckRentableItem(unsigned int itemIdx) = 0;          // 槽4
};

#endif  // DNF_INSTANCE_RENTAL_SYSTEM_H
