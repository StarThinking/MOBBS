#include "librbd/MOBBS/ConfigParser.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;

namespace librbd
{
	ConfigParser::ConfigParser(string conf_path):m_conf_path(conf_path)
	{}

	void ConfigParser::parse()
	{
		ifstream fin(m_conf_path.c_str());
		if(!fin.is_open())
		{
			take_log("warning: mobbs.conf is missiong. using default config");
			return;
		} // fin.is_open
		bool can_parse = false;
		while(!fin.eof())
		{
			char buf[LINE_MAX_SIZE];
			fin.getline(buf, LINE_MAX_SIZE);
			string line(buf);
			if(is_comment(line) || is_blank(line)) continue;
			if(is_label(line))
			{
				string label("mobbs-global");
				if(is_current_label(label, line))
					can_parse = true;
				else
					can_parse = false;
				continue;
			}
			if(!can_parse) continue;
			pair<string, string> p = parse_line(line);
			set_variable(p.first, p.second);
		} // while fin.eof
		fin.close();
	}

	pair<string, string> ConfigParser::parse_line(string line)
	{
		size_t l;
		l = line.find('=');
		string key = line.substr(0, l);
		key = erase_space(key);
		transform(key.begin(), key.end(), key.begin(), ::toupper);
		string value = line.substr(l + 1, line.length());
		value = erase_space(value);
		transform(value.begin(), value.end(), value.begin(), ::toupper);
		
		pair<string, string> p(key, value);
		return p;
	}

	string ConfigParser::erase_space(string line)
	{
		// erase left space
		size_t l;
		l = line.find_first_not_of(" \t");
		if(l != string::npos)
			line.erase(line.begin(), line.begin() + l);
		//erase right space
		l = line.find_last_not_of(" \t");
		if(l != string::npos)
			line.erase(line.begin() + l + 1, line.end());

		return line;
	}

	bool ConfigParser::is_comment(string line)
	{
		size_t l;
		l = line.find('#');
		if(l == string::npos)
			return false;
		return true;
	}

	bool ConfigParser::is_blank(string line)
	{
		size_t l;
		l = line.find_last_not_of(" \t\n\r");
		if(l == string::npos)
			return true;
		return false;
		
	}

	bool ConfigParser::is_label(string line)
	{
		size_t l;
		l = line.find('[');
		if(l == string::npos) return false;
		l = line.find(']', l);
		if(l == string::npos) return false;
		return true;
	}

	bool ConfigParser::is_current_label(string label, string line)
	{
		label = "[" + label + "]";
		size_t l;
		l = line.find(label);
		if(l == string::npos) return false;
		return true;
	}

	void ConfigParser::set_variable(string key, string value)
	{
		if(key.compare("DEFAULT_POOL") == 0)
		{
			if(value.compare("SSD_POOL") == 0)
				DEFAULT_POOL = SSD_POOL;
			else
				DEFAULT_POOL = HDD_POOL;
		}
		else if(key.compare("TAKE_LOG_LIBRBD") == 0)
		{
			if(value.compare("TRUE") == 0)
				TAKE_LOG_LIBRBD = 1;
			else
				TAKE_LOG_LIBRBD = 0;
		}
		else if(key.compare("TAKE_LOG_MIGRATER") == 0)
		{
			if(value.compare("TRUE") == 0)
				TAKE_LOG_MIGRATER = 1;
			else
				TAKE_LOG_MIGRATER = 0;
		}
		else if(key.compare("TAKE_LOG_ANALYZER") == 0)
		{
			if(value.compare("TRUE") == 0)
				TAKE_LOG_ANALYZER = 1;
			else
				TAKE_LOG_ANALYZER = 0;
		}
		else if(key.compare("EXTENT_MAP_REBUILD") == 0)
		{
			if(value.compare("TRUE") == 0)
				EXTENT_MAP_REBUILD = 1;
			else
				EXTENT_MAP_REBUILD = 0;
		}
		else if(key.compare("DO_MIGRATION") == 0)
		{
			if(value.compare("TRUE") == 0)
				DO_MIGRATION = 1;
			else
				DO_MIGRATION = 0;
		}
	}
}
