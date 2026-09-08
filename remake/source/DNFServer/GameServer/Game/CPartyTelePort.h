#ifndef DNF_GAME_CPARTY_TELEPORT_H_
#define DNF_GAME_CPARTY_TELEPORT_H_

struct stPartyTelePort
{
    void reset();
    unsigned char get_teleport_village() const;
    unsigned char get_teleport_area_index() const;
    unsigned short get_teleport_posx() const;
    unsigned short get_teleport_posy() const;
    unsigned char get_teleport_direction() const;
    char m_state;
    unsigned char m_village;
    unsigned char m_areaIndex;
    char m_pad1;
    short m_posx;
    short m_posy;
    // 字段宽度权威证据（撤销一次 short 误判）：ORIG mangled
    // _ZN14CPartyTelePort16set_teleport_dataEhhsshc 参数编码 = h,h,s,s,**h**,c
    // ⇒ direction 是 unsigned char；0x085bb88c `mov %dl,-0x3c(%ebp)`（1 字节
    // store）与 0x085bf68e `movzbl`（1 字节读）双向实证。
    unsigned char m_direction;
    // ORIG set_party_teleport 0x08283d98 整结构赋值只拷 4+4+2=10 字节（两 dword+一 word
    // 覆盖 direction+1 pad）⇒ sizeof=0xa，pad2 仅 1 字节（CU19B 证）。
    char m_pad2[1];
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
    // SIGFIX:CPartyTelePort::process_teleport — ORIG __PRETTY_FUNCTION__ 串
    // "bool CPartyTelePort::process_teleport()"（返回类型不入 mangled）。
    bool process_teleport();
    void process_leave_user_at_teleport(CUser* user);
    void send_teleport_effect_at_different_place(int param);
    void send_teleport_effect_at_equal_place(int param);
    // [b28-L14] ORIG 调用点 check_cur_teleport_state 0x85bbb4x 为 `call
    // get_teleport_member_state; cmp $0x4,%al`（单字节直测）⇒ 返回类型 char
    // （mangled 不含返回类型，P159/P27）。
    char get_teleport_member_state(int idx);
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
