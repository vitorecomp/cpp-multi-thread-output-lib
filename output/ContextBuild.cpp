#include "IOMananger.hpp"

// Start screen
void Output::makeMap() {
	// Screen *main_messages = (Screen *)new MessageBox(
	// 	"main_messages", 0, 0, 35, 80, io::configs.windows_lines);
	// screen.insert(std::pair<string, Screen *>("main_messages",
	// main_messages));
	// Screen *progress_bar = (Screen *)new ProgressBar(
	// 	"progress_bar", 0, 80, 5, 60, ProgressBar::HORIZONTAL, 0, 100,
	// 	io::configs.windows_lines);
	// screen.insert(std::pair<string, Screen *>("progress_bar", progress_bar));
	Screen *main_messages_local = (Screen *)new Terminal();
	screen.insert(
		std::pair<string, Screen *>("main_messages", main_messages_local));
}

// make configs
void Configs::decodeMap() {
	// start configs
}

// Logger make Map
void Logger::makeMap() {
}
