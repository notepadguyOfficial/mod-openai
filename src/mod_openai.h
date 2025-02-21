#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"
#include "Log.h"
#include "Chat.h"

using namespace Acore::ChatCommands;

class OpenAICommandScript : public CommandScript
{
public:
    std::string OPENAI_API_URL;
    std::string OPENAI_API_KEY;

    OpenAICommandScript() : CommandScript("OpenAICommandScript")
    {
        if(sConfigMgr->GetOption<bool>("OpenAI.Enable", true))
        {
            LOG_DEBUG("Server", "OpenAI is enabled.");
            OPENAI_API_URL = "https://api.openai.com/v1/chat/completions";
            OPENAI_API_KEY = sConfigMgr->GetStringDefault("AskOpenAI.APIKey", "");

            if(!OPENAI_API_KEY)
                LOG_ERROR("Server", "OpenAI API key is not set. Please set it in the configuration file.");
        }
        else
            LOG_DEBUG("Server", "OpenAI is disabled.");
    }

    ChatCommandTable GetCommands() const override;

private:
    std::string MakeOpenAIRequest(const std::string& prompt);
    static bool HandleAskAICommand(ChatHandler* handler, const char* args);
};

void AddSC_OpenAICommandScript()
{
    new OpenAICommandScript();
}
