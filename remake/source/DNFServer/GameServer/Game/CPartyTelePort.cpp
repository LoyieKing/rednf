// ============================================================================
// df_game_r CPartyTelePort 还原（G2-5 批次，自包含 TU）
// 逐函数对照 docs/class_func_reports/CPartyTelePort.md 与 ORIG 反汇编实现。
// 布局（ORIG 构造/析构/弱 getter 推导，sizeof 0x24，CParty +0x1ad0）：
//   +0x00 CParty* m_party
//   +0x04 stPartyTelePort m_tp（0xc 字节：state/village/area/pad/x/y/dir/pad）
//   +0x10 保留区（0x14 字节）
// 成员状态存于 CParty::GetMember() 数组（每项 0x18，状态 +0x12）。
// 跨类调用一律 extern "C" asm("_ZN...")，mangled 名与 ORIG 完全一致。
// ============================================================================

#include <vector>

class CParty;
class CUser;

// ---- stPartyTelePort（0xc；reset 由 ORIG/桩提供 _ZN15stPartyTelePort5resetEv）----
class stPartyTelePort
{
public:
    void reset();

    char m_state;              // +0x00（CPartyTelePort 绝对 +0x04）
    unsigned char m_village;   // +0x01
    unsigned char m_areaIndex; // +0x02
    char m_pad1;               // +0x03
    short m_posx;              // +0x04
    short m_posy;              // +0x06
    unsigned char m_direction; // +0x08
    char m_pad2[3];            // +0x09..0x0b
};

// ---- PacketGuard（0xc；C1/D1 由 ORIG/桩提供，本 TU 手动调用）----
class PacketGuard
{
public:
    char m_pad[0xc];
};

// ---- effect_data（0x14：village + area + vector<unsigned short>；C1/D1 外部）----
class effect_data
{
public:
    effect_data();
    ~effect_data();

    char m_village;                  // +0x00
    char m_pad[3];                   // +0x01..0x03
    int m_area;                      // +0x04
    std::vector<unsigned short> m_users;  // +0x08
};

// ---- cMyTrace（局部占位；C1/operator() 由 ORIG/桩提供）----
class cMyTrace
{
public:
    char m_pad[0x10];
};

class CSystemTime
{
public:
    int getCurSec();
};

class GameWorld;

// ============================================================================
// 跨类调用（asm-label extern，mangled 名与 ORIG 一致）
// ============================================================================
extern "C" void sub_stPartyTelePort_reset(void* self)
    asm("_ZN15stPartyTelePort5resetEv");
extern "C" void sub_PacketGuard_C1(void* self)
    asm("_ZN11PacketGuardC1Ev");
extern "C" void sub_PacketGuard_D1(void* self)
    asm("_ZN11PacketGuardD1Ev");

extern "C" void sub_IPB_clear(void* self)
    asm("_ZN18InterfacePacketBuf5clearEv");
extern "C" void sub_IPB_put_header(void* self, int a, int b)
    asm("_ZN18InterfacePacketBuf10put_headerEii");
extern "C" void sub_IPB_put_byte(void* self, int v)
    asm("_ZN18InterfacePacketBuf8put_byteEi");
extern "C" void sub_IPB_put_short(void* self, int v)
    asm("_ZN18InterfacePacketBuf9put_shortEi");
extern "C" void sub_IPB_finalize(void* self, bool b)
    asm("_ZN18InterfacePacketBuf8finalizeEb");

extern "C" void sub_cMyTrace_C1(void* self, const char* file, int line, int level)
    asm("_ZN8cMyTraceC1EPKcii");
extern "C" void sub_cMyTrace_op(void* self, const char* fmt, ...)
    asm("_ZN8cMyTraceclEPKcz");

// CParty
extern "C" char sub_CParty_checkValidUser(void* self, int idx)
    asm("_ZN6CParty14checkValidUserEi");
extern "C" void* sub_CParty_get_user(void* self, int idx)
    asm("_ZN6CParty8get_userEi");
extern "C" void* sub_CParty_GetMember(void* self)
    asm("_ZN6CParty9GetMemberEv");
