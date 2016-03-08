#include "../output/IOMananger.hpp"

//Start screen
void Output::makeMap(){
    Screen *main_messages = (Screen*) new MessageBox("main_messages", 0, 0, 35, 80, io::configs.windows_lines);
    screen.insert(std::pair<string,Screen*>("main_messages",main_messages) );
    Screen *progress_bar = (Screen*) new ProgressBar(   "progress_bar",
                                                        0, 80, 5, 60,
                                                        ProgressBar::HORIZONTAL, 0, 100,
                                                        io::configs.windows_lines);
    screen.insert(std::pair<string,Screen*>("progress_bar",progress_bar));

}

//make configs
void Configs::decodeMap(){
    //start configs

    if(json["num_actions_routers"] != Json::nullValue)
        this->num_actions_routers = json["num_actions_routers"].asInt();
    else
        this->num_actions_routers = 1;

    if(json["num_http_routers"] != Json::nullValue)
        this->num_http_routers = json["num_http_routers"].asInt();
    else
        this->num_http_routers = 10;

    //server
    if(json["server_interface"] != Json::nullValue)
        this->server_interface = json["server_interface"].asString();
    else
        this->server_interface = "wlp8s0";

    //udp server
    if(json["udp_port"] != Json::nullValue)
        this->udp_port = json["udp_port"].asInt();
    else
        this->udp_port = 6000;

    if(json["num_try_udp_get_connection"] != Json::nullValue)
        this->num_try_udp_get_connection = json["num_try_udp_get_connection"].asInt();
    else
        this->num_try_udp_get_connection = 5;

    if(json["force_udp_connection"] != Json::nullValue)
        this->force_udp_connection = json["force_udp_connection"].asBool();
    else
        this->force_udp_connection = true;

    //tcp server
    if(json["tcp_port"] != Json::nullValue)
        this->tcp_port = json["tcp_port"].asInt();
    else
        this->tcp_port = 10000;

    //tcp server
    if(json["high_ip"] != Json::nullValue)
        this->high_ip = json["high_ap"].asString();
    else
        this->high_ip = "10.190.60.25";

    //database
    if(json["database_name"] != Json::nullValue)
        this->database_name = json["database_name"].asString();
    else
        this->database_name= "./resources/monkey.db";

}

//Logger make Map
void Logger::makeMap(){

}
