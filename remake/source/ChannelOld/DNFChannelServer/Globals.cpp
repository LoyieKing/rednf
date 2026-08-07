#include "Globals.h"

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
