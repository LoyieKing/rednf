// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CQuestShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CQuestShop ORIG-only 符号恢复（Wave8 W8_questshop）。
// 反汇编依据 df_game_r objdump（0x85eed2a..0x85efd73）；ctor/dtor 见
// docs/class_func_reports/CQuestShop/ 三份报告。
// df_game_r CQuestShop::clearQP 恢复。
// 依据 docs/class_func_reports/CQuestShop.md 的反汇编与反编译报告。
#include "CUser.h"
#include "CQuestShop.h"
#include "PacketGuard.h"
#include "GlobalData.h"

// CQuestShop 消费侧 statusCounts 视图（CHARAC_QUEST_SHOP_DATA 0xaa 的
// 计数视角：+0x0c..+0xa5 为 0x4d 个 ushort 状态已购计数；DB 侧 _LoadQuestShop
// 同一区按属性具名填充，两视角布局一致）。
struct QuestShopStatusView
{
    unsigned int m_pad0;              // +0x00
    unsigned int m_qp;                // +0x04
    unsigned int m_initCount;         // +0x08
    unsigned short m_statusCounts[0x4d];  // +0x0c..+0xa5
    unsigned int m_questPiece;        // +0xa6
};

#include "CInventory.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
// 顶层 stStatusLevelInfo / stQuestShopStatus 类型定义已上移至 CDataManager.h
// （全局作用域，ORIG mangled 无 stQuestShop 前缀）；本 TU 仅保留方法定义
// （C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826 / C1ERKS_ 0x85f07fe /
// getMaxCount / getNextLevel，见文件后部）。
// ORIG C1 0x85eebc8 size 0x97：CData 基类（vptr）、map@+0x08、map@+0x20 构造，
// _reset() 收尾；异常清理路径按 ORIG try/leave 版式由编译器生成。
#include "CQuestShop/CQuestShop.inc"
{
    _reset();   // ORIG +0x3e：map 构造后直调 _reset（085efd74）
}

// ORIG D1 0x85eec60 size 0x89：vptr 重置 → ~map(+0x20) → ~map(+0x08) → ~CData；
// D0（deleting）由编译器按虚析构规则产出（+operator delete，D1 后 delete this）。
#include "CQuestShop/dtor_CQuestShop.inc"
{
}

#include "CQuestShop/getQp.inc"
#include "CQuestShop/getInitQp.inc"
#include "CQuestShop/getInitCount.inc"
#include "CQuestShop/getPiece.inc"

#include "CQuestShop/setQp.inc"
#include "CQuestShop/setInitQp.inc"
#include "CQuestShop/setInitCount.inc"
#include "CQuestShop/setPiece.inc"
#include "CQuestShop/addQp.inc"
#include "CQuestShop/subQp.inc"
#include "CQuestShop/incInitQp.inc"
#include "CQuestShop/incInitCount.inc"
#include "CQuestShop/addPiece.inc"

#include "CQuestShop/clearBuyingStatuslist.inc"
{
    alter();
    m_buyingStatus.clear();
}

#include "CQuestShop/sendCharacQp.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x15c);
    packet.put_int(getQp());
    packet.put_short(getInitCount());
    packet.put_byte(static_cast<int>(flag));
    packet.finalize(true);
    user->Send(packet);
}

#include "CQuestShop/sendCharacQuestPiece.inc"
{
    // ORIG 0x85ef7cc T：put_header(0,0x1be) / put_int(getPiece) / finalize(true) / user->Send
    PacketGuard packet;
    packet.put_header(0, 0x1be);
    packet.put_int(getPiece());
    packet.finalize(true);
    user->Send(packet);
}

#include "CQuestShop/clearQP.inc"
{
    const int oldQp = getQp();          // -0x14
    user->setInitQuestPoint();
    const int initQp = getInitQp();     // -0x10
    setQp(initQp);
    clearBuyingStatuslist();
    // ORIG 0x85ef5d5：afterQp=getQp() 先于 getInitCount()（实参求值序），history = +0x79700
    const int afterQp = getQp();        // -0xc
    user->m_historyLog.ClearUsedQP(oldQp, afterQp, getInitCount(), 0);
}

