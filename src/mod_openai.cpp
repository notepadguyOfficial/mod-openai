#include "mod_openai.h"
#include <curl/curl.h>
#include <json/json.h>

std::vector<ChatCommand> OpenAICommandScript::GetCommands()
{
    static std::vector<ChatCommand> commandTable =
    {
        { "ask", SEC_PLAYER, false, &HandleAskAICommand, "" }
    };

    return commandTable;
}

static bool OpenAICommandScript::HandleAskAICommand(ChatHandler* handler, const char* args)
{
    if (sConfigMgr->GetOption<bool>("OpenAI.Enabled", false))
        return false;

    if (!*args)
        return false;

    std::string prompt = "Provide a guide or answer for World of Warcraft WotLK 3.3.5a: ";
    prompt += args;

    std::string response = MakeOpenAIRequest(prompt);

    std::istringstream jsonStream(response);
    boost::property_tree::ptree jsonResponse;
    boost::property_tree::read_json(jsonStream, jsonResponse);

    if (auto choice = jsonResponse.get_child_optional("choices"))
    {
        std::string aiResponse = choice->front().second.get<std::string>("message.content", "");
        handler->PSendSysMessage("AI Response: %s", aiResponse.c_str());
    }
    else
    {
        handler->PSendSysMessage("Failed to get a valid response from OpenAI.");
    }

    return true;
}

std::string OpenAICommandScript::MakeOpenAIRequest(const std::string& prompt)
{
    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve("api.openai.com", "https");
        stream.connect(results);

        std::ostringstream requestBody;
        requestBody << "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"" << prompt << "\"}]}";

        http::request<http::string_body> req{http::verb::post, "/v1/chat/completions", 11};
        req.set(http::field::host, "api.openai.com");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.set("Authorization", "Bearer " + OPENAI_API_KEY);
        req.body() = requestBody.str();
        req.prepare_payload();

        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);
        return res.body();
    }
    catch (const std::exception& e)
    {
        return std::string("Error: ") + e.what();
    }
}
