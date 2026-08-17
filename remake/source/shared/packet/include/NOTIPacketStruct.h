//
// NOTIPacketStruct.h - advancealtar NOTI packet payload family (df_game_r)
//
// Reconstructed from:
//   - ctors @ /tmp/cmd_noti_decompiled.txt
//     _NotiAdvanceAltarSpawnUnit  @ 0x81346a6  (call advancealtar::_SummonObject ctor @ this+4, then memset(this,0,0x28))
//     _NotiAdvanceAltarStageInfo  @ 0x81346d8  (memset(this,0,8))
//     _NotiAdvanceAltarMapInfo    @ 0x81346fc  (memset(this,0,0x14))
//     _NotiAdvanceAltarClearStage @ 0x8134720  (call advancealtar::_Stage ctor @ this+0xd, then memset(this,0,0x1c))
//     _NotiAdvanceAltarEplpState  @ 0x8134752  (memset(this,0,8))
//     _NotiAdvanceAltarUpdateRidable @ 0x8141ff8 (memset(this,0,1))
//   - senders (objdump of df_game_r):
//     summonObjectAndSendPacket @ 0x812d658  header 0x20b, put_binary 0x28
//     sendStageInfo             @ 0x81300f6  header 0x209, put_binary 8
//     sendMapInfo               @ 0x81301e0  header 0x20a, put_binary 0x14
//     sendEplp                  @ 0x81306be  header 0x20e, put_binary 8
//     sendClearStage            @ 0x812e6de  header 0x20f, put_binary 0x1c
//     Dispatcher_ResetStar::process_packet @ 0x81410aa  header 0x210, put_binary 1
//   - sub-object layout evidence:
//     advancealtar::_SummonObject ctor @ 0x8134650 (memset 0x24);
//       fields partially named from StageTimeLineParameter::importSummonObject @ 0x889a21e
//     advancealtar::_Stage ctor @ 0x8134452 (memset 8);
//       fields named from sendClearStage (index @ +0, difficulty @ +4) and putRidableInfoToPacket @ 0x8141781 (put_binary 8)
//
// Layout is packed (ClearStage puts _Stage at unaligned offset 0xd and an int at 0x18).
//

#ifndef NOTIPACKETSTRUCT_H
#define NOTIPACKETSTRUCT_H

#include <cstring>

namespace advancealtar {

// 36-byte summon-object record (sent verbatim inside _NotiAdvanceAltarSpawnUnit).
// ctor @0x8134650: memset(this,0,0x24).
struct _SummonObject {
    int            summonId;      // +0x00  script ScanInt #1 (used as key of map<int,_SummonObject> in importSummonObject)
    char           isEnemy;       // +0x04  "enemy"/"friend" (byte store in importSummonObject)
    unsigned char  unk_05[3];     // +0x05  placeholder: 3 zero bytes between isEnemy and type (no evidence)
    int            type;          // +0x08  script "named"->1, "boss"->2, else 0
    int            field_0C;      // +0x0c  placeholder: script ScanInt, no naming evidence
    short          field_10;      // +0x10  placeholder: script ScanInt stored 16-bit
    unsigned char  unk_12[2];     // +0x12  placeholder: 2 zero bytes after field_10 (no evidence)
    int            field_14;      // +0x14  placeholder (likely altar spawn position part)
    int            field_18;      // +0x18  placeholder
    int            field_1C;      // +0x1c  placeholder
    int            field_20;      // +0x20  placeholder
    _SummonObject() { std::memset(this, 0, 0x24); }
} __attribute__((packed));
static_assert(sizeof(_SummonObject) == 0x24, "advancealtar::_SummonObject size");

// 8-byte stage summary { stageIndex, stageDifficulty }.
// ctor @0x8134452: memset(this,0,8).
struct _Stage {
    int stageIndex;      // +0x00  ProcStage::getStageIndex()
    int stageDifficulty; // +0x04  ProcStage::getStageDifficulty()
    _Stage() { std::memset(this, 0, 8); }
} __attribute__((packed));
static_assert(sizeof(_Stage) == 8, "advancealtar::_Stage size");

} // namespace advancealtar

struct NOTIPacketStruct {

