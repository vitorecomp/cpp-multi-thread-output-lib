#include "Entities.hpp"

////////////////////////////////////////////////////////////////////////////////////////
// MessageBox
////////////////////////////////////////////////////////////////////////////////////////
MessageBox::MessageBox(string name, uint stx, uint sty, uint sx, uint sy,
					   uint endx) {
	this->name = name;
	this->endx = endx;

	this->start_x = stx;
	this->start_y = sty;

	this->size_x = sx;
	this->size_y = sy;

	Box::draw(stx, sty, sx, sy);
	Figure::move(start_x, start_y + 5);
	cout << "   ";
	cout << name;
	cout << "   ";
	cout << flush;
}

void MessageBox::print(Message &msg) {
	string value = msg.msg;

	std::replace(value.begin(), value.end(), '\n',
				 ' ');  // replace all 'x' to 'y'

	if (value.size() <= (size_y - 6))
		add_on_list(value);
	else {
		do {
			string value_inter = value.substr(0, size_y - 6);
			add_on_list(value_inter);
			value = value.substr(size_y - 6);
		} while (value.size() >= size_y - 6);
		add_on_list(value);
	}
	printlist();
}

void MessageBox::add_on_list(string msg) {
	if (msg_list.size() > (size_x - 4))
		msg_list.pop_front();
	msg_list.push_back(msg);
}

void MessageBox::printlist() {
	Figure::move(start_x + 2, start_y + 3);
	uint j = 3;
	list<string>::iterator i;
	for (i = msg_list.begin(); i != msg_list.end(); ++i, j++) {
		Figure::move(start_x + j, start_y + 3);
		cout << *i;
		for (uint k = i->size(); k < size_y - 6; k++)
			cout << " ";
		cout << flush;
	}

	Figure::end(endx);
}

////////////////////////////////////////////////////////////////////////////////////////
// ProgressBar
////////////////////////////////////////////////////////////////////////////////////////
ProgressBar::ProgressBar(string name, uint stx, uint sty, uint sx, uint sy,
						 DIRECTION dir, double min, double max, uint endx) {
	this->name = name;
	this->endx = endx;

	this->start_x = stx;
	this->start_y = sty;

	this->size_x = sx;
	this->size_y = sy;

	this->max = max;
	this->min = min;

	this->dir = dir;

	Box::draw(stx, sty, sx, sy);
	Figure::move(start_x, start_y + 5);

	cout << "   ";
	cout << name;
	cout << "   ";
	cout << flush;

	switch (dir) {
		case HORIZONTAL:
			printHorizontalGraphArrange();
			printHorizontalGraph(50);
			break;
		case VERTICAL:
			throw std::invalid_argument("Ainda não implementado");
			break;
		default:
			throw std::invalid_argument("Ainda não implementado");
			break;
	}
}

void ProgressBar::printHorizontalGraphArrange() {
	Box::move(start_x + 2, start_y + 2);
	cout << "\u250F"
		 << " Progress" << endl;

	for (uint i = 0; i < size_x - 4; i++) {
		Box::move(start_x + 3 + i, start_y + 2);
		cout << "\u2503" << endl;

		Box::move(start_x + 3 + i, start_y + size_y - 2);
		cout << "\u2503" << endl;
	}
	Box::move(start_x + size_x - 1, start_y + 2);
	cout << "\u2517" << endl;

	Box::move(start_x + 2, start_y + size_y - 2);
	cout << "\u2513" << endl;

	Box::move(start_x + size_x - 1, start_y + size_y - 2);
	cout << "\u251B" << endl;

	if (size_y > 20) {
		Box::move(start_x + size_x - 1, start_y + size_y - 20);
		cout << "Percentage:     %" << flush;
	}
}

void ProgressBar::printHorizontalGraph(double percentage) {
	Box::move(start_x + size_x - 1, start_y + size_y - 8);
	cout << std::setw(3) << floor(percentage) << endl;
	uint numBars = floor((size_y - 6) * ((double)percentage / 100));
	for (uint i = 0; i < size_x - 4; i++) {
		Box::move(start_x + 3 + i, start_y + 3);
		for (uint j = 0; j < numBars; j++)
			cout << "\u2588";
		for (uint j = 0; j < size_y - numBars - 5; j++)
			cout << " ";
	}

	Figure::end(endx);
}

void ProgressBar::print(Message &msg) {
	double value = std::stod(msg.msg);
	if (value > max) {
		value = max;
	}

	if (value < min) {
		value = min;
	}

	double percentage = (value - min) / (max - min);
	printHorizontalGraph(percentage * 100);
}
