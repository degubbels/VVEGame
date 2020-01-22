//#include <string>

#ifndef MIDIREADER_H
#define MIDIREADER_H

namespace ve {

	class Midi {

	public:
		Midi() {};
		~Midi() {};

		void readFile(std::string fileName);
	};
}

#endif