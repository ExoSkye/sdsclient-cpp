#include <string>
#include <json/json.h>
#include <filesystem>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <fstream>
#include <httplib.h>
#include <base64.h>

enum class Transitions {
    None,
    Fade,
    Slide
};

struct TimePoint {
    SDL_Surface* surf = nullptr;
    Transitions transition = Transitions::None;
};

void doFade(SDL_Surface *pSurface, SDL_Window *pWindow);

void doSlide(SDL_Surface *pSurface, SDL_Window *pWindow);

std::array<TimePoint, 1440> TimePoints;
std::map<std::string, SDL_Surface*> surfaces;

void getNewConfig(const std::string &username, const std::string &password, const std::string &ip, const int port, const std::string& name) {
    httplib::Client client(ip, port);

    client.set_digest_auth(username.c_str(), password.c_str());

    auto res = client.Get("/digest/getConfig");

    Json::Value root_config;
    Json::CharReaderBuilder builder;
    std::string errors;
    std::stringstream stream(res->body);
    bool parsingSuccessful = Json::parseFromStream(builder, stream, &root_config, &errors);

    if (!parsingSuccessful) {
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse configuration\n" << errors << std::endl;
    }

    std::vector<std::string> files_to_extract;

    for (const auto &rule: root_config["result"]["rules"]) {
        for (const auto &rulepart: rule) {
            if (rulepart["type"] == "Display") {
                files_to_extract.push_back(rulepart["args"][0].asString());
            }
        }
    }

    for (const auto &to_extract: files_to_extract) {
        std::cout << "Getting " << to_extract << std::endl;
        std::ofstream resource(to_extract, std::ifstream::out);
        resource << base64_decode(root_config["result"]["resources"][to_extract].asString(), true);
        surfaces[to_extract] = IMG_Load(("./"+to_extract).c_str());
    }

    for (const auto &rule: root_config["result"]["rules"]) {
        Json::ValueConstIterator img_val = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "Display"; });
        std::string img;
        if (img_val != rule.end()) {
            img = img_val->operator[]("args")[0].asString();
        }
        else {
            continue;
        }

        Json::ValueConstIterator screen = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "On"; });
        if (screen != rule.end()) {
            if (screen->operator[]("args")[0].asString() != name) {
                continue;
            }
        }
        Json::ValueConstIterator between = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "Between"; });
        Json::ValueConstIterator every = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "Every"; });
        Json::ValueConstIterator at = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "At"; });
        Json::ValueConstIterator transition = std::find_if(rule.begin(), rule.end(), [](auto x) { return x["type"] == "Transition"; });
        Transitions transition_enum = Transitions::None;
        if (transition != rule.end()) {
            std::string transition_str = transition->operator[]("args")[0].asString();
            std::transform(transition_str.begin(), transition_str.end(), transition_str.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            if (transition_str == "none") {
                transition_enum = Transitions::None;
            }
            else if(transition_str == "fade") {
                transition_enum = Transitions::Fade;
            }
            else if(transition_str == "slide") {
                transition_enum = Transitions::Slide;
            }
        }

        if (every != rule.end() && between != rule.end()) {
            int every_start = 0;
            if (every->operator[]("args").size() == 2) {
                std::string start_str = every->operator[]("args")[1].asString();
                int start_hours = std::atoi(start_str.substr(0, start_str.find(":")).c_str());
                int start_mins = std::atoi(start_str.substr(start_str.find(":")+1, start_str.length()).c_str());

                every_start = start_hours * 60 + start_mins;
            }

            std::string start_str = between->operator[]("args")[0].asString();
            int start_hours = std::atoi(start_str.substr(0, start_str.find(":")).c_str());
            int start_mins = std::atoi(start_str.substr(start_str.find(":")+1, start_str.length()).c_str());

            int between_start = start_hours * 60 + start_mins;

            std::string end_str = between->operator[]("args")[1].asString();

            int end_hours = std::atoi(end_str.substr(0, end_str.find(":")).c_str());
            int end_mins = std::atoi(end_str.substr(end_str.find(":")+1, end_str.length()).c_str());

            int end = end_hours * 60 + end_mins;

            std::string period = every->operator[]("args")[0].asString();

            int minute_period = 0;

            if (period.substr(period.length()-1,period.length()) == "m") {
                minute_period = std::atoi(period.substr(0,period.length()-1).c_str());
            }
            else if (period.substr(period.length()-1,period.length()) == "h") {
                minute_period = std::atoi(period.substr(0,period.length()-1).c_str())*60;
            }

            std::vector<int> times;

            for (int i = every_start; i < 1440; i+=minute_period) {
                times.push_back(i);
            }
            for (auto& time : times) {
                if (time > between_start && time < end) {
                    TimePoints[time].surf = surfaces[img];
                    TimePoints[time].transition = transition_enum;
                }
            }
        }
        else if (every != rule.end()) {
            int start = 0;
            if (every->operator[]("args").size() == 2) {
                std::string start_str = every->operator[]("args")[1].asString();
                int start_hours = std::atoi(start_str.substr(0, start_str.find(":")).c_str());
                int start_mins = std::atoi(start_str.substr(start_str.find(":")+1, start_str.length()).c_str());

                start = start_hours * 60 + start_mins;
            }

            std::string period = every->operator[]("args")[0].asString();

            int minute_period = 0;

            if (period.substr(period.length()-1,period.length()) == "m") {
                minute_period = std::atoi(period.substr(0,period.length()-1).c_str());
            }
            else if (period.substr(period.length()-1,period.length()) == "h") {
                minute_period = std::atoi(period.substr(0,period.length()-1).c_str())*60;
            }

            for (int i = start; i < 1440; i+=minute_period) {
                TimePoints[i].surf = surfaces[img];
                TimePoints[i].transition = transition_enum;
            }
        }
        else if (between != rule.end()) {
            std::string start_str = between->operator[]("args")[0].asString();
            int start_hours = std::atoi(start_str.substr(0, start_str.find(":")).c_str());
            int start_mins = std::atoi(start_str.substr(start_str.find(":")+1, start_str.length()).c_str());

            int start = start_hours * 60 + start_mins;

            std::string end_str = between->operator[]("args")[1].asString();

            int end_hours = std::atoi(end_str.substr(0, end_str.find(":")).c_str());
            int end_mins = std::atoi(end_str.substr(end_str.find(":")+1, end_str.length()).c_str());

            int end = end_hours * 60 + end_mins;

            for (int i = start; i < end; i++) {
                TimePoints[i].surf = surfaces[img];
                TimePoints[i].transition = transition_enum;
            }
        }
        else if (at != rule.end()) {
            std::string time_str = at->operator[]("args")[0].asString();
            int time_hours = std::atoi(time_str.substr(0, time_str.find(":")).c_str());
            int time_mins = std::atoi(time_str.substr(time_str.find(":")+1, time_str.length()).c_str());

            int time = time_hours * 60 + time_mins;

            TimePoints[time].surf = surfaces[img];
            TimePoints[time].transition = transition_enum;
        }
    }
}

