#include "./BaseTypes.hpp"
#include "./Entities.hpp"
#include "./IOMananger.hpp"


void run_out();

int main(int argc, char* argv[]) {
	if (argc < 2) {
		io::configs.runFile("{}");
	} else
		io::configs.openFile(argv[1]);
	io::configs.setConfigs();

	thread io_th(run_io);

	std::vector<thread> threads;

	for (int i = 0; i < 1; i++)
		threads.push_back(thread(run_out));

	while (1)
		std::this_thread::sleep_for(std::chrono::seconds(50));
}


void run_out() {
	while (1) {
		std::thread::id this_id = std::this_thread::get_id();
		io::man.out(Msg()) << "Printando thread " << this_id << " name";
		io::man.out_log(Msg()) << "my name is";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
