#include "../output/IOMananger.hpp"

//Start screen
void Output::makeMap(){
    Screen *main_messages = (Screen*) new MessageBox("main_messages", 0, 0, 20, 80, io::configs.windows_lines);
    screen.insert(std::pair<string,Screen*>("main_messages",main_messages) );
    Screen *progress_bar = (Screen*) new ProgressBar(   "progress_bar",
                                                        0, 80, 5, 60,
                                                        ProgressBar::HORIZONTAL, 0, 100,
                                                        io::configs.windows_lines);
    screen.insert(std::pair<string,Screen*>("progress_bar",progress_bar));

}

//make configs
void Configs::decodeMap(){
    /*
    if(json["start_number"] != Json::nullValue)
        this->startNumber = json["start_number"].asInt();
    else
        this->startNumber = 2;

    if(json["maxNumber"] != Json::nullValue)
        this->maxNumber = json["maxNumber"].asInt();
    else
        this->maxNumber = 2000;

    if(json["maxGenerations"] != Json::nullValue)
        this->maxGenerations = json["maxGenerations"].asInt();
    else
        this->maxGenerations = 100;

    if(json["min_acuracy"] != Json::nullValue)
        this->min_acuracy = json["min_acuracy"].asDouble();
    else
        this->min_acuracy = 10;

    if(json["database_name"] != Json::nullValue)
        this->database_name = json["database_name"].asString();
    else
        this->database_name = "./database/normal.csv";


    if(json["seeds"] != Json::nullValue){
        const Json::Value seeds = json["seeds"];
        for ( int index = 0; index < seeds.size(); ++index )  // Iterates over the sequence elements.
            this->seeds.push_back(seeds[index].asString());
    }else{
        this->seeds.push_back("0");
        this->seeds.push_back("1");
    }


    if(json["inputs"] != Json::nullValue)
        this->inputs = json["inputs"].asInt();
    else
        this->inputs = 2;

    if(json["outputs"] != Json::nullValue)
        this->outputs = json["outputs"].asInt();
    else
        this->outputs = 1;

    if(json["num_neurons"] != Json::nullValue)
        this->num_neurons = json["num_neurons"].asInt();
    else
        this->num_neurons = 2;

    types = (int*)calloc(num_neurons, sizeof(int));
    types[0] = 0;
    types[1] = 1;

    threads = 16;
    */
}

//Logger make Map
void Logger::makeMap(){
    
}
