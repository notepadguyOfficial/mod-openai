#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"
#include "Chat.h"
#include "Log.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace json = boost::json;

using namespace Acore::ChatCommands;

class OpenAICommandScript : public CommandScript
{
public:
    std::string OPENAI_API_URL;
    std::string OPENAI_API_KEY;

    OpenAICommandScript() : CommandScript("OpenAICommandScript")
    {
        if (sConfigMgr->GetOption<bool>("OpenAI.Enabled", false))
        {
            OPENAI_API_URL = "https://api.openai.com/v1/chat/completions";
            OPENAI_API_KEY = sConfigMgr->GetOption<std::string>("AskOpenAI.APIKey", "");
        }
    }

    ChatCommandTable GetCommands() const override;

private:
    static std::string MakeOpenAIRequest(const std::string& prompt);
    static bool HandleAskAICommand(ChatHandler* handler, const char* args);
};

void AddSC_OpenAICommandScript()
{
    new OpenAICommandScript();
}
