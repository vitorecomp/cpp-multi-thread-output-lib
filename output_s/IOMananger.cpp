#include "../../headers/output/IOMananger.hpp"

///////////////////////////////////////////////////////////////////////////////
//Configs
//
///////////////////////////////////////////////////////////////////////////////
Configs io::configs;


Configs::Configs(){
    this->ended = false;
    this->ok = true;
}

bool Configs::configOk(){
    return !ok;
}

void Configs::openFile(const char *name){
    string line;
    stringstream ss;
    //open the text
    ifstream myfile;
    myfile.open(name, std::ifstream::in);

    //get the string
    if (myfile.is_open()){
        while(getline (myfile,line))
            ss << line << '\n';
        myfile.close();
    }else{
        throw std::invalid_argument("Json File invalid");
    }

    //validate the input
    string json = ss.str();
    if(json.length() == 0)
        throw std::invalid_argument("Json File invalid");


    //parse the string
    Json::Value value;
    Json::Reader reader;

    bool parsed = reader.parse(json, value);
    if(!parsed)
        throw std::invalid_argument("Json File invalid");

    //configure from json
    this->json = value;
}

void Configs::setConfigs(){
    if(json["output"] != Json::nullValue)
        this->output = json["output"].asBool();
    else
        this->output = true;


    if(json["windows_cols"] != Json::nullValue)
        this->windows_cols = json["windows_cols"].asInt();
    else
        this->windows_cols = 40;

    if(json["windows_lines"] != Json::nullValue)
        this->windows_lines = json["windows_lines"].asInt();
    else
        this->windows_lines = 40;

    decodeMap();
    this->endSignal();
}



void Configs::endSignal(){
    std::unique_lock<std::mutex> lck(m_ended);
    ended = true;
    s_ended.notify_all();
}

void Configs::waitEnd(){
    std::unique_lock<std::mutex> lck(m_ended);
    while (!ended)
        s_ended.wait(lck);
}

///////////////////////////////////////////////////////////////////////////////
//Output
//
///////////////////////////////////////////////////////////////////////////////
Output io::output;

void run_io(){
    io::configs.waitEnd();
    bool empty = false;
    io::output.start();
    io::logger.start();
    io::input.start();
	while(true) {
        empty = io::output.run();
        empty |= io::input.run();
        empty |= io::logger.run();

        if(io::input.isEnded() && empty)
            break;
	}
}

Output::Output(){
    this->ended = false;
}


Output::~Output(){
    for (auto& x: screen) {
        delete x.second;
    }
}

void Output::setSize(uint ncols, uint nlines){
    stringstream ss;
    ss << "echo -ne " << "'" << "\e[8;" << nlines << ";" << ncols << "t" << "'";
    system(ss.str().c_str());
}

void Output::start(){
    Output::setSize(io::configs.windows_cols, io::configs.windows_lines);
    Figure::clearAll();
    makeMap();
    this->endSignal();
}

void Output::endSignal(){
    std::unique_lock<std::mutex> lck(m_ended);
    ended = true;
    s_ended.notify_all();
}

void Output::waitEnd(){
    std::unique_lock<std::mutex> lck(m_ended);
    while (!ended)
        s_ended.wait(lck);
}

bool Output::run(){
    queue_mutex.lock();

    if(print_queue.empty()){
        queue_mutex.unlock();
        return true;
    }

    Message message = print_queue.front();
    print_queue.pop();

    queue_mutex.unlock();

    if(screen.find(message.box) != screen.end()){
        this->screen[message.box]->print(message);
    }else{
        stringstream ss;
        ss << "Mapa de screen não encontrado: " << message.box;
        io::logger.log(module_name, Logger::ERROR, ss.str());
    }
    return false;
}

void Output::printMsgBox(string box, string msg){
    queue_mutex.lock();
    Message message(0, 0, box, msg);
    this->print_queue.push(message);
    queue_mutex.unlock();
}

void Output::printBarGraph(string box, double value){
    stringstream msg;
    msg << value;
    queue_mutex.lock();
    Message message(0, 0, box, msg.str());
    this->print_queue.push(message);
    queue_mutex.unlock();
}

void Output::printValues(string box, map<string, string>){
    /*
    queue_mutex.lock();
    Message message(0, 0, box, msg);
    this->print_queue.push(message);
    queue_mutex.unlock();
    */
}



///////////////////////////////////////////////////////////////////////////////
//Input
//
///////////////////////////////////////////////////////////////////////////////
Input io::input;

Input::Input(){
    ended = false;
}

void Input::endSignal(){
    end_mutex.lock();
    ended = true;
    end_mutex.unlock();
}

void Input::waitEnter(){}

bool Input::run(){
    return true;
}

bool Input::isEnded(){
    end_mutex.lock();
    bool ended = this->ended;
    end_mutex.unlock();
    return ended;
}

///////////////////////////////////////////////////////////////////////////////
//Input
//
///////////////////////////////////////////////////////////////////////////////
Logger io::logger;


Logger::Logger(){
    this->ended = false;
}


Logger::~Logger(){
    for (auto& x: screen) {
        delete x.second;
    }
}

void Logger::start(){
    Output::setSize(io::configs.windows_cols, io::configs.windows_lines);
    Figure::clearAll();
    makeMap();
    this->endSignal();
}

void Logger::endSignal(){
    std::unique_lock<std::mutex> lck(m_ended);
    ended = true;
    s_ended.notify_all();
}

void Logger::waitEnd(){
    std::unique_lock<std::mutex> lck(m_ended);
    while (!ended)
        s_ended.wait(lck);
}

bool Logger::run(){
    queue_mutex.lock();

    if(print_queue.empty()){
        queue_mutex.unlock();
        return true;
    }

    Message message = print_queue.front();
    print_queue.pop();

    queue_mutex.unlock();

    if(screen.find(message.box) != screen.end()){
        this->screen[message.box]->print(message);
    }else{
        stringstream ss;
        ss << "Mapa de screen não encontrado: " << message.box;
        io::logger.log(module_name, Logger::ERROR, ss.str());
    }
    return false;
}

void Logger::log(string box, Type, string msg){
    queue_mutex.lock();
    //this->print_queue.push(message);
    queue_mutex.unlock();
}