extern "C" void* sub_CParty_getManager(void* self)
    asm("_ZN6CParty10getManagerEv");
extern "C" int sub_CParty_GetMemberSlotNo(const void* self, const void* user)
    asm("_ZNK6CParty15GetMemberSlotNoEPK5CUser");
extern "C" void sub_CParty_send_to_party(void* self, void* pg)
    asm("_ZN6CParty13send_to_partyER11PacketGuard");

// CUser
extern "C" int sub_CUser_get_state(void* self)
    asm("_ZN5CUser9get_stateEv");
extern "C" unsigned short sub_CUser_get_unique_id(const void* self)
    asm("_ZNK5CUser13get_unique_idEv");
extern "C" int sub_CUser_get_area(void* self, bool b)
    asm("_ZN5CUser8get_areaEb");
extern "C" int sub_CUser_is_equip_aura_avatar(void* self, char c, int* out)
    asm("_ZN5CUser20is_equip_aura_avatarEcRi");
extern "C" int sub_CUser_get_aura_avatar_option_value(void* self, int idx)
    asm("_ZN5CUser28get_aura_avatar_option_valueEi");
extern "C" void sub_CUser_UpdateAuraAvatarOption(void* self, int a, int b)
    asm("_ZN5CUser22UpdateAuraAvatarOptionEii");

// CUserCharacInfo（方法在 CUser* 上调用的 this-call）
extern "C" void sub_CUserCharacInfo_set_charac_visible_teleport(void* self, bool b)
    asm("_ZN15CUserCharacInfo28set_charac_visible_teleport_Eb");
extern "C" int sub_CUserCharacInfo_getCurCharacNo(const void* self)
    asm("_ZNK15CUserCharacInfo14getCurCharacNoEv");
extern "C" char sub_CUserCharacInfo_getCurCharacVill(const void* self)
    asm("_ZNK15CUserCharacInfo16getCurCharacVillEv");
extern "C" void sub_CUserCharacInfo_set_aura_avatar_option_value(void* self, int a, int b)
    asm("_ZN15CUserCharacInfo28set_aura_avatar_option_valueEii");

// GameWorld
extern "C" void* sub_G_GameWorld()
    asm("_Z11G_GameWorldv");
extern "C" int sub_GameWorld_check_move_area(void* self, void* user, int village,
                                             int area, int x, int y, int dir,
                                             bool a, int b)
    asm("_ZN9GameWorld15check_move_areaEP5CUseriiiiibi");
extern "C" char sub_GameWorld_move_area(void* self, void* user, int village,
                                        int area, int x, int y, int dir,
                                        bool a, int b, int c, int d)
    asm("_ZN9GameWorld9move_areaEP5CUseriiiiibiii");
extern "C" char sub_GameWorld_IsPVPChannel(void* self)
    asm("_ZNK9GameWorld12IsPVPChannelEv");
extern "C" void sub_GameWorld_send_to_area(void* self, int village, int area, void* pg)
    asm("_ZN9GameWorld12send_to_areaEiiR11PacketGuard");

// ============================================================================
// CPartyTelePort（sizeof 0x24）
// ============================================================================
class CPartyTelePort
{
public:
    CPartyTelePort();
    ~CPartyTelePort();

    void init(CParty* party);
    void set_teleport_data(unsigned char village, unsigned char area,
                           short posx, short posy,
                           unsigned char direction, char state);
    void set_teleport_state(char state);
    void send_teleport_status(unsigned char status);
    void set_teleport_member_state(int idx, char state);
    void reset_teleport_data();
    char check_cur_teleport_state();
    char process_teleport();
    void process_leave_user_at_teleport(CUser* user);
    void send_teleport_effect_at_different_place(int param);
    void send_teleport_effect_at_equal_place(int param);

    unsigned int get_teleport_member_state(int idx);
    unsigned char get_teleport_village();
    unsigned char get_teleport_area_index();
    short get_teleport_posx();
    short get_teleport_posy();
    unsigned char get_teleport_direction();
    char get_teleport_state();

    CParty* m_party;         // +0x00
    stPartyTelePort m_tp;    // +0x04（0xc）
    char m_pad[0x14];        // +0x10..+0x23
};