int main() {
    Json::Value root;
    std::ifstream config_doc("config.json", std::ifstream::binary);
    config_doc >> root;

    const int x_size = root["screen"].get("x", 1280).asInt();
    const int y_size = root["screen"].get("y", 720).asInt();

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *wnd = SDL_CreateWindow("SDSClient", 0, 0, x_size, y_size, 0);

    const std::string ip = root["network"].get("ip", "0.0.0.0").asString();
    const int port = root["network"].get("port", 17420).asInt();

    const std::string username = root["auth"].get("user", "a").asString();
    const std::string password = root["auth"].get("pass", "c").asString();

    const std::string name = root["ident"].get("name", "unknown").asString();

    getNewConfig(username, password, ip, port, name);

    // Main loop

    while (true) {
        int cur_minute = time(NULL) / 60 % 1440;

        httplib::Client client(ip, port);

        client.set_digest_auth(username.c_str(), password.c_str());

        auto res = client.Get("/digest/needReload");

        Json::Value need_update;
        Json::CharReaderBuilder builder;
        std::string errors;
        std::stringstream stream(res->body);
        bool parsingSuccessful = Json::parseFromStream(builder, stream, &need_update, &errors);

        if (!parsingSuccessful) {
            // report to the user the failure and their locations in the document.
            std::cout << "Failed to parse configuration\n" << errors << std::endl;
        }

        if (need_update["result"].asBool()) {
            getNewConfig(username, password, ip, port, name);
        }

        TimePoint& curTimePoint = TimePoints[cur_minute];

        if (curTimePoint.surf == nullptr) {
            continue;
        }

        SDL_Surface* surf = curTimePoint.surf;

        if (surf != nullptr) {
            if (curTimePoint.transition == Transitions::None) {
                SDL_Surface* wnd_surf = SDL_GetWindowSurface(wnd);
                SDL_BlitScaled(surf, NULL, wnd_surf, NULL);
                SDL_UpdateWindowSurface(wnd);
                SDL_FreeSurface(wnd_surf);
            }
            else if (curTimePoint.transition == Transitions::Fade) {
                doFade(surf,wnd);
            }
            else if (curTimePoint.transition == Transitions::Slide) {
                doSlide(surf,wnd);
            }
        }

        SDL_Delay(1000);
    }

    SDL_DestroyWindow(wnd);
    SDL_Quit();

    return 0;
}

void doSlide(SDL_Surface *pSurface, SDL_Window *pWindow) {

}

void doFade(SDL_Surface *pSurface, SDL_Window *pWindow) {
    printf("Doing fade");
    SDL_Surface* window_surf = SDL_GetWindowSurface(pWindow);
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface* window_surf_copy = SDL_CreateRGBSurface(0,window_surf->w,window_surf->h,window_surf->format->BitsPerPixel,rmask,gmask,bmask,amask);
    SDL_Surface* window_alpha = SDL_CreateRGBSurface(0,window_surf->w,window_surf->h,window_surf->format->BitsPerPixel,rmask,gmask,bmask,amask);
    SDL_BlitSurface(window_surf,NULL,window_surf_copy,NULL);
    for (int i = 0; i < 256; i++) {
        SDL_SetSurfaceAlphaMod(window_surf_copy,255-i);
        SDL_BlitScaled(pSurface,NULL,window_alpha,NULL);
        SDL_BlitSurface(window_surf_copy,NULL,window_alpha,NULL);
        SDL_BlitSurface(window_alpha,NULL,window_surf,NULL);
        SDL_UpdateWindowSurface(pWindow);
        SDL_Delay(1000/60);
    }
    SDL_BlitScaled(pSurface,NULL,window_surf,NULL);
    SDL_UpdateWindowSurface(pWindow);
    SDL_FreeSurface(window_surf);
    SDL_FreeSurface(window_surf_copy);
}