#include "CQuestShop/makePacket_BuyingStatusList.inc"
{
    // ORIG 0x85ef5dc：flag 分支选 map（if(!flag) 取反布局）
    std::map<unsigned char, short>* status;   // ORIG 非 const begin/end
    if (!flag)
        status = &m_buyingStatus;
    else
        status = &m_tagBuyingStatus;
    const unsigned char sz = static_cast<unsigned char>(status->size()); // -0x15
    packet.put_byte(static_cast<int>(sz));

    std::map<unsigned char, short>::iterator end = status->end(); // -0x20 循环前
    for (std::map<unsigned char, short>::iterator it =
             status->begin();                    // -0x24
         it != end; ++it)
    {
        int itemStatus = -1;                     // -0x14
        const int itemIndex = static_cast<int>(it->first); // -0x10
        int count = static_cast<int>(it->second); // -0xc
        if (itemIndex > 0x4c)                    // ORIG jle 跳计算分支（fall-through 超界）
        {
            itemStatus = -1;
            count = 0;
        }
        else
        {
            // ORIG 每次重读 G_CDataManager
            itemStatus = static_cast<signed char>(
                G_CDataManager()->m_questShop.m_pad28[itemIndex]);
        }
        packet.put_byte(itemStatus);
        packet.put_short(count);
    }
}

// ============================================================================
// ---- CQuestShop::subQp(unsigned char, unsigned char)  ORIG T 0x85eed2a
#include "CQuestShop/subQp_1.inc"
{
    int cost = 0;                                 // ORIG movl $0x0,-0xc
    cost = findStatusQp(status);                  // +0x00..+0x33
    if (cost <= 0)
        return 1;                                 // +0x3b
    cost = static_cast<int>(count) * cost;        // imul +0x4a
    if ((getQp() - cost >> 31) != 0)              // shr $0x1f 负数判定
        return 0x16;                              // +0x5d
    subQp(cost);                                  // _ZN10CQuestShop5subQpEi
    return 0;                                     // +0xa1
}

// ---- ORIG T 0x85eeda8 size 0xc2：查 [quest status] 表中该 status 的 QP ---
#include "CQuestShop/findStatusQp.inc"
{
    // ORIG 0x85eeda8：m_map0 局部 map 拷贝（非 const）+ find/end/operator->；
    // value+0x18 单字节读取（movzbl 实证 = second.m_questPoint +0x14）。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0); // +0x6398
    // b28c-L10：ORIG 0x85eeda8 槽序 table=-0x28 / key临时=-0x10。4.4 对「具名 key 局部」
    // 把 key 排到 table 之前（-0x28/-0x24）→ 全帧 key/table 引用差 4。实内联 cast 作实参
    // 时 4.4 复现 ORIG 槽序（探针 /tmp/l10_probe3.cpp fC/fD 实测 table=-0x28、临时=-0x10）。
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));
    if (it != table.end())                        // ORIG ne 比较 + je 跳过命中块
        return static_cast<int>(it->second.m_questPoint); // movzbl 0x18(%eax)
    return -1;
}

// ---- ORIG T 0x85eee6a size 0xbe：查该 status 的槽位类型 -------------------
// （stQuestShopStatus +0x1c dword；表外返回哨兵 0x4d）
#include "CQuestShop/findStatusType.inc"
{
    // ORIG 0x85eee6a：map 拷贝 + 非 const find/end；value+0x1c = second.m_field18。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));     // b28c-L10：同上（ORIG 0x85eee6a 槽序）
    if (it != table.end())                        // ORIG ne 比较
        return it->second.m_field18;              // mov 0x1c(%eax)（value+0x1c）
    return 0x4d;                                  // +0x85
}

