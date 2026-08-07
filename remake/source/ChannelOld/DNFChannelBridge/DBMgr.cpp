#include "DBMgr.h"
#include "ScriptData.h"
#include "ChannelServiceApp.h"
#include <stdio.h>
#include <unistd.h>

int DBMgr::Mysql_logon()
{
    h_db = mysql_init(NULL);
    printf("DB ip='%s'\n", G_ScriptData()->db_ip);
    printf("DB id='%s'\n", G_ScriptData()->db_id);
    printf("DB pw='%s'\n", G_ScriptData()->db_pwd);
    printf("DB name='%s'\n", G_ScriptData()->db_name);
    puts("Try Mysql Login~~~~");
    if (mysql_real_connect(h_db, G_ScriptData()->db_ip, G_ScriptData()->db_id,
                           G_ScriptData()->db_pwd, G_ScriptData()->db_name, 0xcea, 0, 0) == 0)
    {
        Mysql_error();
        puts("mysql connect error");
        return false;
    }
    puts("mysql connect success");
    return true;
}

int DBMgr::Mysql_relogon()
{
    mysql_close(h_db);
    sleep(1);
    h_db = mysql_init(NULL);
    printf("DB ip='%s'\n", G_ScriptData()->db_ip);
    printf("DB id='%s'\n", G_ScriptData()->db_id);
    printf("DB pw='%s'\n", G_ScriptData()->db_pwd);
    printf("DB name='%s'\n", G_ScriptData()->db_name);
    puts("Try Mysql Re-Login~~~~");
    if (mysql_real_connect(h_db, G_ScriptData()->db_ip, G_ScriptData()->db_id,
                           G_ScriptData()->db_pwd, G_ScriptData()->db_name, 0xcea, 0, 0) == 0)
    {
        Mysql_error();
        puts("mysql connect error");
        return false;
    }
    else
    {
        sleep(1);
        ChannelServiceApp::gFileLogError.Lock();
        ChannelServiceApp::gFileLogError << "MySql Re-Logon Success" << endl;
        ChannelServiceApp::gFileLogError.Unlock();
        return true;
    }
}

void DBMgr::Mysql_logoff()
{
    mysql_close(h_db);
}

void DBMgr::Mysql_error()
{
    int mysqlerrno = mysql_errno(h_db);
    ChannelServiceApp::gFileLogError.Lock();
    ChannelServiceApp::gFileLogError << "MySql Error = " << mysqlerrno << endl;
    ChannelServiceApp::gFileLogError.Unlock();
    if ((mysqlerrno == 0x7dd) || (mysqlerrno == 0x7d3) || (mysqlerrno == 0x7d6) ||
        (mysqlerrno == 0x7de) || (mysqlerrno == 0x480))
    {
        Mysql_logoff();
        Mysql_logon();
    }
}

MYSQL_RES* DBMgr::Mysql_query(char* query)
{
    if (mysql_query(h_db, query) != 0)
    {
        Mysql_error();
        return NULL;
    }
    MYSQL_RES* res = mysql_store_result(h_db);
    if (res == NULL)
    {
        Mysql_error();
        res = NULL;
    }
    return res;
}

MYSQL_ROW* DBMgr::Mysql_fetch(MYSQL_ROW& row, MYSQL_RES* res)
{
    row = mysql_fetch_row(res);
    return &row;
}

void DBMgr::Mysql_free(MYSQL_RES* res)
{
    mysql_free_result(res);
}
