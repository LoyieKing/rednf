#include "Globals.h"
#include "ScriptData.h"
#include "GlobalInstance.h"

// ORIG 的 GlobalInstance<ScriptData>::create 实例化来自 ctor 不可见(仅声明)的 TU
// (DNFChannelServer.cpp)，保留 new 表达式的 EH cleanup(_ZdlPv) 路径。
// 若在 ScriptData.cpp 同 TU 实例化，GCC -O0 可证 ctor 不抛而省略该路径，
// 导致机器码与 ORIG 不一致；Globals.o 在链接序中先于 ScriptData.o，故此处
// 显式实例化可确保链接选中带 cleanup 的版本。
template class GlobalInstance<ScriptData>;

TGlobalInstance<TextOutputDevice_FILE> ChannelServiceApp::g_FileLogInfo;
TDebugTrace<char> ChannelServiceApp::gFileLogInfo;
TGlobalInstance<TextOutputDevice_FILE> ChannelServiceApp::g_FileLogWarn;
TDebugTrace<char> ChannelServiceApp::gFileLogWarn;
TGlobalInstance<TextOutputDevice_FILE> ChannelServiceApp::g_FileLogError;
TDebugTrace<char> ChannelServiceApp::gFileLogError;
TGlobalInstance<TextOutputDevice_FILE> ChannelServiceApp::g_FileLogCri;
TDebugTrace<char> ChannelServiceApp::gFileLogCri;
TGlobalInstance<TextOutputDevice_stdout> ChannelServiceApp::g_LogInfo;
TDebugTrace<char> ChannelServiceApp::gLogInfo;
TGlobalInstance<TextOutputDevice_stdout> ChannelServiceApp::g_LogCri;
TDebugTrace<char> ChannelServiceApp::gLogCri;
TGlobalInstance<TextOutputDevice_stdout> ChannelServiceApp::g_LogWarn;
TDebugTrace<char> ChannelServiceApp::gLogWarn;
TGlobalInstance<TextOutputDevice_stdout> ChannelServiceApp::g_LogError;
TDebugTrace<char> ChannelServiceApp::gLogError;

void TextOutputDevice_FILE::close()
{
    if (fp_ != NULL)
    {
        fclose(fp_);
        fp_ = NULL;
    }
}

TextOutputDevice_FILE::~TextOutputDevice_FILE()
{
    close();
}
