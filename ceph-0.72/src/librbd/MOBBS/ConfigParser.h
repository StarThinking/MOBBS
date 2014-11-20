#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#include "librbd/MOBBS/MOBBS.h"
#include <iostream>
#include <string>

#define LINE_MAX_SIZE 1024

using namespace std;

namespace librbd
{
	class ConfigParser
	{
	public:
		ConfigParser(string conf_path);
		void parse();
	
	private:
		string m_conf_path;

		pair<string, string> parse_line(string line);
		string erase_space(string line);
		bool is_comment(string line);
		bool is_blank(string line);
		bool is_label(string line);
		bool is_current_label(string label, string line);
		void set_variable(string key, string value);
		
	};
}


#endif
