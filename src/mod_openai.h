#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"
#include "Chat.h"

class OpenAICommandScript : public CommandScript
{
public:
    std::string OPENAI_API_URL;
    std::string OPENAI_API_KEY;

    OpenAICommandScript() : CommandScript("OpenAICommandScript")
    {
        OPENAI_API_URL = "https://api.openai.com/v1/chat/completions";
        OPENAI_API_KEY = sConfigMgr->GetStringDefault("AskOpenAI.APIKey", "");
    }

    std::vector<ChatCommand> GetCommands() const override;

private:
    std::string MakeOpenAIRequest(const std::string& prompt);
    static bool HandleAskAICommand(ChatHandler* handler, const char* args);
};

void AddSC_OpenAICommandScript()
{
    new OpenAICommandScript();
}
