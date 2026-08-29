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
// 顶层 stStatusLevelInfo：ORIG 顶层类型（_ZNSt6vectorI17stStatusLevelInfo... 模板
// 实例实证，C1 0x891b8b4 与嵌套 stQuestShop::stStatusLevelInfo 同构：+0x01 清零后
// 拷至 +0x00，sizeof 2）。镜像 m_levelInfos 必须为 vector<stStatusLevelInfo>
// 才能产生 ORIG mangled（D1/C1ERKS_ 均以该模板实例调用）。
struct stStatusLevelInfo
{
    stStatusLevelInfo()      // ORIG W 0x0891b8b4：+0x01 清零后拷至 +0x00
    {
        m_second = 0;
        m_first = m_second;
    }
    // ORIG D1/D2 0x08940782（W 0x5）：用户空体析构（non-trivial，vector 析构调 D1）。
    ~stStatusLevelInfo() {}
    unsigned char m_first;   // +0x00（getNextLevel 返回；= 嵌套版 m_max）
    unsigned char m_second;  // +0x01（getMaxCount/getNextLevel 累计；= 嵌套版 m_min）
};

struct stQuestShopStatus
{
    int getMaxCount(int level);
    int getNextLevel(int level);
    void clear();                                  // W 0x891b8cc
    stQuestShopStatus();                           // W 0x891b948
    stQuestShopStatus(stQuestShopStatus const&);   // W 0x85f07fe
    ~stQuestShopStatus();                          // W 0x83cd826

    std::string m_name;                    // +0x00
    std::string m_name2;                   // +0x04
    std::string m_explain;                 // +0x08
    std::string m_icon;                    // +0x0c
    int m_field10;                         // +0x10（clear 置 -1）
    unsigned char m_questPoint;            // +0x14（clear 置 0）
    unsigned char m_pad15[3];
    int m_field18;                         // +0x18（clear 置 0x4d；movl 4 字节实证）
    int m_statusType;                      // +0x1c（clear 置 2）
    int m_field20;                         // +0x20（clear 置 0）
    std::vector<stStatusLevelInfo> m_levelInfos; // +0x24
};
static_assert(sizeof(stQuestShopStatus) == 0x30,
              "ORIG stQuestShopStatus layout drift");
// ORIG C1 0x85eebc8 size 0x97：CData 基类（vptr）、map@+0x08、map@+0x20 构造，
// _reset() 收尾；异常清理路径按 ORIG try/leave 版式由编译器生成。
CQuestShop::CQuestShop()
{
    _reset();   // ORIG +0x3e：map 构造后直调 _reset（085efd74）
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
void CQuestShop::setPiece(int value) { alter(); m_piece = value; } // ORIG alter 前缀
void CQuestShop::addQp(int value) { alter(); m_qp += value; }   // ORIG 8695a1c alter 前缀
void CQuestShop::subQp(int value) { alter(); m_qp -= value; }   // ORIG 85efebc alter 前缀
void CQuestShop::incInitQp(int value) { m_initQp += value; }
void CQuestShop::incInitCount() { alter(); ++m_initCount; }     // ORIG 85eff16 alter 前缀 +0x3c 自增
void CQuestShop::addPiece(int value) { alter(); m_piece += value; } // ORIG alter 前缀

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
    const int oldQp = getQp();          // -0x14
    user->setInitQuestPoint();
    const int initQp = getInitQp();     // -0x10
    setQp(initQp);
    clearBuyingStatuslist();
    // ORIG 0x85ef5d5：afterQp=getQp() 先于 getInitCount()（实参求值序），history = +0x79700
    const int afterQp = getQp();        // -0xc
    user->m_historyLog.ClearUsedQP(oldQp, afterQp, getInitCount(), 0);
}

