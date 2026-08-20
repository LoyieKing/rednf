#ifndef DNF_GAME_CPARTY_TELEPORT_H_
#define DNF_GAME_CPARTY_TELEPORT_H_

struct stPartyTelePort
{
    void reset();
    char m_state;
    unsigned char m_village;
    unsigned char m_areaIndex;
    char m_pad1;
    short m_posx;
    short m_posy;
    unsigned char m_direction;
    char m_pad2[3];
};

class CParty;
class CUser;

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

    CParty* m_party;
    stPartyTelePort m_tp;
    char m_pad[0x14];
};

#endif
