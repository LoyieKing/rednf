// df_game_r：Gm_List 独立编译单元。

namespace
{
class GmListSync
{
public:
    static void send(int serverGroup)
    {
        volatile int group = serverGroup;
        (void)group;
    }
};
}

class Gm_List
{
public:
    void SendGMList_To_UpperServer(int serverGroup);
};

void Gm_List::SendGMList_To_UpperServer(int serverGroup)
{
    GmListSync::send(serverGroup);
}
