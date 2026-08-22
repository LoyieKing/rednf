#ifndef _CGM_MANAGER_H_
#define _CGM_MANAGER_H_

class CGM_Manager
{
public:
    CGM_Manager();
    ~CGM_Manager();

    bool IsGm(unsigned int accId);
    void TurnGmMode(unsigned int accId);
    char GetCurrentGmMode(unsigned int accId);

private:
    char m_pad[0x3c];
};

#endif // _CGM_MANAGER_H_
