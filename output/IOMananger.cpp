#include "IOMananger.hpp"

///////////////////////////////////////////////////////////////////////////////
// Configs
//
///////////////////////////////////////////////////////////////////////////////
Configs io::configs;

Configs::Configs() {
	this->ended = false;
	this->ok = true;
}

bool Configs::configOk() {
	return !ok;
}

void Configs::runFile(const char *valor) {
	// validate the input
	string json = valor;

	// parse the string
	Json::Value value;
	Json::Reader reader;

	bool parsed = reader.parse(json, value);
	if (!parsed)
		throw std::invalid_argument("Json File invalid");

	this->json = value;
}


void Configs::openFile(const char *name) {
	string line;
	stringstream ss;
	// open the text
	ifstream myfile;
	myfile.open(name, std::ifstream::in);

	// get the string
	if (myfile.is_open()) {
		while (getline(myfile, line))
			ss << line << '\n';
		myfile.close();
	} else {
		throw std::invalid_argument("Json File invalid");
	}

	// validate the input
	string json = ss.str();
	if (json.length() == 0)
		throw std::invalid_argument("Json File invalid");

	// parse the string
	Json::Value value;
	Json::Reader reader;

	bool parsed = reader.parse(json, value);
	if (!parsed)
		throw std::invalid_argument("Json File invalid");

	// configure from json
	this->json = value;
}

void Configs::setConfigs() {
	if (json["output"] != Json::nullValue)
		this->output = json["output"].asBool();
	else
		this->output = true;

	if (json["sync_output"] != Json::nullValue)
		this->sync_output = json["sync_output"].asBool();
	else
		this->sync_output = false;

	if (json["logname"] != Json::nullValue)
		this->logname = json["logname"].asString();
	else
		this->logname = "./logs/logging";

	if (json["logging"] != Json::nullValue)
		this->logging = json["logging"].asBool();
	else
		this->logging = true;

	if (json["sync_logging"] != Json::nullValue)
		this->sync_logging = json["sync_logging"].asBool();
	else
		this->sync_logging = false;

	if (json["windows_cols"] != Json::nullValue)
		this->windows_cols = json["windows_cols"].asInt();
	else
		this->windows_cols = 140;

	if (json["windows_lines"] != Json::nullValue)
		this->windows_lines = json["windows_lines"].asInt();
	else
		this->windows_lines = 40;

	decodeMap();
	this->endSignal();
}

void Configs::endSignal() {
	std::unique_lock<std::mutex> lck(m_ended);
	ended = true;
	s_ended.notify_all();
}

void Configs::waitEnd() {
	std::unique_lock<std::mutex> lck(m_ended);
	while (!ended)
		s_ended.wait(lck);
}

///////////////////////////////////////////////////////////////////////////////
// Output
//
///////////////////////////////////////////////////////////////////////////////
Output io::output;

string Output::module_name = "output_module";

void run_io() {
	io::configs.waitEnd();

	io::output.start();
	io::logger.start();
	io::input.start();

	io::jobs.setOK();

	while (true) {
		io::jobs.waitWork();

		io::output.run();
		io::input.run();
		io::logger.run();

		if (io::jobs.finished())
			break;
	}
}

Output::Output() {
	this->ended = false;
}

Output::~Output() {
	for (auto &x : screen) {
		delete x.second;
	}
}

void Output::setSize(uint ncols, uint nlines) {
	stringstream ss;
	ss << "echo -ne "
	   << "'"
	   << "\e[8;" << nlines << ";" << ncols << "t"
	   << "'";
	system(ss.str().c_str());
}

void Output::start() {
	Output::setSize(io::configs.windows_cols, io::configs.windows_lines);
	Figure::clearAll();
	makeMap();
	this->endSignal();
}


void Output::endSignal() {
	std::unique_lock<std::mutex> lck(m_ended);
	ended = true;
	s_ended.notify_all();
}