// ---- ORIG T 0x85ef02e size 0xa0：读 m_buyingStatus[type] -----------------
#include "CQuestShop/getStatusBuyingCount.inc"
{
    const int type = findStatusType(status);      // -0xc
    if (type == 0x4d || type == -1)               // +0x2e/+0x34
        return -1;
    // ORIG 0x85ef02e：type 收窄 byte 后非 const find/end/operator->
    std::map<unsigned char, short>::iterator it =
        m_buyingStatus.find(static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())               // +0x84
        return it->second;                        // movzwl 0x2(%eax) 直返 16 位
    return 0;                                     // +0x99
}

// ---- ORIG T 0x85eef28 size 0x105：alter() 后累加/新建购买数 --------------
#include "CQuestShop/incStatusBuyingCount.inc"
{
    const int type = findStatusType(status);      // -0x28
    if (type == 0x4d)                             // +0x28 je → 直接返回
        return;
    alter();                                      // charac_expand::CData::alter
    // ORIG 0x85eef28：type 收窄 byte 后非 const find/end
    std::map<unsigned char, short>::iterator it = m_buyingStatus.find(
        static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())
    {
        it->second += static_cast<short>(count);  // ORIG lea 累加后 mov %dx,0x2(%eax)
        return;
    }
    // ORIG make_pair<IRiRKh>(type, count) 左值 + pair 移动 + insert
    m_buyingStatus.insert(std::make_pair(
        static_cast<const int&>(type),
        static_cast<const unsigned char&>(count)));
}

#include "CQuestShop/getMaxCount.inc"
{
    int count = 0;                                       // -0x0c

    std::vector<stStatusLevelInfo>::iterator it = m_levelInfos.begin(); // -0x18（ORIG 循环外 begin）
    while (it != m_levelInfos.end())                     // 循环头 end() + operator!=
    {
        if (static_cast<int>(it->m_first) <= level)      // cmp 0xc(%ebp),%eax
            count += static_cast<int>(it->m_second);     // add m_max
        it++;                                            // ORIG 后置 operator++(int)
    }
    return count;
}

#include "CQuestShop/getNextLevel.inc"
{
    int count = 0;                                       // -0x0c

    std::vector<stStatusLevelInfo>::iterator it = m_levelInfos.begin(); // -0x18
    while (it != m_levelInfos.end())
    {
        count += static_cast<int>(it->m_second);
        if (count > level)                               // +0x3f cmp/jle 反向
            return static_cast<int>(it->m_first);
        it++;
    }
    return -1;
}

// ORIG clear 0x891b8cc（objdump 直转）：4×string clear + +0x10=-1、+0x14=0、
// +0x18=0x4d、+0x1c=2、+0x20=0、vector clear。
#include "CQuestShop/clear.inc"
{
    // [b28c-27] AE：+0x20 ORIG `mov $0x0,%edx; mov %edx,0x20(%eax)`（常量经 edx
    // 中转）⇒ 字段改 float + `= 0.0f`（P211 常量 store 方向开关的 float 变体，
    // C1ERKS_ 4 字节 mov 拷贝不变）；int/unsigned int 两形实测均直写。
    m_name.clear();
    m_name2.clear();
    m_explain.clear();
    m_icon.clear();
    m_field10 = -1;
    m_questPoint = 0;
    m_field18 = 0x4d;
    m_statusType = 2;
    m_field20 = 0.0f;            // ORIG mov $0x0,%edx 中转（gcc4.1 特性，本工具链直写）
    m_levelInfos.clear();
}

// ORIG C1 0x891b948（objdump 直转）：4×string + vector 构造后 clear()。
#include "CQuestShop/stQuestShopStatus.inc"
    : m_name(), m_name2(), m_explain(), m_icon(), m_levelInfos()
{
    clear();
}

