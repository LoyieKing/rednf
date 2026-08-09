#include "ScriptThread.h"
#include "DBMgr.h"
#include "ScriptData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ChannelServiceApp::ScriptThread::ScriptThread()
    : Thread(), TManager<ChannelService>()
{
}

ChannelServiceApp::ScriptThread::~ScriptThread()
{
}

void ChannelServiceApp::ScriptThread::loop(void* temp)
{
    puts("*** Start up ScriptThread");
    ChannelService* pApp = getManager();
    DBMgr DBMgr_;
    DBMgr_.Mysql_logon();

    char query[1024];
    char sub_query[1024];
    char kind_name[20];
    char dungeon_name[30];
    char dungeon_id[6];
    char server_id[4];
    char tm_id[4];
    char pre_server_id[4];
    char tmp_buf[300];
    MYSQL_ROW row;
    MYSQL_RES* res;
    FILE* fp;
    int count;
    int sub_count;

    memset(query, 0, 0x400);
    snprintf(query, 0x400, "select * from ch_script_version");
    res = DBMgr_.Mysql_query(query);
    row = *DBMgr_.Mysql_fetch(row, res);
    memset(G_ScriptData(), 0, 0x10);
    strcpy(G_ScriptData()->channel_script_version, row[0]);
    DBMgr_.Mysql_free(res);

    memset(query, 0, 0x400);
    snprintf(query, 0x400, "select group_name, group_gc_no from ch_gc_info order by group_gc_no asc");
    res = DBMgr_.Mysql_query(query);
    count = 0;
    while (true)
    {
        row = *DBMgr_.Mysql_fetch(row, res);
        if (row == NULL)
        {
            break;
        }
        pApp->gc_map[row[0]] = atoi(row[1]);
        printf("1.[%s],[%d]\n", row[0], atoi(row[1]));
        count = count + 1;
    }

    while (fp = fopen("./cfg/channel_info.etc", "w+"), fp != NULL)
    {
        memset(query, 0, 0x400);
        snprintf(query, 0x400, "select kind_name, dungeon_name from ch_dungeon_data");
        res = DBMgr_.Mysql_query(query);
        if (res == NULL)
        {
            DBMgr_.Mysql_relogon();
            DBMgr_.Mysql_query(query);
        }
        count = 0;
        while (true)
        {
            row = *DBMgr_.Mysql_fetch(row, res);
            if (row == NULL)
            {
                break;
            }
            strncpy(kind_name, row[0], 0x14);
            strncpy(dungeon_name, row[1], 0x1e);
            fwrite("[dungeon]\n", 1, 10, fp);
            fputs(kind_name, fp);
            fputc(10, fp);
            fputs(dungeon_name, fp);
            fputc(10, fp);
            memset(sub_query, 0, 0x400);
            snprintf(sub_query, 0x400, "select kind_name, dungeon_id from ch_dungeon_list where kind_name='%s'", kind_name);
            MYSQL_RES* sub_res = DBMgr_.Mysql_query(sub_query);
            if (sub_res == NULL)
            {
                DBMgr_.Mysql_relogon();
                DBMgr_.Mysql_query(sub_query);
            }
            sub_count = 0;
            while (true)
            {
                MYSQL_ROW sub_row;
                sub_row = *DBMgr_.Mysql_fetch(sub_row, sub_res);
                if (sub_row == NULL)
                {
                    break;
                }
                snprintf(dungeon_id, 6, "%d", atoi(sub_row[1]));
                fputs(dungeon_id, fp);
                fputc(10, fp);
                sub_count = sub_count + 1;
            }
            DBMgr_.Mysql_free(sub_res);
            fwrite("[/dungeon]\n\n\n", 1, 0xd, fp);
            count = count + 1;
        }
        DBMgr_.Mysql_free(res);

        memset(query, 0, 0x400);
        snprintf(query, 0x400, "select * from ch_server_data where is_use = '1' order by server_id, channel_number");
        res = DBMgr_.Mysql_query(query);
        if (res == NULL)
        {
            DBMgr_.Mysql_relogon();
            DBMgr_.Mysql_query(query);
        }
        count = 0;
        while (true)
        {
            row = *DBMgr_.Mysql_fetch(row, res);
            if (row == NULL)
            {
                break;
            }
            snprintf(server_id, 4, "%d", atoi(row[1]));
            if (count == 0)
            {
                fwrite("[server]\n", 1, 9, fp);
                fputs(server_id, fp);
                fputc(10, fp);
                strncpy(pre_server_id, server_id, 4);
                printf("[%s]\n", server_id);
            }
            strncpy(tm_id, server_id, 4);
            memset(tmp_buf, 0, 300);
            snprintf(tmp_buf, 300,
                     "   %d   %s   %d   `[%s]`   %d   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f   %0.1f",
                     atoi(row[3]), row[4], atoi(row[5]), row[6], atoi(row[7]),
                     atof(row[8]), atof(row[9]), atof(row[10]), atof(row[11]), atof(row[12]),
                     atof(row[13]), atof(row[14]), atof(row[15]), atof(row[16]), atof(row[17]));
            puts(tmp_buf);
            if (strncmp(pre_server_id, tm_id, 4) != 0)
            {
                fwrite("[/server]\n\n\n", 1, 0xc, fp);
                fwrite("[server]\n", 1, 9, fp);
                fputs(server_id, fp);
                fputc(10, fp);
            }
            strncpy(pre_server_id, server_id, 4);
            fputs(tmp_buf, fp);
            fputc(10, fp);
            count = count + 1;
        }
        fwrite("[/server]", 1, 9, fp);
        fflush(fp);
        fclose(fp);
        DBMgr_.Mysql_free(res);

        pApp->ChannelScript::ReloadScript();
        char* cur_version;
        while (true)
        {
            memset(query, 0, 0x400);
            snprintf(query, 0x400, "select * from ch_script_version");
            res = DBMgr_.Mysql_query(query);
            if (res == NULL)
            {
                DBMgr_.Mysql_relogon();
                DBMgr_.Mysql_query(query);
            }
            row = *DBMgr_.Mysql_fetch(row, res);
            cur_version = row[0];
            if (strcmp(G_ScriptData()->channel_script_version, cur_version) != 0)
            {
                break;
            }
            DBMgr_.Mysql_free(res);
            usleep(4000000);
        }
        GLOG(gFileLogInfo, "Script Reload : cur=" << cur_version
            << ", prev=" << G_ScriptData()->channel_script_version);
        memset(G_ScriptData(), 0, 0x10);
        strcpy(G_ScriptData()->channel_script_version, cur_version);
        DBMgr_.Mysql_free(res);
    }
    puts("[ERROR] cann't open channel_into.etc");
    exit(1);
}
