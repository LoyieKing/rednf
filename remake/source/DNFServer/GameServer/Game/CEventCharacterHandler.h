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
    bool _AddItem(CUser* user, unsigned long itemIdx, int count, int reason);
    bool _SetLevel(CUser* user, int level);
    bool _SetGrowType(CUser* user, int growTypeKind, int growType);
};

CEventCharacterHandler* CEventCharacterHandlerInstance();

#endif