// ORIG C1ERKS_ 0x85f07fe（objdump 直转）：4×string + 标量 + vector 复制。
#include "CQuestShop/stQuestShopStatus_1.inc"
    : m_name(other.m_name),
      m_name2(other.m_name2),
      m_explain(other.m_explain),
      m_icon(other.m_icon),
      m_field10(other.m_field10),
      m_questPoint(other.m_questPoint),
      m_field18(other.m_field18),
      m_statusType(other.m_statusType),
      m_field20(other.m_field20),
      m_levelInfos(other.m_levelInfos)
{
}

// ORIG D1 0x83cd826（objdump 直转）：vector + 4×string 逆序析构。
#include "CQuestShop/dtor_stQuestShopStatus.inc"
{
}

// ---- ORIG T 0x85ef0ce size 0xec：限购等级校验 ----------------------------
#include "CQuestShop/checkBuyingStatus_LimitLevel.inc"
                                              int characLevel)
{
    // ORIG 0x85ef0ce：m_map0 局部 map 拷贝 + 非 const find/end；
    // value（+0x4 = second）直接按顶层镜像 stQuestShopStatus 调 getMaxCount。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));     // b28c-L10：同 findStatusQp（ORIG 0x85ef0ce table=-0x2c/临时=-0x14）
    if (it != table.end())                        // ORIG ne + je 尾部 false 块
    {
        const int maxCount = it->second.getMaxCount(characLevel); // +0x4 value + getMaxCount
        if (static_cast<int>(count) > maxCount)   // +0xc0 cmp/jle
            return false;
        return true;                              // +0xaa
    }
    return false;                                 // +0xb3: 未找到 → false
}

// ---- ORIG T 0x85ef1ba size 0x5a：initCost 基础公式 ------------------------
#include "CQuestShop/getInitCost.inc"
{
    // ORIG 0x85ef1ba：G_CDataManager() 每次重读（三次独立 call）
    const int cost0 = G_CDataManager()->m_questShop.m_initCost.m_cost0; // dm+0x63b4
    const int cost1 = G_CDataManager()->m_questShop.m_initCost.m_cost1; // dm+0x63b8
    const int cost2 = G_CDataManager()->m_questShop.m_initCost.m_cost2; // dm+0x63bc
    if (count == 0)                               // +0x30 jne
        return cost0;                             // -0x18
    const int total = count * cost1 + cost0;      // imul/add -0xc
    if (total >= cost2)                           // ORIG jl 跳 return total（P47 布局）
        return cost2;                             // cap
    return total;
}

// ---- ORIG T 0x85ef214 size 0xf1：已用 QP 汇总 -----------------------------
#include "CQuestShop/calUsedQp.inc"
{
    unsigned char qp = 0;                                  // -0x19
    int used = 0;                                          // -0x18
    std::map<unsigned char, short>::iterator end =
        m_buyingStatus.end();                              // -0x20（ORIG 循环前缓存）
    for (std::map<unsigned char, short>::iterator it =
             m_buyingStatus.begin();                       // -0x24
         it != end; ++it)
    {
        int itemStatus = -1;                              // -0x14（循环内首声明）
        const int statusIdx = static_cast<int>(it->first); // -0x10
        const int count = static_cast<int>(it->second);    // cwtl -0xc
        if (statusIdx > 0x4c)                              // cmpl $0x4c → return -1
            return -1;
        itemStatus = static_cast<signed char>(
            G_CDataManager()->m_questShop.m_pad28[statusIdx]); // ORIG 每次重读 G_CDataManager
        qp = static_cast<unsigned char>(findStatusQp(itemStatus));
        if (qp == 0)                                       // +0xbc cmpb jne
            return -1;
        used += static_cast<int>(qp) * count;              // imul/add
    }
    return used;                                           // +0xd2 循环外
}

