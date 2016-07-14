#ifndef IOMananger_H
#define IOMananger_H

// includes
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

// files
#include "../json_parser/json.h"
#include "./BaseTypes.hpp"
#include "./ContextVariables.hpp"
#include "./Entities.hpp"
// defines

// namespace
using namespace std;

//
class Output;
class OutMessage;
typedef OutMessage Msg;

class Configs : public Variables {
	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;
	bool ok;

	void decodeMap();

  public:
	Configs();

	void openFile(const char*);
	void runFile(const char*);
	void setConfigs();

	bool configOk();
	string error_msg;

	// variables
	Json::Value json;

	// variavles list
	bool output;
	bool sync_output;

	bool logging;
	bool sync_logging;
	string logname;

	// io
	uint windows_cols;
	uint windows_lines;

	// parallel
	uint threads;

	void endSignal();
	void waitEnd();
};


class Output {
  private:
	static string module_name;

	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;

	queue<Message> print_queue;
	std::mutex queue_mutex;
	std::mutex out_mutex;

	void makeMap();
	map<string, Screen*> screen;
	void toScreen(Message&);
	void print(Message&);
	static void setSize(uint, uint);

  public:
	Output();
	~Output();

	void run();
	void start();
	void waitSignal();
	void runSignal();
	void endSignal();
	void waitEnd();

	void print(uint, uint, string, string);
	void printMsgBox(string, string);
	void printBarGraph(string, double);
	void printValues(string, map<string, string>);
};

class Logger {
  public:
	enum TYPE { ERROR = 0, INFO = 1, WARNING = 2, DEBUG = 3, OTHER = 4 };

  private:
	static string module_name;

	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;

	queue<Message> print_queue;
	std::mutex queue_mutex;
	std::mutex log_mutex;
	thread* running_thread;

	void makeMap();
	map<string, Screen*> screen;

	void toFile(Message&);
	void decision(Message&);
	void logit(string, string, uint);

	string getTime();
	string getFileName(TYPE);

	bool type_activated(Message&);


  public:
	Logger();
	~Logger();

	void run();
	void start();
	void waitSignal();
	void runSignal();

	void log(string, TYPE, string);

	void endSignal();
	void waitEnd();
};


class Jobs {
  private:
	std::mutex work_mutex;
	std::condition_variable work_signal;
	int works;

	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;
	
	std::mutex finish_mutex;
	bool is_online;

  public:
	Jobs();

	void setOK();
	void waitOK();

	void waitWork();
	void addWork();

	bool finished();
	void finish();
};

class OutMessage {
	string stream;


  public:
	Logger::TYPE type;

	string name1, name2;
	double valor;

	bool isOutput, isLog, isGrapth;

	Output* output;
	Logger* log;

	OutMessage(string name);
	OutMessage(string name, Logger::TYPE type);
	OutMessage(string name1, string name2);
	OutMessage(string name1, string name2, Logger::TYPE type);
	OutMessage();

	~OutMessage();

	template <class A0>
	void add(A0& os) {
		stringstream ss;
		ss << stream << os;
		stream = ss.str();
	}
};


class Input {
  private:
	std::mutex end_mutex;
	bool ended;

  public:
	Input();

	void start();

	void waitEnter();
	bool isEnded();

	void endSignal();

	void run();
};

template <class T>
OutMessage& operator<<(OutMessage& msg, const T& val) {
	msg.add(val);
	return msg;
}

class Mananger {
  private:
	Output* output;
	Logger* log;

  public:
	Mananger(Output* output, Logger* logger);
	OutMessage& out(OutMessage out);
	OutMessage& out_log(OutMessage out);
};

namespace io {
	extern Configs configs;
	extern Output output;
	extern Input input;
	extern Logger logger;
	extern Jobs jobs;
	extern Mananger man;
}

void run_io();

#endif
