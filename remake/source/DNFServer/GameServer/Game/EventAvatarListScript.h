#ifndef GAME_EVENTAVATARLISTSCRIPT_H_
#define GAME_EVENTAVATARLISTSCRIPT_H_
#include <set>
class EventAvatarListScript {
public:
    EventAvatarListScript();
    ~EventAvatarListScript();
    bool isfindEventAvatar(int avatar_id) const;
    std::set<int>& avatars() { return m_avatars; }
    const std::set<int>& avatars() const { return m_avatars; }
private:
    std::set<int> m_avatars;
};
#endif