// ---- ORIG T 0x85ef306 size 0x11f：购买状态主流程 --------------------------
#include "CQuestShop/buyStatus.inc"
                          unsigned char count, short& result)
{
    int buyCount = getStatusBuyingCount(status);    // -0x18
    if (buyCount == -1)                             // +0x33 jne
        return 1;
    result = static_cast<short>(static_cast<unsigned short>(count) +
                                static_cast<unsigned short>(buyCount));
    const bool limited = checkBuyingStatus_LimitLevel(status, result,
                                                      user->get_charac_level());
    if (!limited)                                   // ORIG mov %al,-0x11 → xor $1
        return 0xe;                                 // +0x93
    const int beforeQp = getQp();                   // -0x10
    const int subRet = subQp(status, count);        // -0xc
    if (subRet != 0)                                // +0xca je → return subRet
        return subRet;
    incStatusBuyingCount(status, count);
    user->m_historyLog.QPSub(beforeQp, getQp(), static_cast<int>(status));
    return 0;
}

// ---- ORIG T 0x85ef426 size 0x125：花金币重置 QP ---------------------------
#include "CQuestShop/clearUsedQp.inc"
{
    int initQp = 0;                                 // -0x18
    const int cost = getInitCost(getInitCount());   // -0x14
    if (user->getCurCharacInvenR()->get_money() < cost)
        return 0xa;                                 // +0x45 setl
    if (!user->getCurCharacInvenW()->use_money(
            cost, (eMoneySubReason)0x2b, true))     // xor $1 判定 false→ret1
        return 1;                                   // +0x86
    const int beforeQp = getQp();                   // -0x10
    incInitCount();
    user->setInitQuestPoint();
    initQp = getInitQp();                           // -0x18
    setQp(initQp);
    clearBuyingStatuslist();
    const int afterQp = getQp();                    // -0xc
    // ORIG +0x10f：getInitCount() 直接进实参位（不存局部）
    user->m_historyLog.ClearUsedQP(beforeQp, afterQp, getInitCount(), cost);
    return 0;
}

// ---- ORIG T 0x85efd74 size 0x4c：_reset 虚槽 7 -----------------------------
#include "CQuestShop/_reset.inc"
{
    m_buyingStatus.clear();          // 0x85efd83 map clear (+0x08)
    m_qp = 0;                        // movl $0,0x38(%eax)
    m_initCount = 0;                 // movl $0,0x3c(%eax)（ORIG 序：0x38→0x3c→0x40）
    m_initQp = 0;                    // movl $0,0x40(%eax)
    m_tagBuyingStatus.clear();       // +0x20 map clear
    m_piece = 0;                     // 0x44
}

// ---- ORIG T 0x85efdc0 size 0x94：装入 tag 状态计数 ------------------------
#include "CQuestShop/SetTagCharacQuestShopData.inc"
{
    for (int i = 0; i <= 0x4c; ++i)                 // cmp $0x4c setle
    {
        // b28c-L10：完整类型具名成员直访问（+0x0c 计数视角，见 CHARAC_QUEST_SHOP_DATA.h
        // 匿名 union）。ORIG 0x85efdc0 装载序为「i 先、base 后」且 make_pair 实例
        // `IRiRt`（非 const ushort 左值）；reinterpret_cast 视图形在 4.4 反装为
        // 「base 先 + 多一条 add」（ndiff 17→0 见探针 /tmp/l10_probe6.cpp fH）。
	        // [FragB] ORIG nm 实证：全库只有 `_ZSt9make_pairIRiRtE` /
	        // `_ZNSt4pairIKhsEC[12]IitE`，**不存在** short 版（IRiRs/Iis）实例；
	        // 旧 short 视角（m_statusCounts）使 4.4 发射 IRiRs+C1Iis+C2Iis 一组
	        // ORIG 无的弱符号，且 make_pair 值经 short 符号扩展（>0x7fff 语义分叉）
	        // ⇒ 改用头内既有同址 ushort 视角 m_statusCountsU（零布局影响）。
	        if (data->m_statusCountsU[i] == 0)
            continue;                               // movzwl + test ax / je
        m_tagBuyingStatus.insert(std::make_pair(
	            i, data->m_statusCountsU[i]));
    }
    return 1;                                       // movl $0x1,%eax
}

