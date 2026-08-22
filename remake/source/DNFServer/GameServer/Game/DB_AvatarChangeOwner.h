#ifndef GAME_DB_AVATARCHANGEOWNER_H_
#define GAME_DB_AVATARCHANGEOWNER_H_

// df_game_r 还原 —— DB_AvatarChangeOwner（球员换主 DB 请求，DBThread 派发）。
// 唯一声明点（原定义于 DB_AvatarChangeOwner.cpp，本头拆出供多 TU include）。
// 语义：向 DB 队列发一条 SIG_AVATAR_CHANGE_OWNER 请求。参考
// docs/class_func_reports/DB_AvatarChangeOwner/makeRequest.md，ORIG 0x0840055c。
class DB_AvatarChangeOwner
{
public:
    static void makeRequest(unsigned int a, unsigned int b, unsigned int c,
                            unsigned char d);   // ORIG T 0x0840055c
};

#endif  // GAME_DB_AVATARCHANGEOWNER_H_