// CQuestShop ORIG-only 符号恢复（Wave8 W8_questshop）。
// 反汇编依据 df_game_r objdump（0x85eed2a..0x85efd73）；ctor/dtor 见
// docs/class_func_reports/CQuestShop/ 三份报告。
// df_game_r CQuestShop::clearQP 恢复。
// 依据 docs/class_func_reports/CQuestShop.md 的反汇编与反编译报告。
#include "CUser.h"
#include "CQuestShop.h"
#include "PacketGuard.h"
#include "GlobalData.h"

#include "CInventory.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
// ORIG C1 0x85eebc8 size 0x97：CData 基类（vptr）、map@+0x08、map@+0x20 构造，
// _reset() 收尾；异常清理路径按 ORIG try/leave 版式由编译器生成。
CQuestShop::CQuestShop()
{
}

// ORIG D1 0x85eec60 size 0x89：vptr 重置 → ~map(+0x20) → ~map(+0x08) → ~CData；
// D0（deleting）由编译器按虚析构规则产出（+operator delete，D1 后 delete this）。
CQuestShop::~CQuestShop()
{
}

int CQuestShop::getQp() const { return m_qp; }
int CQuestShop::getInitQp() const { return m_initQp; }
int CQuestShop::getInitCount() const { return m_initCount; }
int CQuestShop::getPiece() const { return m_piece; }

void CQuestShop::setQp(int value) { alter(); m_qp = value; }
void CQuestShop::setInitQp(int value) { m_initQp = value; }
void CQuestShop::setInitCount(int value) { m_initCount = value; }
void CQuestShop::setPiece(int value) { m_piece = value; }
void CQuestShop::addQp(int value) { m_qp += value; }
void CQuestShop::subQp(int value) { m_qp -= value; }
void CQuestShop::incInitQp(int value) { m_initQp += value; }
void CQuestShop::incInitCount() { ++m_initCount; }
void CQuestShop::addPiece(int value) { m_piece += value; }

void CQuestShop::clearBuyingStatuslist()
{
    alter();
    m_buyingStatus.clear();
}

void CQuestShop::sendCharacQp(CUser* user, char flag)
{
    PacketGuard packet;
    packet.put_header(0, 0x15c);
    packet.put_int(getQp());
    packet.put_short(getInitCount());
    packet.put_byte(static_cast<int>(flag));
    packet.finalize(true);
    user->Send(packet);
}

void CQuestShop::sendCharacQuestPiece(CUser* user)
{
    // ORIG 0x85ef7cc T：put_header(0,0x1be) / put_int(getPiece) / finalize(true) / user->Send
    PacketGuard packet;
    packet.put_header(0, 0x1be);
    packet.put_int(getPiece());
    packet.finalize(true);
    user->Send(packet);
}

void CQuestShop::clearQP(CUser* user)
{
    const int oldQp = getQp();
    user->setInitQuestPoint();
    setQp(getInitQp());
    clearBuyingStatuslist();
    // ORIG 0x85ef5d5：history = CUser +0x79700（具名成员 m_historyLog）
    user->m_historyLog.ClearUsedQP(oldQp, getQp(), getInitCount(), 0);
}

void CQuestShop::makePacket_BuyingStatusList(PacketGuard& packet, bool flag)
{
    const std::map<unsigned char, short>& status =
        flag ? m_tagBuyingStatus : m_buyingStatus;
    packet.put_byte(static_cast<int>(status.size()));

    const stQuestShop& questShop = G_CDataManager()->m_questShop;
    for (std::map<unsigned char, short>::const_iterator it = status.begin();
         it != status.end(); ++it)
    {
        int itemStatus = -1;
        const int itemIndex = static_cast<int>(it->first);
        int count = static_cast<int>(it->second);
        if (itemIndex <= 0x4c)
            itemStatus = static_cast<signed char>(questShop.m_pad28[itemIndex]);
        else
            count = 0;
        packet.put_byte(itemStatus);
        packet.put_short(count);
    }
}

// ============================================================================
// ---- CQuestShop::subQp(unsigned char, unsigned char)  ORIG T 0x85eed2a
int CQuestShop::subQp(unsigned char status, unsigned char count)
{
    int cost = findStatusQp(status);              // +0x00..+0x33
    if (cost <= 0)
        return 1;                                 // +0x3b
    cost = static_cast<int>(count) * cost;        // imul +0x4a
    if ((getQp() - cost >> 31) != 0)              // shr $0x1f 负数判定
        return 0x16;                              // +0x5d
    subQp(cost);                                  // _ZN10CQuestShop5subQpEi
    return 0;                                     // +0xa1
}

