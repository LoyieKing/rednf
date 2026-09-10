#ifndef GAME_CEVENTCHARACTERHANDLER_H_
#define GAME_CEVENTCHARACTERHANDLER_H_

class CUser;

class CEventCharacterHandler
{
public:
    CEventCharacterHandler();

    bool _makeEventCharacter(CUser* user, int level);
    void _UpdateEventCharacInfo(CUser* user);
    bool _checkMakeCommand(CUser* user, int level, int& cmdIdx);
    void _SetClearedQuest(CUser* user);
    bool _MasterNewSkill(CUser* user, int skillIdx, int count);
    // [Q6HeaderTypeAxis 2026-09-11 类型轴] ORIG prett = `bool CEventCharacterHandler::
    //   _AddItem(CUser*, itemIndexOnlyServer_t, int, int)`（我方实测串 0x4b vs ORIG 0x4f）；
    //   itemIndexOnlyServer_t = unsigned long（GameTypes.h 已备），mangled/ABI 零变化。
    bool _AddItem(CUser* user, itemIndexOnlyServer_t itemIdx, int count, int reason);
    bool _SetLevel(CUser* user, int level);
    bool _SetGrowType(CUser* user, int growTypeKind, int growType);
};

CEventCharacterHandler* CEventCharacterHandlerInstance();

#endif