// ---- ORIG T 0x85efa2c size 0x13a：loadData 虚槽 1 --------------------------
#include "CQuestShop/loadData.inc"
{
    _reset();   // ORIG call *(vtable+0x1c)（CData 虚槽 7 _reset）
    // b28d-36（修法 P61/P167）：ORIG 的 data 形参非 const（char* data），
    // make_pair 第二参是非 const 左值 ⇒ 实例 `IRiRt`；旧 const 视图生成
    // `IRiRKt`（p167 EXTRA _ZSt9make_pairIRiRKtE…）。
    SigQuestShopLoadBlock* block =
        reinterpret_cast<SigQuestShopLoadBlock*>(data); // -0x10
    SigQuestShopLoadBlock* p = block;               // -0xc（ORIG data→-0x10→-0xc 两步）
    m_qp = static_cast<int>(p->m_main.m_qp);        // mov 0x4→m_qp(+0x38)
    setInitCount(static_cast<int>(p->m_main.m_initCount));
    m_piece = static_cast<int>(p->m_main.m_questPiece); // 0xa6→(+0x44)
    for (int i = 0; i <= 0x4c; ++i)
    {
        // ORIG 0x085efa8d：`mov -0x28(%ebp),%edx; mov -0xc(%ebp),%eax;
        // movzwl 0xc(%eax,%edx,2),%eax`（**i 先、base mem 直用**）——只有完整类型
        // 具名成员访问能出此序；整指针 reinterpret_cast 视图恒使 4.4 先载 base
        // （P235）。元素左值按 make_pair 实例 `IRiRt` 要求转 `unsigned short&`。
        if (p->m_main.m_statusCountsU[i] == 0)          // movzwl + test ax / je
            continue;
        // ORIG make_pair<IRiRt>(i, m_statusCounts[i]) 左值 + pair 移动 + insert
        m_buyingStatus.insert(
            std::make_pair(i, p->m_main.m_statusCountsU[i]));
    }
    if (block->m_tagFlag != 0)                      // movzbl 0x154(%eax)（ORIG 用 -0x10=block）
    {
        p = reinterpret_cast<SigQuestShopLoadBlock*>(
            &block->m_tag);   // ORIG SetTag 实参从 block(-0x10) 算 +0xaa 经 -0xc 中转
        SetTagCharacQuestShopData(
            reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(p));
    }
    if (user->getCurCharacLastPlayTick() <=
        static_cast<long>(0x500723cf))              // cmp/setle +0x12a
        clearQP(user);
    return 1;                                       // +0x139
}

// ---- ORIG T 0x85efb66 size 0x20e：_saveData 虚槽 6（入 DB_Q）--------------
#include "CQuestShop/_saveData.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QuestShop.cpp", 0x21b),
                       true);                       // ORIG ctor(stream,1)
    (*guard)->operator<<(0x279);                    // ORIG de→ls（P132 operator* 链）
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    // ORIG GetInBuffer<SIG_QUEST_SHOP_DATA> size 0x155；SIG_QUEST_SHOP_DATA
    // 即 0x155 双实例结构（SigTypes_D.h），_saveData 直接装载。
    SigQuestShopLoadBlock* sig =
        guard.operator->()->GetInBuffer<SigQuestShopLoadBlock>(); // ORIG pt→GetInBuffer（P103）
    if (sig == 0)
        return 0;                                   // ebx=0
    memset(sig, 0, 0x155);
    SigQuestShopLoadBlock* view = sig;              // -0x10（ORIG sig→view 两步）
    view->m_main.m_field0 =
        static_cast<unsigned int>(user->get_charac_no(-1)); // mov %edx,(%eax)
    view->m_main.m_qp = static_cast<unsigned int>(getQp());
    view->m_main.m_initCount = static_cast<unsigned int>(getInitCount());
    view->m_main.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    // b28d-36（修法 P13/P28）：ORIG 0x085efc7e 段先 end()→-0x20、后 begin()→-0x24，
    // 循环 cond 只重调 operator!=（不重算 end）⇒ 原码缓存 end 迭代器；每轮 end()
    // 版多一个 -0x18 临时槽并把 guard 顶到 -0x20（ORIG -0x1c）。
    std::map<unsigned char, short>::iterator itEnd = m_buyingStatus.end();
    for (std::map<unsigned char, short>::iterator it =
             m_buyingStatus.begin();
         it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // cmp $0x4c seta → skip
            continue;
        view->m_main.m_statusCounts[key] = value;   // mov %cx,0xc(key*2)
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard); // DB_Q @GlobalData
    return 1;                                       // ebx=1
}

