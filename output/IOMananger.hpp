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

	// io
	bool sync_output;
	uint windows_cols;
	uint windows_lines;

	// parallel
	uint threads;

	void endSignal();
	void waitEnd();
};

class Output;

class OutMessage {
	string stream;

  public:
	Output* output;
	OutMessage(Output*);
	OutMessage(){};
	~OutMessage();

	template <class A0>
	void add(A0& os) {
		stringstream ss;
		ss << stream << os;
		stream = ss.str();
	}
};

typedef OutMessage Msg;

class Output {
  private:
	static string module_name;

	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;

	queue<Message> print_queue;
	std::mutex queue_mutex;
	std::mutex out_mutex;

	thread* running_thread;

	void makeMap();
	map<string, Screen*> screen;

	void toScreen(Message&);

	void print(Message&);

  public:
	Output();
	~Output();

	void run();
	void start();
	void waitSignal();
	void runSignal();

	void print(uint, uint, string, string);
	void printMsgBox(string, string);
	void printBarGraph(string, double);
	void printValues(string, map<string, string>);

	static void setSize(uint, uint);

	void endSignal();
	void waitEnd();

	OutMessage& out(OutMessage out) {
		out.output = this;
		return out;
	}
};

template <class T>
OutMessage& operator<<(OutMessage& msg, const T& val) {
	msg.add(val);
	return msg;
}


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

class Logger {
  private:
	static string module_name;

	std::mutex m_ended;
	std::condition_variable s_ended;
	bool ended;

	queue<Message> print_queue;
	std::mutex queue_mutex;
	thread* running_thread;

	void makeMap();
	map<string, Screen*> screen;

  public:
	enum Type { ERROR, INFO, WARNING, DEBUG };
	Logger();
	~Logger();

	void run();
	void start();
	void waitSignal();
	void runSignal();

	void log(string, Type, string);

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

  public:
	Jobs();

	void setOK();
	void waitOK();

	void waitWork();
	void addWork();

	bool finished();
	void finish();
};

namespace io {
	extern Configs configs;
	extern Output output;
	extern Input input;
	extern Logger logger;
	extern Jobs jobs;
}

void run_io();

#endif
