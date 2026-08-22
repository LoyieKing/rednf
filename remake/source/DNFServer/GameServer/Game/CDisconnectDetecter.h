#ifndef DNF_GAME_CDISCONNECTDETECTER_H_
#define DNF_GAME_CDISCONNECTDETECTER_H_

// ============================================================================
// df_game_r disconnect_detecter::CDisconnectDetecter（断线检测器）。
// 唯一声明点（原 CDisconnectDetecter.cpp 本地类迁出；实现仍在
// CDisconnectDetecter.cpp）。ORIG 符号：
//   RegisterUser 0x08472272 / UnregisterUser 0x0847236a /
//   GetInstanceDisconnectDetecter 0x082348df
// 布局：+0x00 std::map<DetectedObjectKey,int> m_map（sizeof 0x18）。
// ============================================================================

#include <map>

class CUser;

namespace disconnect_detecter
{

class DetectedObjectKey
{
public:
    DetectedObjectKey() {}
    DetectedObjectKey(int uid, int uniqueId);   // 0x0847266e W
    bool operator<(const DetectedObjectKey& other) const;  // 0x0847262a W

    int m_uid;        // +0x00
    int m_uniqueId;   // +0x04
};

class CDisconnectDetecter
{
public:
    CDisconnectDetecter();    // 0x082348cc W
    ~CDisconnectDetecter();   // 0x082348b8 W

    bool RegisterUser(CUser* user);            // 0x08472272 T
    bool UnregisterUser(CUser* user);          // 0x0847236a T
    bool RefreshRecvTime(CUser* user);         // 0x08472408 T
    bool DoDetectingDisconnect();              // 0x084724b0 T
    void OnDisconnect(CUser* user);            // 0x084725ba T

    bool _IsTimeout(int time);                 // 0x084721f4 T
    CUser* _GetUser(int uid, int uniqueId);    // 0x08472222 T

    std::map<DetectedObjectKey, int> m_map;    // +0x00
};

CDisconnectDetecter* GetInstanceDisconnectDetecter();  // 0x082348df W

}  // namespace disconnect_detecter

#endif  // DNF_GAME_CDISCONNECTDETECTER_H_
