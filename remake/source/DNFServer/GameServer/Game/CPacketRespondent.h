#ifndef GAME_CPACKETRESPONDENT_H
#define GAME_CPACKETRESPONDENT_H

#include <map>

#include "PacketGuard.h"

class CUser;

namespace user_creature
{

class CCreatureItem;

// ============================================================================
// user_creature::CPacketRespondent（宠物通知/命令包发送器）。
//
// ORIG 符号全集（nm --defined-only df_game_r，全部 W，区间 0x08340256..0x083409e0；
// 无独立 class_func_reports 目录，逐方法依据 objdump 反汇编转写，见
// /tmp/df_batches/CreatureMgr_send_full.md）：
//   ctor   _ZN13user_creature17CPacketRespondentC1EP5CUserii W 0x0834032a
//   dtor   _ZN13user_creature17CPacketRespondentD1Ev        W 0x0834036a
//   finalize _ZN...8finalizeEb                               W 0x083403b8
//   Send   _ZN...4SendEv                                     W 0x083403e0
//   Broadcast _ZN...9BroadcastEii                            W 0x08340404
//   SendAll _ZN...7SendAllEv                                 W 0x0834048a
//   IsGateArea _ZN...10IsGateAreaEP5CUseri                   W 0x08340256
//   SendAccordingToPlace _ZN...20SendAccordingToPlaceEv      W 0x083404b8
//   SetError _ZN...8SetErrorEi                               W 0x08340538
//   SetOk   _ZN...5SetOkEv                                   W 0x083405a2
//   MakeCmdpacketRenameCreature _ZN...27MakeCmdpacketRenameCreatureEii W 0x083405fa
//   MakeNotipacketRenameCreature _ZN...28MakeNotipacketRenameCreatureEPKci W 0x08340672
//   MakeNotipacketDiedCreature _ZN...26MakeNotipacketDiedCreatureEv W 0x083406fe
//   MakeNotipacketGainExpCreature _ZN...29MakeNotipacketGainExpCreatureEiib W 0x08340760
//   MakeNotipacketEvoluteCreature _ZN...29MakeNotipacketEvoluteCreatureEi W 0x08340842
//   MakeNotipacketRevivalCreature _ZN...29MakeNotipacketRevivalCreatureEv W 0x083408b6
//   MakeNotipacketCreatureState _ZN...27MakeNotipacketCreatureStateEii W 0x08340918
//   MakeNotipacketResponseCreature _ZN...30MakeNotipacketResponseCreatureEv W 0x0834097e
//   MakeNotipacketCreatureItemList
//     _ZN13user_creature17CPacketRespondent30MakeNotipacketCreatureItemListEPKSt3mapIiPNS_13CCreatureItemSt4lessIiESaISt4pairIKiPS3_EEEi
//                                                            W 0x083409e0
//
// 布局（ctor/dtor 反汇编实证）：基类 PacketGuard(0xc：InterfacePacketBuf +
// PacketPool* + m_external) 位于偏移 0；派生字段 +0x0c..+0x1f，总尺寸 0x20。
// dtor 在 +0x1c 非零时补发一次 Send（finalize(true) 后未显式 Send 的路径由
// 析构兜底）；Send/Broadcast/SendAll 均先清 +0x1c。
// ============================================================================
class CPacketRespondent : public PacketGuard
{
public:
    CPacketRespondent(CUser* user, int header, int subHeader);   // W 0x0834032a
    ~CPacketRespondent();                                        // W 0x0834036a

    // ORIG finalize(bool)：置 m_autoSend=1 后转发 InterfacePacketBuf::finalize。
    void finalize(bool encryptRequired);                         // W 0x083403b8
    // ORIG Send()：清 m_autoSend 后 m_user->Send(*this)，透传其返回值。
    int Send();                                                  // W 0x083403e0
    // ORIG Broadcast(vill, area)：门区直发本人，否则 GameWorld 区域广播。
    bool Broadcast(int vill, int area);                          // W 0x08340404
    // ORIG SendAll()：GameWorld 全服广播，恒返回 true。
    bool SendAll();                                              // W 0x0834048a
    // ORIG IsGateArea(user, area)：user 所在村 gate_area == area。
    bool IsGateArea(CUser* user, int area);                      // W 0x08340256
    // ORIG SendAccordingToPlace()：state==5 直发本人，否则按当前村/区 Broadcast。
    void SendAccordingToPlace();                                 // W 0x083404b8
    void SetError(int errorCode);                                // W 0x08340538
    void SetOk();                                                // W 0x083405a2
    void MakeCmdpacketRenameCreature(int param1, int param2);    // W 0x083405fa
    void MakeNotipacketRenameCreature(const char* name, int len);  // W 0x08340672
    void MakeNotipacketDiedCreature();                           // W 0x083406fe
    void MakeNotipacketGainExpCreature(int exp, int level,       // W 0x08340760
                                       bool growFlag);
    void MakeNotipacketEvoluteCreature(int slot);                // W 0x08340842
    void MakeNotipacketRevivalCreature();                        // W 0x083408b6
    void MakeNotipacketCreatureState(int uid, int state);        // W 0x08340918
    void MakeNotipacketResponseCreature();                       // W 0x0834097e
    // slot == -1 表示全量列出；仅收 GetCreatureItemType()==1 的条目，
    // 计数字节经 get_index()/put_byte(index,count) 回填。
    void MakeNotipacketCreatureItemList(                         // W 0x083409e0
        const std::map<int, CCreatureItem*>* items, int slot);

    CUser* m_user;            // +0x0c（ctor 参数 1）
    int m_header;             // +0x10（ctor 参数 2；SetError/SetOk put_header 用）
    int m_subHeader;          // +0x14（ctor 参数 3）
    int m_field18;            // +0x18（ctor 置 0；ORIG 各方法未读）
    unsigned char m_autoSend; // +0x1c（finalize 置 1；Send/Broadcast/SendAll 清 0）
};

}  // namespace user_creature

#endif  // GAME_CPACKETRESPONDENT_H