void CQuestShop::makePacket_BuyingStatusList(PacketGuard& packet, bool flag)
{
    // ORIG 0x85ef5dc：flag 分支选 map（if(!flag) 取反布局）
    const std::map<unsigned char, short>* status;
    if (!flag)
        status = &m_buyingStatus;
    else
        status = &m_tagBuyingStatus;
    const unsigned char sz = static_cast<unsigned char>(status->size()); // -0x15
    packet.put_byte(static_cast<int>(sz));

    std::map<unsigned char, short>::const_iterator end = status->end(); // -0x20 循环前
    for (std::map<unsigned char, short>::const_iterator it =
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
int CQuestShop::subQp(unsigned char status, unsigned char count)
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
int CQuestShop::findStatusQp(unsigned char status)
{
    // ORIG 0x85eeda8：m_map0 局部 map 拷贝（非 const）+ find/end/operator->；
    // value+0x18 单字节读取（movzbl 实证 = second.m_questPoint +0x14）。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0); // +0x6398
    const int key = static_cast<int>(status);     // -0x10
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(key);
    if (it != table.end())                        // ORIG ne 比较 + je 跳过命中块
        return static_cast<int>(it->second.m_questPoint); // movzbl 0x18(%eax)
    return -1;
}

// ---- ORIG T 0x85eee6a size 0xbe：查该 status 的槽位类型 -------------------
// （stQuestShopStatus +0x1c dword；表外返回哨兵 0x4d）
int CQuestShop::findStatusType(unsigned char status)
{
    // ORIG 0x85eee6a：map 拷贝 + 非 const find/end；value+0x1c = second.m_field18。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    const int key = static_cast<int>(status);     // -0x10
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(key);
    if (it != table.end())                        // ORIG ne 比较
        return it->second.m_field18;              // mov 0x1c(%eax)（value+0x1c）
    return 0x4d;                                  // +0x85
}

// ---- ORIG T 0x85ef02e size 0xa0：读 m_buyingStatus[type] -----------------
short CQuestShop::getStatusBuyingCount(unsigned char status)
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
void CQuestShop::incStatusBuyingCount(unsigned char status, unsigned char count)
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

int stQuestShopStatus::getMaxCount(int level)
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

int stQuestShopStatus::getNextLevel(int level)
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
void stQuestShopStatus::clear()
{
    m_name.clear();
    m_name2.clear();
    m_explain.clear();
    m_icon.clear();
    m_field10 = -1;
    m_questPoint = 0;
    m_field18 = 0x4d;
    m_statusType = 2;
    m_field20 = 0;               // ORIG mov $0x0,%edx 中转（gcc4.1 特性，本工具链直写）
    m_levelInfos.clear();
}

// ORIG C1 0x891b948（objdump 直转）：4×string + vector 构造后 clear()。
stQuestShopStatus::stQuestShopStatus()
    : m_name(), m_name2(), m_explain(), m_icon(), m_levelInfos()
{
    clear();
}

// ORIG C1ERKS_ 0x85f07fe（objdump 直转）：4×string + 标量 + vector 复制。
stQuestShopStatus::stQuestShopStatus(stQuestShopStatus const& other)
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
stQuestShopStatus::~stQuestShopStatus()
{
}

// ---- ORIG T 0x85ef0ce size 0xec：限购等级校验 ----------------------------
bool CQuestShop::checkBuyingStatus_LimitLevel(unsigned char status, short count,
                                              int characLevel)
{
    // ORIG 0x85ef0ce：m_map0 局部 map 拷贝 + 非 const find/end；
    // value（+0x4 = second）直接按顶层镜像 stQuestShopStatus 调 getMaxCount。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    const int key = static_cast<int>(status);     // -0x14
    std::map<int, stQuestShopStatus>::iterator it = table.find(key); // -0x30
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
int CQuestShop::getInitCost(int count)
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
int CQuestShop::calUsedQp()
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
int CQuestShop::buyStatus(CUser* user, unsigned char status,
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
int CQuestShop::clearUsedQp(CUser* user)
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
void CQuestShop::_reset()
{
    m_buyingStatus.clear();          // 0x85efd83 map clear (+0x08)
    m_qp = 0;                        // movl $0,0x38(%eax)
    m_initCount = 0;                 // movl $0,0x3c(%eax)（ORIG 序：0x38→0x3c→0x40）
    m_initQp = 0;                    // movl $0,0x40(%eax)
    m_tagBuyingStatus.clear();       // +0x20 map clear
    m_piece = 0;                     // 0x44
}

// ---- ORIG T 0x85efdc0 size 0x94：装入 tag 状态计数 ------------------------
bool CQuestShop::SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data)
{
    // ORIG 按序列化块偏移访问 tag 区（0x85efdc0 movzwl 0xc+i*2）；
    // 形参为不完整类型，经 SIG 视图表达式内联访问（P86 逐次重读，不缓存）。
    // SIG_QUEST_SHOP_DATA 为 0x155 双实例（m_main@+0 与 CHARAC_QUEST_SHOP_DATA 同构）。
    for (int i = 0; i <= 0x4c; ++i)                 // cmp $0x4c setle
    {
        if (reinterpret_cast<const QuestShopStatusView*>(data)->m_statusCounts[i] == 0)
            continue;                               // movzwl + test ax / je
        // ORIG make_pair<IRiRt>(i, m_statusCounts[i]) 左值 + pair 移动 + insert
        m_tagBuyingStatus.insert(std::make_pair(
            i, reinterpret_cast<const QuestShopStatusView*>(data)->m_statusCounts[i]));
    }
    return 1;                                       // movl $0x1,%eax
}

// ---- ORIG T 0x85efa2c size 0x13a：loadData 虚槽 1 --------------------------
bool CQuestShop::loadData(CUser* user, char* data)
{
    _reset();   // ORIG call *(vtable+0x1c)（CData 虚槽 7 _reset）
    const SigQuestShopLoadBlock* block =
        reinterpret_cast<const SigQuestShopLoadBlock*>(data); // -0x10
    const SigQuestShopLoadBlock* p = block;         // -0xc（ORIG data→-0x10→-0xc 两步）
    m_qp = static_cast<int>(p->m_main.m_qp);        // mov 0x4→m_qp(+0x38)
    setInitCount(static_cast<int>(p->m_main.m_initCount));
    m_piece = static_cast<int>(p->m_main.m_questPiece); // 0xa6→(+0x44)
    for (int i = 0; i <= 0x4c; ++i)
    {
        if (reinterpret_cast<const QuestShopStatusView*>(p)->m_statusCounts[i] == 0)       // movzwl + test ax / je
            continue;
        // ORIG make_pair<IRiRt>(i, m_statusCounts[i]) 左值 + pair 移动 + insert
        m_buyingStatus.insert(std::make_pair(i, reinterpret_cast<const QuestShopStatusView*>(p)->m_statusCounts[i]));
    }
    if (p->m_tagFlag != 0)                          // movzbl 0x154(%eax)
    {
        p = reinterpret_cast<const SigQuestShopLoadBlock*>(
            &block->m_tag);   // ORIG SetTag 实参从 block(-0x10) 算 +0xaa 经 -0xc 中转
        SetTagCharacQuestShopData(const_cast<CHARAC_QUEST_SHOP_DATA*>(
            reinterpret_cast<const CHARAC_QUEST_SHOP_DATA*>(p)));
    }
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
    for (std::map<unsigned char, short>::iterator it =
             m_buyingStatus.begin();
         it != m_buyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // cmp $0x4c seta → skip
            continue;
        reinterpret_cast<QuestShopStatusView*>(view)->m_statusCounts[key] = value;   // mov %cx,0xc(key*2)
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard); // DB_Q @GlobalData
    return 1;                                       // ebx=1
}

// ---- ORIG T 0x85ef868 size 0x1c4：getData 虚槽 2（双实例导出）-------------
void CQuestShop::getData(char* out) const
{
    char* p = out;                                   // -0x18
    memset(p, 0, 0x155);                             // ORIG P68 装载序
    SIG_QUEST_SHOP_DATA* view =
        reinterpret_cast<SIG_QUEST_SHOP_DATA*>(p);   // -0x14
    view->m_main.m_qp = static_cast<unsigned int>(getQp());       // 0x4(%eax)
    view->m_main.m_initCount = static_cast<unsigned int>(getInitCount()); // 0x8
    view->m_main.m_questPiece = static_cast<unsigned int>(getPiece()); // 0xa6
    // ORIG 0x85ef868：判定时重读 it->first（第三次 ptEv，O0 无 CSE）
    for (std::map<unsigned char, short>::const_iterator it =
             m_buyingStatus.begin();
         it != m_buyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xf
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)                       // seta → skip 写入
            continue;
        reinterpret_cast<QuestShopStatusView*>(view)->m_statusCounts[key] = value;
    }
    view = reinterpret_cast<SIG_QUEST_SHOP_DATA*>(p + 0xaa); // +0x97a（ORIG 复用 -0x14）
    for (std::map<unsigned char, short>::const_iterator it =
             m_tagBuyingStatus.begin();
         it != m_tagBuyingStatus.end(); ++it)
    {
        const unsigned char key = it->first;        // -0xb
        const unsigned short value = static_cast<unsigned short>(it->second);
        if (it->first > 0x4c)
            continue;
        reinterpret_cast<QuestShopStatusView*>(view)->m_statusCounts[key] = value;
    }
}

// ---- ORIG W 0x0891b8b4：顶层 stStatusLevelInfo C1（+0x01 清零后拷至 +0x00）
// 合法 force-use 发射（vector<stStatusLevelInfo> 元素构造点）。 ----
namespace {
void emitStStatusLevelInfoWeakSymbols()
{
    // C1（构造）+ D1/D2（delete 完整对象析构）。
    stStatusLevelInfo* info = new stStatusLevelInfo();
    delete info;
}
}  // namespace
