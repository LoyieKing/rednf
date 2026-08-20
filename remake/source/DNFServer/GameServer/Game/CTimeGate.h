#ifndef DNF_GAME_CTIMEGATE_H_
#define DNF_GAME_CTIMEGATE_H_

class CTimeGate
{
public:
    CTimeGate();
    void SetTimeGate(int a, int b, int c, int d, int e);
    bool setBlindTimeGateStateQuest(int questIdx);
    void SetLevel(int level) { m_openState = (char)level; }
    char isOpen() const;
    int getNpcIndex() const { return m_npcIndex; }
    int getQuestIndex() const { return m_questIndex; }
    int getOpenedQuestIndex() const { return m_openedQuestIndex; }
private:
    char m_pad0[4];
    char m_openState;
    char m_pad1[3];
    int m_field8;
    int m_fieldc;
    int m_npcIndex;
    int m_questIndex;
    int m_openedQuestIndex;
};

#endif