// ============================================================================
// 实现
// ============================================================================

CPartyTelePort::CPartyTelePort()
{
    m_tp.reset();
}

CPartyTelePort::~CPartyTelePort()
{
}

void CPartyTelePort::init(CParty* party)
{
    m_party = party;
}

void CPartyTelePort::set_teleport_data(unsigned char village, unsigned char area,
                                       short posx, short posy,
                                       unsigned char direction, char state)
{
    m_tp.reset();
    m_tp.m_village = village;
    m_tp.m_areaIndex = area;
    m_tp.m_posx = posx;
    m_tp.m_posy = posy;
    m_tp.m_direction = direction;
    m_tp.m_state = state;
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, 2);
    }
}

void CPartyTelePort::set_teleport_state(char state)
{
    m_tp.m_state = state;
}

unsigned int CPartyTelePort::get_teleport_member_state(int idx)
{
    if (m_party == 0)
    {
        return (unsigned int)-1;
    }
    if (idx < 0 || idx > 3)
    {
        return (unsigned int)-1;
    }
    if (m_party == 0)
    {
        return (unsigned int)-1;
    }
    unsigned char* members = (unsigned char*)sub_CParty_GetMember(m_party);
    return members[idx * 0x18 + 0x12];
}

void CPartyTelePort::set_teleport_member_state(int idx, char state)
{
    if (m_party == 0)
    {
        return;
    }
    if (idx < 0)
    {
        return;
    }
    if (idx > 3)
    {
        return;
    }
    unsigned char* members = (unsigned char*)sub_CParty_GetMember(m_party);
    members[idx * 0x18 + 0x12] = state;
}

unsigned char CPartyTelePort::get_teleport_village()
{
    return m_tp.m_village;
}

unsigned char CPartyTelePort::get_teleport_area_index()
{
    return m_tp.m_areaIndex;
}

short CPartyTelePort::get_teleport_posx()
{
    return m_tp.m_posx;
}

short CPartyTelePort::get_teleport_posy()
{
    return m_tp.m_posy;
}

unsigned char CPartyTelePort::get_teleport_direction()
{
    return m_tp.m_direction;
}

char CPartyTelePort::get_teleport_state()
{
    return m_tp.m_state;
}

void CPartyTelePort::send_teleport_status(unsigned char status)
{
    if (m_party == 0)
    {
        return;
    }

    PacketGuard pg;
    sub_PacketGuard_C1(&pg);
    sub_IPB_clear(&pg);
    sub_IPB_put_header(&pg, 0, 0x179);
    sub_IPB_put_byte(&pg, (int)get_teleport_village());
    sub_IPB_put_byte(&pg, (int)get_teleport_state());
    for (int i = 0; i <= 3; ++i)
    {
        short uid = -1;
        unsigned char mstate = 0;
        if (sub_CParty_checkValidUser(m_party, i) != 0)
        {
            CUser* u = (CUser*)sub_CParty_get_user(m_party, i);
            if (u != 0)
            {
                uid = (short)sub_CUser_get_unique_id(u);
                mstate = (unsigned char)get_teleport_member_state(i);
            }
        }
        sub_IPB_put_short(&pg, (int)uid);
        sub_IPB_put_byte(&pg, (int)mstate);
    }
    sub_IPB_put_byte(&pg, (int)status);
    sub_IPB_finalize(&pg, true);
    sub_CParty_send_to_party(m_party, &pg);
    sub_PacketGuard_D1(&pg);
}

void CPartyTelePort::reset_teleport_data()
{
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, -1);
    }
    m_tp.reset();
    set_teleport_state(-1);
}

