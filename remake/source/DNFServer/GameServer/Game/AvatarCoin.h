#ifndef DNF_GAME_AVATAR_COIN_H_
#define DNF_GAME_AVATAR_COIN_H_

class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    int Add(unsigned int value);
    unsigned int Use();
    unsigned int Get() const;
    char IsUsable() const;
    unsigned int m_coin;
};

#endif
