// df_game_r 还原 —— QuickParty 独立类声明（batch-5 迁移自 GameStubs.cpp）。
// 权威符号（ORIG df_game_r）：
//   QuickParty::CQuickParty::CQuickParty()                     T @0x0826952c
//   QuickParty::CQuickParty::~CQuickParty()                    T @0x08269540
//   QuickParty::CQuickParty::get_quick_party_index() const     W @0x0826b74e
//   QuickParty::CQuickParty::init()                            T @0x08269546
//   QuickParty::CQuickParty::set_quick_party_index(int)        W @0x082a69be
//   QuickParty::CQuickPartySystemManager::CQuickPartySystemManager()       T @0x08269bb6
//   QuickParty::CQuickPartySystemManager::~CQuickPartySystemManager()      T @0x08269bf8
//   QuickParty::CQuickPartySystemManager::init_quick_party_pool(CDungeon const*)  T @0x08269c0c
//   QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*,bool,CUser*) T @0x0826a386

#ifndef QUICKPARTY_H
#define QUICKPARTY_H

class CDungeon;
class CParty;
class CUser;

namespace QuickParty
{

class CQuickParty
{
public:
    CQuickParty();
    ~CQuickParty();

    int get_quick_party_index() const;
    void init();
    void set_quick_party_index(int idx);

private:
    char m_pad[0x28];
};

class CQuickPartySystemManager
{
public:
    CQuickPartySystemManager();
    ~CQuickPartySystemManager();

    void init_quick_party_pool(const CDungeon* dungeon);
    int cancel_quick_party(CParty* party, bool flag, CUser* user);  // ORIG 返回 0/1（undefined4）

private:
    char m_pad[0x100];
};

}  // namespace QuickParty

#endif  // QUICKPARTY_H