// ---- ORIG T 0x85eeda8 size 0xc2：查 [quest status] 表中该 status 的 QP ---
int CQuestShop::findStatusQp(unsigned char status)
{
    const std::map<int, stQuestShop::stQuestShopStatus>& table =
        G_CDataManager()->m_questShop.m_map0;     // CDataManager +0x6398
    std::map<int, stQuestShop::stQuestShopStatus>::const_iterator it =
        table.find(static_cast<int>(status));
    if (it == table.end())
        return -1;
    return static_cast<int>(it->second.m_field18); // stQuestShopStatus +0x18
}

// ---- ORIG T 0x85eee6a size 0xbe：查该 status 的槽位类型 -------------------
// （stQuestShopStatus +0x1c dword；表外返回哨兵 0x4d）
int CQuestShop::findStatusType(unsigned char status)
{
    const std::map<int, stQuestShop::stQuestShopStatus>& table =
        G_CDataManager()->m_questShop.m_map0;
    std::map<int, stQuestShop::stQuestShopStatus>::const_iterator it =
        table.find(static_cast<int>(status));
    if (it == table.end())
        return 0x4d;                              // +0x85
    return it->second.m_field1c;                  // mov 0x1c(%eax)
}

// ---- ORIG T 0x85ef02e size 0xa0：读 m_buyingStatus[type] -----------------
int CQuestShop::getStatusBuyingCount(unsigned char status)
{
    const int type = findStatusType(status);      // +0x00..+0x29
    if (type == 0x4d || type == -1)               // +0x2e/+0x34
        return -1;
    std::map<unsigned char, short>::const_iterator it =
        m_buyingStatus.find(static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())               // +0x84
        return static_cast<int>(it->second);      // movzwl 0x2(%eax)
    return 0;                                     // +0x99
}

// ---- ORIG T 0x85eef28 size 0x105：alter() 后累加/新建购买数 --------------
void CQuestShop::incStatusBuyingCount(unsigned char status, unsigned char count)
{
    const int type = findStatusType(status);
    if (type == 0x4d)                             // +0x28 je → 直接返回
        return;
    alter();                                      // charac_expand::CData::alter
    std::map<unsigned char, short>::iterator it = m_buyingStatus.find(
        static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())
    {
        it->second = static_cast<short>(
            static_cast<unsigned short>(it->second) +
            static_cast<unsigned short>(count));  // add word @value+0x02
        return;
    }
    m_buyingStatus.insert(std::map<unsigned char, short>::value_type(
        static_cast<unsigned char>(type), static_cast<short>(count)));
}

// ============================================================================
 // ORIG 全局级成员符号 _ZN17stQuestShopStatus11getMaxCountEi（T 0x890f8ec）。
 // ORIG 类型 stQuestShopStatus 为命名空间顶层；权威逻辑模型是
 // stQuestShop::stQuestShopStatus（CDataManager.h）。此处按 ORIG 布局建唯一
 // 镜像以产生/承载真实符号与 getMaxCount 内部访问。
struct stQuestShopStatus
{
    int getMaxCount(int level);

    std::string m_name;                    // +0x00
    std::string m_name2;                   // +0x04
    std::string m_explain;                 // +0x08
    std::string m_icon;                    // +0x0c
    int m_field10;                         // +0x10
    unsigned char m_questPoint;            // +0x14
    unsigned char m_pad15[3];
    unsigned char m_qp;                    // +0x18（findStatusQp movzbl 读点）
    unsigned char m_pad19[3];
    int m_statusType;                      // +0x1c（findStatusType 读点）
    int m_field20;                         // +0x20
    std::vector<std::pair<unsigned char, unsigned char> > m_levelInfos; // +0x24
};
static_assert(sizeof(stQuestShopStatus) == 0x30,
              "ORIG stQuestShopStatus layout drift");

int stQuestShopStatus::getMaxCount(int level)
{
    int count = 0;                                       // -0x0c
    for (std::vector<
             std::pair<unsigned char, unsigned char> >::const_iterator it =
             m_levelInfos.begin();
         it != m_levelInfos.end(); ++it)
    {
        if (static_cast<int>(it->first) <= level)        // cmp 0xc(%ebp),%eax
            count += static_cast<int>(it->second);       // add m_max
    }
    return count;
}