void Output::waitEnd() {
	std::unique_lock<std::mutex> lck(m_ended);
	while (!ended)
		s_ended.wait(lck);
}

void Output::run() {
	queue_mutex.lock();

	if (print_queue.empty()) {
		queue_mutex.unlock();
		return;
	}

	Message message = print_queue.front();
	print_queue.pop();

	queue_mutex.unlock();
	this->toScreen(message);
}

void Output::toScreen(Message &message) {
	if (screen.find(message.box) != screen.end()) {
		this->screen[message.box]->print(message);
	} else {
		stringstream ss;
		ss << "Mapa de screen não encontrado: " << message.box;
		io::logger.log(module_name, Logger::ERROR, ss.str());
	}
}

void Output::print(Message &message) {
	if (!io::configs.output)
		return;

	if (io::configs.sync_output) {
		io::jobs.waitOK();
		std::unique_lock<std::mutex> lck(out_mutex);
		this->toScreen(message);
	} else {
		io::jobs.addWork();
		queue_mutex.lock();
		this->print_queue.push(message);
		queue_mutex.unlock();
	}
}

void Output::printMsgBox(string box, string msg) {
	Message message(0, 0, box, msg);
	this->print(message);
}

void Output::printBarGraph(string box, double value) {
	stringstream msg;
	msg << value;
	Message message(0, 0, box, msg.str());
	this->print(message);
}

void Output::printValues(string box, map<string, string>) {
	// Message message(0, 0, box, msg);
	// this->print_queue.push(message);
}


///////////////////////////////////////////////////////////////////////////////
// Input
//
///////////////////////////////////////////////////////////////////////////////
Input io::input;

Input::Input() {
	ended = false;
}

void Input::start() {
}

void Input::endSignal() {
	end_mutex.lock();
	ended = true;
	end_mutex.unlock();
}

void Input::waitEnter() {
}

void Input::run() {
}

bool Input::isEnded() {
	end_mutex.lock();
	bool ended = this->ended;
	end_mutex.unlock();
	return ended;
}

///////////////////////////////////////////////////////////////////////////////
// Logger
//
///////////////////////////////////////////////////////////////////////////////
Logger io::logger;

string Logger::module_name = "logger_module";

Logger::Logger() {
	this->ended = false;
}

Logger::~Logger() {
	for (auto &x : screen) {
		delete x.second;
	}
}

void Logger::start() {
	makeMap();
	stringstream ss;
	std::size_t pos = io::configs.logname.find_last_of("/");
	std::string str3 = io::configs.logname.substr(0, pos);

	ss << "mkdir -p " << str3;
	system(ss.str().c_str());
	this->endSignal();
}

void Logger::endSignal() {
	std::unique_lock<std::mutex> lck(m_ended);
	ended = true;
	s_ended.notify_all();
}

void Logger::waitEnd() {
	std::unique_lock<std::mutex> lck(m_ended);
	while (!ended)
		s_ended.wait(lck);
}

void Logger::run() {
	queue_mutex.lock();

	if (print_queue.empty()) {
		queue_mutex.unlock();
		return;
	}

	Message message = print_queue.front();
	print_queue.pop();

	queue_mutex.unlock();
	this->toFile(message);
}

void Logger::toFile(Message &message) {
	try {
		this->logit(message.box, message.msg, message.type);
	} catch (...) {
		try {
			stringstream ss;
			ss << "Error ao logar: " << message.box;
			this->logit(module_name, ss.str(), OTHER);
		} catch (...) {
		}
	}
}

string Logger::getFileName(Logger::TYPE type) {
	switch (type) {
		case Logger::DEBUG:
			return io::configs.logname + "-debug.log";
		case Logger::INFO:
			return io::configs.logname + "-info.log";
		case Logger::ERROR:
			return io::configs.logname + "-error.log";
		case Logger::WARNING:
			return io::configs.logname + "-warning.log";
	}
	return io::configs.logname + ".log";
}

string Logger::getTime() {
	return "----------------";
}

