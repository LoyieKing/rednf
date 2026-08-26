#ifndef GAME_DB_UPDATEINGAMEEVENTHISTORY_H_
#define GAME_DB_UPDATEINGAMEEVENTHISTORY_H_

// ORIG 0x08444ac4
// _ZN27DB_UpdateInGameEventHistory11makeRequestEjiii
class DB_UpdateInGameEventHistory
{
public:
    static void makeRequest(unsigned int accountId, int eventType,
                            int history1, int history2);
};

#endif  // GAME_DB_UPDATEINGAMEEVENTHISTORY_H_
