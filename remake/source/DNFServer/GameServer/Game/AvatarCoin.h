#ifndef DNF_GAME_AVATAR_COIN_H_
#define DNF_GAME_AVATAR_COIN_H_

class CUser;
class Inven_Item;
class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    int Add(unsigned int value);
    bool Use();   // [b28d-Main 代落·S341 P292] unsigned int→bool（ORIG 0x08198e52 调用点 `call; xor $0x1,%eax; test %al,%al; je` 无 setcc ⇒ bool callee）
    unsigned int Get() const;
    static bool BuyProcess(CUser* user, Inven_Item& item);          // ORIG 0x081801d0
    bool IsUsable() const;
    static bool GetPackage(CUser* user, int avatarUid, bool bReset); // ORIG 0x0818028a
    // [b28c-lane17] ORIG 调用点 0x08180225/0x081802ce `xor $0x1,%eax; test %al`
    // ⇒ 返回 bool（P168a：bool callee 裸形判据），非 int。
    static bool SaveToDB(CUser* user);                              // ORIG 0x081800d6
    static bool SendSyncPacket(CUser* user);                        // ORIG 0x0817ffe4；调用点 xor $1 判 bool

    class HistoryLog  // ORIG 嵌套类 AvatarCoin::HistoryLog（0x817ff9c/0x817ff54）
    {
    public:
        static void AddLog(CUser* user, unsigned int amount);
        static void SubLog(CUser* user, unsigned int amount);  // ORIG 0x0817ff54
    };
    unsigned int m_coin;
};

#endif
