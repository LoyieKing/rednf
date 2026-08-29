// ============================================================================
// NOTIPacketStruct 独占头（ORIG 12 符号：6 类 × C1/C2 弱符号）
// ----------------------------------------------------------------------------
// ORIG 符号（nm -S --defined-only df_game_r）：
//   _ZN16NOTIPacketStruct24_NotiAdvanceAltarMapInfoC1/C2Ev      @0x081346fc (0x23)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarEplpStateC1/C2Ev    @0x08134752 (0x23)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarSpawnUnitC1/C2Ev    @0x081346a6 (0x31)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarStageInfoC1/C2Ev    @0x081346d8 (0x23)
//   _ZN16NOTIPacketStruct27_NotiAdvanceAltarClearStageC1/C2Ev   @0x08134720 (0x31)
//   _ZN16NOTIPacketStruct30_NotiAdvanceAltarUpdateRidableC1/C2Ev @0x08141ff8 (0x23)
// ctor 定义（C1/C2 一对）见 Game/NOTIPacketStructCtors.cpp；memset 常量为
// ORIG ctor 反汇编字面量（0x28/8/0x14/0x1c/8/1，见各字段注释）。
// 本头自 advancealtar.h 死草稿吸收定义迁出（Main 裁决 2026-08-25 吸收、
// OrigNOTIPkt 批独占迁出）；布局/字段/static_assert 原样保留。
// ============================================================================
#pragma once

struct NOTIPacketStruct
{
    // header 0x20b, payload 0x28
    struct _NotiAdvanceAltarSpawnUnit
    {
        unsigned short m_spawnIndex;      // +0x00（u16，= _SummonObjectMs+4 序号）
        unsigned char m_pad[2];           // +0x02
        unsigned char m_summon[0x24];     // +0x04 = _SummonObject（CDataManager.h 权威，
                                          //   ORIG 为子对象 ctor 后 memcpy 整体）
        _NotiAdvanceAltarSpawnUnit();     // memset(this, 0, 0x28)
    } __attribute__((packed));
    typedef char _assert_spawnunit_size[(sizeof(_NotiAdvanceAltarSpawnUnit) == 0x28) ? 1 : -1];

    // header 0x209, payload 8
    struct _NotiAdvanceAltarStageInfo
    {
        int m_stageIndex;       // +0x00 ProcStage::getStageIndex()
        int m_stageDifficulty;  // +0x04 ProcStage::getStageDifficulty()
        _NotiAdvanceAltarStageInfo();  // memset(this, 0, 8)
    } __attribute__((packed));
    typedef char _assert_stageinfo_size[(sizeof(_NotiAdvanceAltarStageInfo) == 8) ? 1 : -1];

    // header 0x20a, payload 0x14
    struct _NotiAdvanceAltarMapInfo
    {
        int m_stageType;               // +0x00 ProcStage::getStageType()
        int m_currentSurvivalRoundNo;  // +0x04 getCurrentSurvivalRoundNo()
        int m_mapIndex;                // +0x08 CMap::get_index()
        int m_currentPhaseIndex;       // +0x0c TimeLineSummary 首字段（TimeLine+0xc）
        int m_mapBasisLevel;           // +0x10 getMapBasisLevel()
        _NotiAdvanceAltarMapInfo();    // memset(this, 0, 0x14)
    } __attribute__((packed));
    typedef char _assert_mapinfo_size[(sizeof(_NotiAdvanceAltarMapInfo) == 0x14) ? 1 : -1];

    // header 0x20f, payload 0x1c
    // 注意：_Stage 现为带 ctor 的非 POD，packed 内嵌会被 GCC 忽略 packed 警告并
    // 引发填充 → 此处用 8B 字节槽保持 ORIG 扁平布局（_SummonObject 同先例）。
    struct _NotiAdvanceAltarClearStage
    {
        int m_stageType;              // +0x00
        int m_clearSurvivalRoundNo;   // +0x04
        int m_ridableId;              // +0x08（u16 符号扩展）
        char m_clearFlag;             // +0x0c
        unsigned char m_stage[8];     // +0x0d = _Stage（8B {stageIndex, stageDifficulty}）
        unsigned char m_pad[3];       // +0x15
        int m_starCount;              // +0x18
        _NotiAdvanceAltarClearStage();  // memset(this, 0, 0x1c)
    } __attribute__((packed));
    typedef char _assert_clearstage_size[(sizeof(_NotiAdvanceAltarClearStage) == 0x1c) ? 1 : -1];

    // header 0x20e, payload 8
    struct _NotiAdvanceAltarEplpState
    {
        int m_eplpType;                // +0x00 StageEndType::T
        char m_isAvailableTicket;      // +0x04 isAvailableTciket()
        unsigned char m_pad[3];        // +0x05
        _NotiAdvanceAltarEplpState();  // memset(this, 0, 8)
    } __attribute__((packed));
    typedef char _assert_eplpstate_size[(sizeof(_NotiAdvanceAltarEplpState) == 8) ? 1 : -1];

    // header 0x210, payload 1（标记包；发送方从不写字节）
    struct _NotiAdvanceAltarUpdateRidable
    {
        unsigned char m_flag;          // +0x00 恒 0
        _NotiAdvanceAltarUpdateRidable();  // memset(this, 0, 1)
    } __attribute__((packed));
    typedef char _assert_updateridable_size[(sizeof(_NotiAdvanceAltarUpdateRidable) == 1) ? 1 : -1];
};  // struct NOTIPacketStruct
