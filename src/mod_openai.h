#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"
#include "Chat.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

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