// ---- ORIG T 0x85ef0ce size 0xec：限购等级校验 ----------------------------
bool CQuestShop::checkBuyingStatus_LimitLevel(unsigned char status, short count,
                                              int characLevel)
{
    const std::map<int, stQuestShop::stQuestShopStatus>& table =
        G_CDataManager()->m_questShop.m_map0;
    std::map<int, stQuestShop::stQuestShopStatus>::const_iterator it =
        table.find(static_cast<int>(status));
    if (it == table.end())                        // +0xb3: 未找到 → false
        return false;
    // ORIG：把 map 值视图当 ORIG 顶层 stQuestShopStatus 传给 getMaxCount
    // （两类型布局同为 0x30，见上方镜像 static_assert）。
    stQuestShopStatus* qs = const_cast<stQuestShopStatus*>(
        reinterpret_cast<const stQuestShopStatus*>(&it->second));
    const int maxCount = qs->getMaxCount(characLevel);
    if (static_cast<int>(count) > maxCount)       // +0xc0 cmp/jle
        return false;
    return true;                                  // +0xaa
}

// ---- ORIG T 0x85ef1ba size 0x5a：initCost 基础公式 ------------------------
int CQuestShop::getInitCost(int count)
{
    const stQuestShop& questShop = G_CDataManager()->m_questShop;
    const int cost0 = questShop.m_initCost.m_cost0;   // dm+0x63b4
    const int cost1 = questShop.m_initCost.m_cost1;   // dm+0x63b8
    const int cost2 = questShop.m_initCost.m_cost2;   // dm+0x63bc
    if (count == 0)                               // +0x30 jne
        return cost0;                             // -0x18
    const int total = count * cost1 + cost0;      // imul/add -0xc
    if (total < cost2)                            // cmp/jl +0x49
        return total;
    return cost2;                                 // cap
}

// ---- ORIG T 0x85ef214 size 0xf1：已用 QP 汇总 -----------------------------
int CQuestShop::calUsedQp()
{
    unsigned char qp = 0;                                  // -0x19
    int used = 0;                                          // -0x18
    const stQuestShop& questShop = G_CDataManager()->m_questShop;
    for (std::map<unsigned char, short>::const_iterator it =
             m_buyingStatus.begin();
         it != m_buyingStatus.end(); ++it)
    {
        const int statusIdx = static_cast<int>(it->first); // -0x10
        const int count = static_cast<int>(it->second);    // cwtl -0xc
        if (statusIdx > 0x4c)                       // cmpl $0x4c → return -1
            return -1;
        const int itemStatus = static_cast<signed char>(
            questShop.m_pad28[statusIdx]);          // dm+0x63c0 表（具名成员）
        qp = static_cast<unsigned char>(findStatusQp(itemStatus));
        if (qp == 0)                                // +0xbc cmpb jne
            return -1;
        used += static_cast<int>(qp) * count;       // imul/add -0x2d0
    }
    return used;                                    // +0xd2 循环外
}

// ---- ORIG T 0x85ef306 size 0x11f：购买状态主流程 --------------------------
bool CQuestShop::buyStatus(CUser* user, unsigned char status,
                           unsigned char count, short& result)
{
    int buyCount = getStatusBuyingCount(status);    // cwtl -0x18
    if (buyCount == -1)                             // +0x33 jne
        return 1;
    result = static_cast<short>(static_cast<unsigned short>(count) +
                                static_cast<unsigned short>(buyCount));
    if (!checkBuyingStatus_LimitLevel(status, result,
                                      user->get_charac_level()))
        return 0xe;                                 // +0x93
    const int beforeQp = getQp();                   // -0x10
    const int subRet = subQp(status, count);
    if (subRet != 0)                                // +0xca je
        return subRet;
    incStatusBuyingCount(status, count);
    user->m_historyLog.QPSub(beforeQp, getQp(), static_cast<int>(status));
    return 0;
}

// ---- ORIG T 0x85ef426 size 0x125：花金币重置 QP ---------------------------
bool CQuestShop::clearUsedQp(CUser* user)
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
    const int initCount = getInitCount();
    user->m_historyLog.ClearUsedQP(beforeQp, afterQp, initCount, cost);
    return 0;
}

// ---- ORIG T 0x85efd74 size 0x4c：_reset 虚槽 7 -----------------------------
void CQuestShop::_reset()
{
    m_buyingStatus.clear();          // 0x85efd83 map clear (+0x08)
    m_qp = 0;                        // movl $0,0x38(%eax)
    m_initQp = 0;                    // 0x3c
    m_initCount = 0;                 // 0x40
    m_tagBuyingStatus.clear();       // +0x20 map clear
    m_piece = 0;                     // 0x44
}