char CPartyTelePort::check_cur_teleport_state()
{
    char result;
    if (m_party == 0)
    {
        result = 0;
    }
    else
    {
        result = 1;
        for (int i = 0; i <= 3; ++i)
        {
            if (sub_CParty_checkValidUser(m_party, i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 1)
            {
                continue;
            }
            result = 0;
        }
        if (result != 0)
        {
            set_teleport_state(1);
        }
    }
    return result;
}

char CPartyTelePort::process_teleport()
{
    char result;
    if (m_party == 0)
    {
        return 0;
    }

    result = check_cur_teleport_state();
    int moveResult = 0;
    int auraAvatarTime = 0;
    if (result != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (sub_CParty_checkValidUser(m_party, i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            CUser* user = (CUser*)sub_CParty_get_user(m_party, i);
            if (user == 0)
            {
                continue;
            }
            int dir = (int)get_teleport_direction();
            int posy = (int)get_teleport_posy();
            int posx = (int)get_teleport_posx();
            unsigned char area = get_teleport_area_index();
            unsigned char village = get_teleport_village();
            moveResult = sub_GameWorld_check_move_area(
                sub_G_GameWorld(), user, (int)village, (int)area, posx, posy,
                dir, true, 0);
            if (sub_CParty_getManager(m_party) == user)
            {
                moveResult = sub_CUser_is_equip_aura_avatar(user, 0, &auraAvatarTime);
            }
            if (moveResult > 0)
            {
                set_teleport_member_state(i, 0);
                set_teleport_state(2);
                break;
            }
        }

        if (get_teleport_state() == 1)
        {
            send_teleport_effect_at_different_place(1);
            for (int i = 0; i <= 3; ++i)
            {
                if (sub_CParty_checkValidUser(m_party, i) != 1)
                {
                    continue;
                }
                if (get_teleport_member_state(i) == 4)
                {
                    continue;
                }
                CUser* user = (CUser*)sub_CParty_get_user(m_party, i);
                if (user == 0)
                {
                    continue;
                }
                sub_CUserCharacInfo_set_charac_visible_teleport(user, true);
                int dir = (int)get_teleport_direction();
                int posy = (int)get_teleport_posy();
                int posx = (int)get_teleport_posx();
                unsigned char area = get_teleport_area_index();
                unsigned char village = get_teleport_village();
                char moved = sub_GameWorld_move_area(
                    sub_G_GameWorld(), user, (int)village, (int)area, posx, posy,
                    dir, true, 1, 0, 0);
                if (moved != 1)
                {
                    int charNo = sub_CUserCharacInfo_getCurCharacNo(user);
                    cMyTrace tr;
                    sub_cMyTrace_C1(&tr, "bool CPartyTelePort::process_teleport()",
                                    0x43f9, 5);
                    sub_cMyTrace_op(&tr, "party_teleport_error:(char_no:%d)", charNo);
                }
                sub_CUserCharacInfo_set_charac_visible_teleport(user, false);
            }
            send_teleport_effect_at_equal_place(2);
        }
        else
        {
            send_teleport_status((unsigned char)moveResult);
        }
    }

    if (get_teleport_state() == 1)
    {
        reset_teleport_data();
        CUser* manager = (CUser*)sub_CParty_getManager(m_party);
        if (manager != 0)
        {
            int curSec = ((CSystemTime*)0x941f714)->getCurSec();
            int newTime = curSec + auraAvatarTime;
            sub_CUserCharacInfo_set_aura_avatar_option_value(manager, 0, newTime);
            int optValue = sub_CUser_get_aura_avatar_option_value(manager, 0);
            sub_CUser_UpdateAuraAvatarOption(manager, 0, optValue);
            int charNo = sub_CUserCharacInfo_getCurCharacNo(manager);
            cMyTrace tr;
            sub_cMyTrace_C1(&tr, "bool CPartyTelePort::process_teleport()",
                            0x4419, 0);
            sub_cMyTrace_op(&tr, "party_teleport_success:(char_no:%d)", charNo);
        }
    }
    return result;
}

void CPartyTelePort::process_leave_user_at_teleport(CUser* user)
{
    if (m_party == 0)
    {
        return;
    }
    if (get_teleport_state() < 0)
    {
        return;
    }
    if (get_teleport_state() > 3)
    {
        return;
    }
    if (sub_CUser_get_state(user) != 3)
    {
        return;
    }
    if (get_teleport_state() == -1)
    {
        return;
    }

    int slot = sub_CParty_GetMemberSlotNo(m_party, user);
    if (slot < 0)
    {
        return;
    }
    if (slot > 3)
    {
        return;
    }
    if (sub_CParty_checkValidUser(m_party, slot) != 1)
    {
        return;
    }

    if (sub_CParty_getManager(m_party) == user)
    {
        set_teleport_member_state(slot, 0);
        set_teleport_state(2);
        send_teleport_status(8);
        reset_teleport_data();
    }
    else
    {
        set_teleport_member_state(slot, 4);
        check_cur_teleport_state();
        set_teleport_member_state(slot, 0);
        send_teleport_status(0);
        set_teleport_member_state(slot, 4);
        process_teleport();
    }
}

void CPartyTelePort::send_teleport_effect_at_different_place(int param)
{
    if (m_party == 0)
    {
        return;
    }

    std::vector<effect_data> effects;
    for (int i = 0; i <= 3; ++i)
    {
        if (sub_CParty_checkValidUser(m_party, i) != 1)
        {
            continue;
        }
        CUser* user = (CUser*)sub_CParty_get_user(m_party, i);
        if (user == 0)
        {
            continue;
        }

        bool found = false;
        for (std::vector<effect_data>::iterator it = effects.begin();
             it != effects.end(); ++it)
        {
            if (it->m_area == sub_CUser_get_area(user, false) &&
                it->m_village == sub_CUserCharacInfo_getCurCharacVill(user))
            {
                found = true;
                it->m_users.push_back(sub_CUser_get_unique_id(user));
                break;
            }
        }
        if (!found)
        {
            effect_data ed;
            ed.m_area = sub_CUser_get_area(user, false);
            ed.m_village = sub_CUserCharacInfo_getCurCharacVill(user);
            ed.m_users.push_back(sub_CUser_get_unique_id(user));
            effects.push_back(ed);
        }
    }

    for (unsigned int i = 0; i < effects.size(); ++i)
    {
        PacketGuard pg;
        sub_PacketGuard_C1(&pg);
        sub_IPB_put_header(&pg, 0, 0x17b);
        sub_IPB_put_byte(&pg, (int)(char)param);
        int cnt = (int)effects[i].m_users.size();
        sub_IPB_put_byte(&pg, (int)(char)cnt);
        for (int j = 0; j < cnt; ++j)
        {
            sub_IPB_put_short(&pg, (int)effects[i].m_users[j]);
        }
        sub_IPB_finalize(&pg, true);
        sub_GameWorld_send_to_area(sub_G_GameWorld(),
                                   (int)effects[i].m_village,
                                   effects[i].m_area, &pg);
        sub_PacketGuard_D1(&pg);
    }
}

void CPartyTelePort::send_teleport_effect_at_equal_place(int param)
{
    if (m_party == 0)
    {
        return;
    }
    CUser* manager = (CUser*)sub_CParty_getManager(m_party);
    if (manager == 0)
    {
        return;
    }
    if (sub_GameWorld_IsPVPChannel(sub_G_GameWorld()) != 0)
    {
        return;
    }

    PacketGuard pg;
    sub_PacketGuard_C1(&pg);
    sub_IPB_put_header(&pg, 0, 0x17b);
    sub_IPB_put_byte(&pg, (int)(char)param);

    std::vector<unsigned short> users;
    for (int i = 0; i <= 3; ++i)
    {
        if (sub_CParty_checkValidUser(m_party, i) == 1)
        {
            CUser* u = (CUser*)sub_CParty_get_user(m_party, i);
            if (u != 0)
            {
                users.push_back(sub_CUser_get_unique_id(u));
            }
        }
    }

    char cnt = (char)users.size();
    sub_IPB_put_byte(&pg, (int)cnt);
    for (int j = 0; j < (int)cnt; ++j)
    {
        sub_IPB_put_short(&pg, (int)users[j]);
    }
    sub_IPB_finalize(&pg, true);

    if (sub_CUser_get_state(manager) == 3)
    {
        int area = sub_CUser_get_area(manager, false);
        char vill = sub_CUserCharacInfo_getCurCharacVill(manager);
        sub_GameWorld_send_to_area(sub_G_GameWorld(), (int)vill, area, &pg);
    }
    sub_PacketGuard_D1(&pg);
}