    // header 0x20b, payload 0x28
    struct _NotiAdvanceAltarSpawnUnit {
        unsigned short         spawnIndex; // +0x00  u16 copied from _SummonObjectMs+4
                                           //        (= per-phase summon sequence counter, ProcStage+0x14);
                                           //        semantic name inferred, offset/width evidenced
        unsigned char          pad[2];     // +0x02  padding (zeroed)
        advancealtar::_SummonObject summon; // +0x04  36 bytes memcpy'd from _SummonObjectMs+0x14
        _NotiAdvanceAltarSpawnUnit() {
            // mirrors @0x81346a6: sub-object ctor at +4 runs first, then memset(0x28)
            std::memset(this, 0, 0x28);
        }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarSpawnUnit) == 0x28,
                  "NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit size");

    // header 0x209, payload 8
    struct _NotiAdvanceAltarStageInfo {
        int stageIndex;      // +0x00  ProcStage::getStageIndex()
        int stageDifficulty; // +0x04  ProcStage::getStageDifficulty()
        _NotiAdvanceAltarStageInfo() { std::memset(this, 0, 8); }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarStageInfo) == 8,
                  "NOTIPacketStruct::_NotiAdvanceAltarStageInfo size");

    // header 0x20a, payload 0x14
    struct _NotiAdvanceAltarMapInfo {
        int stageType;               // +0x00  ProcStage::getStageType()
        int currentSurvivalRoundNo;  // +0x04  ProcStage::getCurrentSurvivalRoundNo()
        int mapIndex;                // +0x08  CMap::get_index()
        int currentPhaseIndex;       // +0x0c  TimeLine+0xc = TimeLineSummary[0];
                                     //        offset evidenced, name inferred from TimeLine layout
        int mapBasisLevel;           // +0x10  ProcStage::getMapBasisLevel()
        _NotiAdvanceAltarMapInfo() { std::memset(this, 0, 0x14); }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarMapInfo) == 0x14,
                  "NOTIPacketStruct::_NotiAdvanceAltarMapInfo size");

    // header 0x20f, payload 0x1c
    struct _NotiAdvanceAltarClearStage {
        int                    stageType;             // +0x00  ProcStage::getStageType()
        int                    clearSurvivalRoundNo;  // +0x04  ProcStage::getClearSurvivalRoundNo()
        int                    ridableId;             // +0x08  AdvanceAltarData+0x18 (u16, sign-extended)
        char                   clearFlag;             // +0x0c  1 byte (bool)
        advancealtar::_Stage   stage;                 // +0x0d  8 bytes { stageIndex, stageDifficulty }
        unsigned char          pad[3];                // +0x15  padding (zeroed)
        int                    starCount;             // +0x18  int arg of sendClearStage
        _NotiAdvanceAltarClearStage() {
            // mirrors @0x8134720: sub-object ctor at +0xd runs first, then memset(0x1c)
            std::memset(this, 0, 0x1c);
        }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarClearStage) == 0x1c,
                  "NOTIPacketStruct::_NotiAdvanceAltarClearStage size");

    // header 0x20e, payload 8
    struct _NotiAdvanceAltarEplpState {
        int            eplpType;         // +0x00  StageEndType::T (sendEplp param)
        char           isAvailableTicket;// +0x04  CharacAdvanceAltarManager::isAvailableTciket()
        unsigned char  pad[3];           // +0x05  padding (zeroed by memset(8))
        _NotiAdvanceAltarEplpState() { std::memset(this, 0, 8); }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarEplpState) == 8,
                  "NOTIPacketStruct::_NotiAdvanceAltarEplpState size");

    // header 0x210, payload 1 (marker; byte is never written in the sender)
    struct _NotiAdvanceAltarUpdateRidable {
        unsigned char flag; // +0x00  always 0 in Dispatcher_ResetStar::process_packet
        _NotiAdvanceAltarUpdateRidable() { std::memset(this, 0, 1); }
    } __attribute__((packed));
    static_assert(sizeof(_NotiAdvanceAltarUpdateRidable) == 1,
                  "NOTIPacketStruct::_NotiAdvanceAltarUpdateRidable size");
};

#endif // NOTIPACKETSTRUCT_H
