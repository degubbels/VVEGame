//#include <string>

#ifndef MIDIREADER_H
#define MIDIREADER_H

namespace ve {

	class MidiReader {

	public:
		MidiReader() {};
		~MidiReader() {};

		void readFile(std::string fileName);
	};
}

#endif