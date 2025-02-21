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
    if(sConfigMgr->GetOption<bool>("OpenAI.Enable", false))
        return false;

    if (!*args)
        return false;

    std::string prompt = "Provide a guide or answer for World of Warcraft WotLK 3.3.5a: ";
    prompt += args;

    std::string response = MakeOpenAIRequest(prompt);

    Json::Value jsonResponse;
    Json::Reader reader;
    if (reader.parse(response, jsonResponse) && jsonResponse.isMember("choices"))
    {
        std::string aiResponse = jsonResponse["choices"][0]["message"]["content"].asString();
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
    CURL* curl = curl_easy_init();
    if (!curl) return "Failed to initialize CURL.";

    std::string response;

    // Set up the request
    curl_easy_setopt(curl, CURLOPT_URL, OPENAI_API_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Set headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + OPENAI_API_KEY).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string requestBody = R"({"model": "gpt-4o-mini", "messages": [{"role": "user", "content": ")" + prompt + R"("}]})";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, std::string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = "Failed to communicate with OpenAI API.";
        LOGS_ERROR("Server", "Failed to communicate with OpenAI API. Error: %s", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return response;
}