// ---- ORIG T 0x85ef868 size 0x1c4：getData 虚槽 2（双实例导出）-------------
#include "CQuestShop/getData.inc"
{
    char* p = out;                                   // -0x18
    memset(p, 0, 0x155);                             // ORIG P68 装载序
    SIG_QUEST_SHOP_DATA* view =
        reinterpret_cast<SIG_QUEST_SHOP_DATA*>(p);   // -0x14
    view->m_main.m_qp = static_cast<unsigned int>(getQp());       // 0x4(%eax)
    view->m_main.m_initCount = static_cast<unsigned int>(getInitCount()); // 0x8
    view->m_main.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    // ORIG 0x85ef868：end() 只在循环前调一次（-0x1c），cond 段仅 operator!=；
    // 且调用序 end→begin（085ef8e1 end / 085ef8f9 begin）。判定时重读 it->first
    // （第三次 ptEv，O0 无 CSE）。
    std::map<unsigned char, short>::const_iterator itEnd =
        m_buyingStatus.end();                        // -0x1c
    std::map<unsigned char, short>::const_iterator it =
        m_buyingStatus.begin();                      // -0x20
    for (; it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xf
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // seta → skip 写入
            continue;
        // ORIG 0x85ef93e：`movzbl -0xf,%edx; movzwl -0xe,%ecx; mov -0x14(%ebp),%eax;
        // mov %cx,0xc(%eax,%edx,2)`（值先、base 后）⇒ 必须走**完整类型具名成员**
        // 直访问（P235：reinterpret_cast 视图形使 4.4 恒 base 先载）。
        view->m_main.m_statusCounts[key] = value;
    }
    view = reinterpret_cast<SIG_QUEST_SHOP_DATA*>(
        &((SIG_QUEST_SHOP_DATA*)p)->m_tag);     // +0x0aa 段（ORIG 复用 -0x14）
    // ORIG 0x85ef97d/0x85ef99b：第二循环**复用**同一对迭代器变量（begin 返回临时
    // -0x2c → `mov 临时,%eax; mov %eax,-0x20`；end 临时 -0x2c → -0x1c），即源形为
    // 「函数级声明 + 循环前**赋值**」（P50 复活：ORIG 有独立 sret 临时槽 + 拷贝两段形；
    // 此前写成循环内新声明 ⇒ 4.4 直构省略临时，gap=-4）。
    it = m_tagBuyingStatus.begin();                  // -0x20（sret 临时 -0x2c 后拷贝）
    itEnd = m_tagBuyingStatus.end();                 // -0x1c（同上）
    for (; it != itEnd; ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)
            continue;
        view->m_main.m_statusCounts[key] = value;
    }
}

// ---- ORIG W 0x0891b8b4：顶层 stStatusLevelInfo C1（+0x01 清零后拷至 +0x00）
// 合法 force-use 发射（vector<stStatusLevelInfo> 元素构造点）。 ----
namespace {
#include "CQuestShop/emitStStatusLevelInfoWeakSymbols.inc"
{
    // C1（构造）+ D1/D2（delete 完整对象析构）。
    stStatusLevelInfo* info = new stStatusLevelInfo();
    delete info;
}
}  // namespace