// ---- ORIG T 0x85efdc0 size 0x94：装入 tag 状态计数 ------------------------
bool CQuestShop::SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data)
{
    // ORIG 按序列化块偏移访问 tag 区（0x85efdc0 movzwl 0xc+i*2）；
    // 该内存即 SIG_QUEST_SHOP_DATA 布局（SigTypes.h 权威定义），具名访问。
    const SIG_QUEST_SHOP_DATA& view =
        *reinterpret_cast<const SIG_QUEST_SHOP_DATA*>(data);
    for (int i = 0; i <= 0x4c; ++i)                 // cmp $0x4c setle
    {
        const unsigned short count = view.m_statusCounts[i]; // movzwl 0xc+i*2
        if (count == 0)                             // test ax / je
            continue;
        m_tagBuyingStatus.insert(
            std::map<unsigned char, short>::value_type(
                static_cast<unsigned char>(i), static_cast<short>(count)));
    }
    return 1;                                       // movl $0x1,%eax
}

// ---- ORIG T 0x85efa2c size 0x13a：loadData 虚槽 1 --------------------------
bool CQuestShop::loadData(CUser* user, char* data)
{
    _reset();   // ORIG call *(vtable+0x1c)（CData 虚槽 7 _reset）
    const SigQuestShopLoadBlock& block =
        *reinterpret_cast<const SigQuestShopLoadBlock*>(data);
    m_qp = static_cast<int>(block.m_main.m_qp);     // mov 0x4→m_qp(+0x38)
    setInitCount(static_cast<int>(block.m_main.m_initCount));
    m_piece = static_cast<int>(block.m_main.m_questPiece); // 0xa6→(+0x44)
    for (int i = 0; i <= 0x4c; ++i)
    {
        const unsigned short count = block.m_main.m_statusCounts[i];
        if (count == 0)                             // test ax / je
            continue;
        m_buyingStatus.insert(
            std::map<unsigned char, short>::value_type(
                static_cast<unsigned char>(i), static_cast<short>(count)));
    }
    if (block.m_tagFlag != 0)                       // movzbl 0x154(%eax)
        SetTagCharacQuestShopData(const_cast<CHARAC_QUEST_SHOP_DATA*>(
            reinterpret_cast<const CHARAC_QUEST_SHOP_DATA*>(&block.m_tag)));
    if (user->getCurCharacLastPlayTick() <=
        static_cast<long>(0x500723cf))              // cmp/setle +0x12a
        clearQP(user);
    return 1;                                       // +0x139
}

// ---- ORIG T 0x85efb66 size 0x20e：_saveData 虚槽 6（入 DB_Q）--------------
bool CQuestShop::_saveData(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QuestShop.cpp", 0x21b),
                       true);                       // ORIG ctor(stream,1)
    guard << 0x279;                                 // operator<<(int)
    guard << user->GetUID();
    SIG_QUEST_SHOP_DATA* sig =
        guard.GetInBuffer<SIG_QUEST_SHOP_DATA>();   // pt()+GetInBuffer 模板
    if (sig == 0)
        return 0;                                   // ebx=0
    memset(sig, 0, 0x155);
    sig->m_characNo =
        static_cast<unsigned int>(user->get_charac_no(-1)); // mov %edx,(%eax)
    sig->m_qp = static_cast<unsigned int>(getQp());
    sig->m_initCount = static_cast<unsigned int>(getInitCount());
    sig->m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    for (std::map<unsigned char, short>::const_iterator it =
             m_buyingStatus.begin();
         it != m_buyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (key > 0x4c)                             // cmp $0x4c seta → skip
            continue;
        sig->m_statusCounts[key] = value;           // mov %cx,0xc(key*2)
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard); // DB_Q @GlobalData
    return 1;                                       // ebx=1
}

// ---- ORIG T 0x85ef868 size 0x1c4：getData 虚槽 2（双实例导出）-------------
void CQuestShop::getData(char* out) const
{
    memset(out, 0, 0x155);
    SIG_QUEST_SHOP_DATA& view =
        *reinterpret_cast<SIG_QUEST_SHOP_DATA*>(out);
    view.m_qp = static_cast<unsigned int>(getQp());       // 0x4(%eax)
    view.m_initCount = static_cast<unsigned int>(getInitCount()); // 0x8
    view.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    for (std::map<unsigned char, short>::const_iterator it =
             m_buyingStatus.begin();
         it != m_buyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xf
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (key > 0x4c)                             // seta → skip 写入
            continue;
        view.m_statusCounts[key] = value;
    }
    SIG_QUEST_SHOP_DATA& tagView =
        *reinterpret_cast<SIG_QUEST_SHOP_DATA*>(out + 0xaa); // +0x97a
    for (std::map<unsigned char, short>::const_iterator it =
             m_tagBuyingStatus.begin();
         it != m_tagBuyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (key > 0x4c)
            continue;
        tagView.m_statusCounts[key] = value;
    }
}