void Logger::logit(string module, string msg, uint type) {
	ofstream myfile;
	string filename = this->getFileName((Logger::TYPE)type);

	myfile.open(filename, ios::app);

	stringstream ss;
	ss << this->getTime() << ";" << module << ";" << msg << endl;

	if (myfile.is_open())
		myfile << ss.str();
	else
		throw invalid_argument("Error ao abrir o arquivo");
	myfile.close();
}

bool Logger::type_activated(Message &) {
	// TODO fazer isso
	return true;
}

void Logger::decision(Message &message) {
	if (!io::configs.logging)
		return;
	if (!this->type_activated(message))
		return;

	if (io::configs.sync_logging) {
		io::jobs.waitOK();
		std::unique_lock<std::mutex> lck(log_mutex);
		this->toFile(message);
	} else {
		io::jobs.addWork();
		std::unique_lock<std::mutex> lck(queue_mutex);
		this->print_queue.push(message);
	}
}

void Logger::log(string file, TYPE type, string msg) {
	Message message(file, type, msg);
	this->decision(message);
}

///////////////////////////////////////////////////////////////////////////////
// Jobs
//
///////////////////////////////////////////////////////////////////////////////

Jobs io::jobs;

Jobs::Jobs() {
	this->works = 0;
	is_online = true;
}

void Jobs::waitOK() {
	std::unique_lock<std::mutex> lck(m_ended);
	while (!ended)
		s_ended.wait(lck);
}

void Jobs::setOK() {
	std::unique_lock<std::mutex> lck(m_ended);
	ended = true;
	s_ended.notify_all();
}

void Jobs::waitWork() {
	std::unique_lock<std::mutex> lck(work_mutex);
	while (works == 0)
		work_signal.wait(lck);
	works--;
}

void Jobs::addWork() {
	std::unique_lock<std::mutex> lck(work_mutex);
	works++;
	work_signal.notify_all();
}

void Jobs::finish() {
	std::unique_lock<std::mutex> lck(finish_mutex);
	is_online = true;
}

bool Jobs::finished() {
	std::unique_lock<std::mutex> lck(finish_mutex);
	return !is_online;
}

///////////////////////////////////////////////////////////////////////////////
// OutMessage
//
///////////////////////////////////////////////////////////////////////////////
OutMessage::OutMessage(string name) {
	OutMessage();
	this->name1 = name;
}

OutMessage::OutMessage(string name, Logger::TYPE type) {
	OutMessage();
	this->name2 = name;
	this->type = type;
}

OutMessage::OutMessage(string name1, string name2) {
	OutMessage();
	this->name1 = name1;
	this->name2 = name2;
}

OutMessage::OutMessage(string name1, string name2, Logger::TYPE type) {
	OutMessage();
	this->name1 = name1;
	this->name2 = name2;
	this->type = type;
}

OutMessage::OutMessage() {
	this->isLog = false;
	this->isOutput = false;
	this->isGrapth = false;

	this->output = NULL;
	this->log = NULL;

	this->type = Logger::INFO;

	this->name1 = "main_messages";
	this->name2 = "main_log";
	this->stream = "";
}


OutMessage::~OutMessage() {
	if (isOutput)
		this->output->printMsgBox(name1, stream);

	if (isLog)
		this->log->log(name2, type, stream);

	if (isGrapth)
		this->output->printBarGraph(name1, valor);
}

///////////////////////////////////////////////////////////////////////////////
// Mananger
//
///////////////////////////////////////////////////////////////////////////////

Mananger io::man = Mananger(&io::output, &io::logger);

Mananger::Mananger(Output *output, Logger *logger) {
	this->output = output;
	this->log = logger;
}

OutMessage &Mananger::out(OutMessage out) {
	out.output = output;
	out.isOutput = true;
	return out;
}

OutMessage &Mananger::out_log(OutMessage out) {
	out.output = this->output;
	out.log = this->log;
	out.isOutput = true;
	out.isLog = true;
	return out;
}
