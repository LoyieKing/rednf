#ifndef DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_
#define DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_

// itemGloballyUniqueIdentifier_t / CItemGloballyUniqueIdentifierGenerator
// 唯一权威声明点（原 GameStubs 迁移稿已升级为真实布局）。
// ORIG 符号（nm -S 核验）：
//   _ZN30itemGloballyUniqueIdentifier_t5resetEv        W 0x085139e8（size 0x28）
//   _ZN30itemGloballyUniqueIdentifier_t11SetServerIdEi W 0x08894782（+0 写 int）
//   _ZN30itemGloballyUniqueIdentifier_t11SetDatetimeEi W 0x08894798（+4 写 int）
//   _ZN30itemGloballyUniqueIdentifier_t13SetSequenceNoEt W 0x088947b2（+8 写 u16）
//   _ZN38CItemGloballyUniqueIdentifierGenerator6generateEP30itemGloballyUniqueIdentifier_ti T 0x0889246c
//   _ZN38CItemGloballyUniqueIdentifierGenerator6getSeqEv W 0x088947e2
//   _ZN38CItemGloballyUniqueIdentifierGeneratorC1Ev    W 0x088947d4（movw $0）
//   g_ItemGloballyUniqueIdentifierGenerator            B 0x09494d68（sizeof 2）

struct itemGloballyUniqueIdentifier_t
{
    void reset();
    // 字段布局由三个 SetXxx 反汇编实证：
    void SetServerId(int id)                    { m_serverId = id; }
    void SetDatetime(int dt)                    { m_datetime = dt; }
    void SetSequenceNo(unsigned short seq)      { m_sequenceNo = seq; }

    int            m_serverId;                  // +0x00
    int            m_datetime;                  // +0x04
    unsigned short m_sequenceNo;                // +0x08
    char           m_tailPad[7];                // +0x0a..0x10（保持历史 0x11 视图）
};

class CItemGloballyUniqueIdentifierGenerator
{
public:
    CItemGloballyUniqueIdentifierGenerator() { m_seq = 0; }
    unsigned short getSeq();
    void generate(itemGloballyUniqueIdentifier_t* id, int field);

private:
    unsigned short m_seq;                       // @+0x00
};

extern CItemGloballyUniqueIdentifierGenerator g_ItemGloballyUniqueIdentifierGenerator;

#endif  // DNF_GAME_ITEMGLOBALLYUNIQUEIDENTIFIERGENERATOR_H_
