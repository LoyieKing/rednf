#ifndef GAME_EVENTAVATARLISTSCRIPT_H_
#define GAME_EVENTAVATARLISTSCRIPT_H_
#include <set>
class EventAvatarListScript {
public:
    EventAvatarListScript();
    ~EventAvatarListScript();
    void clear();  // ORIG _ZN21EventAvatarListScript5clearEv W 0x8372e52
    bool isfindEventAvatar(int avatar_id) const;
    std::set<int>& avatars() { return m_avatars; }
    const std::set<int>& avatars() const { return m_avatars; }
private:
    std::set<int> m_avatars;
    // import 装载器按 ORIG 形态直接 insert 到成员集合（ORIG 内联于 this+0）。
    friend bool importEventAvatarListScript(EventAvatarListScript* s, const char* path);
};
#endif
