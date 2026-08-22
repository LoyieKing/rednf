// ==========================================================================
#include <vector>
#include "CPartyTelePort.h"
#include "CParty.h"
#include "CUser.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "PacketGuard.h"

struct TeleportEffect
{
    char village;
    int area;
    std::vector<unsigned short> users;
};

// ============================================================================
// CPartyTelePort（sizeof 0x24）
// ============================================================================

// ============================================================================
// 实现
// ============================================================================
// ---- stPartyTelePort（ORIG 0x822d764，memset 0，清 4 字节状态头）----
void stPartyTelePort::reset()
{
    memset(this, 0, 4);
}

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
    unsigned char* members = (unsigned char*)m_party->GetMember();
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
    unsigned char* members = (unsigned char*)m_party->GetMember();
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

    pg.clear();
    pg.put_header(0, 0x179);
    pg.put_byte((int)get_teleport_village());
    pg.put_byte((int)get_teleport_state());
    for (int i = 0; i <= 3; ++i)
    {
        short uid = -1;
        unsigned char mstate = 0;
        if (m_party->checkValidUser(i) != 0)
        {
            CUser* u = (CUser*)m_party->get_user(i);
            if (u != 0)
            {
                uid = (short)u->get_unique_id();
                mstate = (unsigned char)get_teleport_member_state(i);
            }
        }
        pg.put_short((int)uid);
        pg.put_byte((int)mstate);
    }
    pg.put_byte((int)status);
    pg.finalize(true);
    m_party->send_to_party(pg);

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
            if (m_party->checkValidUser(i) != 1)
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
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            CUser* user = (CUser*)m_party->get_user(i);
            if (user == 0)
            {
                continue;
            }
            int dir = (int)get_teleport_direction();
            int posy = (int)get_teleport_posy();
            int posx = (int)get_teleport_posx();
            unsigned char area = get_teleport_area_index();
            unsigned char village = get_teleport_village();
            moveResult = G_GameWorld()->check_move_area(user, (int)village, (int)area, posx, posy,
                dir, true, 0);
            if (m_party->getManager() == user)
            {
                moveResult = user->get_aura_avatar_option_value(0);
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
                if (m_party->checkValidUser(i) != 1)
                {
                    continue;
                }
                if (get_teleport_member_state(i) == 4)
                {
                    continue;
                }
                CUser* user = (CUser*)m_party->get_user(i);
                if (user == 0)
                {
                    continue;
                }
                user->set_charac_visible_teleport_(true);
                int dir = (int)get_teleport_direction();
                int posy = (int)get_teleport_posy();
                int posx = (int)get_teleport_posx();
                unsigned char area = get_teleport_area_index();
                unsigned char village = get_teleport_village();
                char moved = G_GameWorld()->move_area(user, (int)village, (int)area, posx, posy,
                    dir, true, 1, 0, 0);
                if (moved != 1)
                {
                    int charNo = user->getCurCharacNo();
                    cMyTrace tr("bool CPartyTelePort::process_teleport()",
                                    0x43f9, 5);
                    tr("party_teleport_error:(char_no:%d)", charNo);
                }
                user->set_charac_visible_teleport_(false);
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
        CUser* manager = (CUser*)m_party->getManager();
        if (manager != 0)
        {
            int curSec = ((CSystemTime*)0x941f714)->getCurSec();
            int newTime = curSec + auraAvatarTime;
            manager->set_aura_avatar_option_value(0,newTime);
            int optValue = manager->get_aura_avatar_option_value(0);
            manager->send_aura_avatar_option();
            int charNo = manager->getCurCharacNo();
            cMyTrace tr("bool CPartyTelePort::process_teleport()",
                            0x4419, 0);
            tr("party_teleport_success:(char_no:%d)", charNo);
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
    if (user->get_state() != 3)
    {
        return;
    }
    if (get_teleport_state() == -1)
    {
        return;
    }

    int slot = m_party->GetMemberSlotNo(user);
    if (slot < 0)
    {
        return;
    }
    if (slot > 3)
    {
        return;
    }
    if (m_party->checkValidUser(slot) != 1)
    {
        return;
    }

    if (m_party->getManager() == user)
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

    std::vector<TeleportEffect> effects;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) != 1)
        {
            continue;
        }
        CUser* user = (CUser*)m_party->get_user(i);
        if (user == 0)
        {
            continue;
        }

        bool found = false;
        for (std::vector<TeleportEffect>::iterator it = effects.begin();
             it != effects.end(); ++it)
        {
            if (it->area == user->get_area(false) &&
                it->village == user->getCurCharacVill())
            {
                found = true;
                it->users.push_back(user->get_unique_id());
                break;
            }
        }
        if (!found)
        {
            TeleportEffect ed = { user->getCurCharacVill(), user->get_area(false),
                std::vector<unsigned short>(1, user->get_unique_id()) };
            effects.push_back(ed);
        }
    }

    for (unsigned int i = 0; i < effects.size(); ++i)
    {
        PacketGuard pg;

        pg.put_header(0, 0x17b);
        pg.put_byte((int)(char)param);
        int cnt = (int)effects[i].users.size();
        pg.put_byte((int)(char)cnt);
        for (int j = 0; j < cnt; ++j)
        {
            pg.put_short((int)effects[i].users[j]);
        }
        pg.finalize(true);
        G_GameWorld()->send_to_area(
                                   (int)effects[i].village,
                                   effects[i].area, pg);

    }
}

void CPartyTelePort::send_teleport_effect_at_equal_place(int param)
{
    if (m_party == 0)
    {
        return;
    }
    CUser* manager = (CUser*)m_party->getManager();
    if (manager == 0)
    {
        return;
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        return;
    }

    PacketGuard pg;

    pg.put_header(0, 0x17b);
    pg.put_byte((int)(char)param);

    std::vector<unsigned short> users;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) == 1)
        {
            CUser* u = (CUser*)m_party->get_user(i);
            if (u != 0)
            {
                users.push_back(u->get_unique_id());
            }
        }
    }

    char cnt = (char)users.size();
    pg.put_byte((int)cnt);
    for (int j = 0; j < (int)cnt; ++j)
    {
        pg.put_short((int)users[j]);
    }
    pg.finalize(true);

    if (manager->get_state() == 3)
    {
        int area = manager->get_area(false);
        char vill = manager->getCurCharacVill();
        G_GameWorld()->send_to_area( (int)vill, area, pg);
    }

}
