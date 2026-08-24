#ifndef DNF_GAME_AVATAR_COIN_H_
#define DNF_GAME_AVATAR_COIN_H_

class CUser;
class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    int Add(unsigned int value);
    unsigned int Use();
    unsigned int Get() const;
    char IsUsable() const;
    static int GetPackage(CUser* user, int avatarUid, bool bReset); // ORIG 0x0818028a
    static int SaveToDB(CUser* user);                               // ORIG 0x081800d6
    static int SendSyncPacket(CUser* user);                         // ORIG 0x0817ffe4

    class HistoryLog  // ORIG 嵌套类 AvatarCoin::HistoryLog（0x817ff9c/0x817ff54）
    {
    public:
        static void AddLog(CUser* user, unsigned int amount);
    };
    unsigned int m_coin;
};

#endif
