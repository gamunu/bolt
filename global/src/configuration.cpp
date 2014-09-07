#include <fstream>
#include <configuration.hpp>

using namespace std;

/// <summary>
/// The mysql_host
/// </summary>
const string Config::mysql_host = "mysql-host";
/// <summary>
/// The mysql_port
/// </summary>
const string Config::mysql_port = "mysql-port";
/// <summary>
/// The mysql_database
/// </summary>
const string Config::mysql_database = "mysql-database";
/// <summary>
/// The mysql_user
/// </summary>
const string Config::mysql_user = "mysql-user";
/// <summary>
/// The mysql_password
/// </summary>
const string Config::mysql_password = "mysql-password";

/// <summary>
/// The bolt_host
/// </summary>
const string Config::bolt_host = "bolt-host";
/// <summary>
/// The bolt_port
/// </summary>
const string Config::bolt_port = "bolt-port";

// Config Class Declaration
unique_ptr<Config> Config::m_instance;
once_flag Config::m_instance_flag;

Config& Config::getInstance()
{
	call_once(m_instance_flag,
		[] {
		m_instance.reset(new Config);
	});
	return *m_instance.get();
}

Config::Config()
{
	try {
		string conf_mysql_database;
		string conf_mysql_password;
		string conf_mysql_user;
		string conf_mysql_host;
		string conf_mysql_port;

		string conf_bolt_host;
		string conf_bolt_port;

		// Declare a group of options that will be
		// allowed in config file
		po::options_description config("Configuration");
		config.add_options()
			(mysql_host.c_str(), po::value<string>(&conf_mysql_host)->default_value("localhost"), "MySql Database host")
			(mysql_port.c_str(), po::value<string>(&conf_mysql_port)->default_value("3306"), "MySql Database port")
			(mysql_database.c_str(), po::value<string>(&conf_mysql_database)->default_value("boltmysql"), "Name of the mysql database")
			(mysql_user.c_str(), po::value<string>(&conf_mysql_user)->default_value("root"), "Name of the mysql database")
			(mysql_password.c_str(), po::value<string>(&conf_mysql_password)->default_value(""), "Password of the mysql database")
			(bolt_host.c_str(), po::value<string>(&conf_bolt_host)->default_value("localhost"), "Bolt server host")
			(bolt_port.c_str(), po::value<string>(&conf_bolt_port)->default_value("34568"), "bolt server password");

		//Add allowed configurations
		m_config_file_options.add(config);

		ifstream ifs(m_config_file.c_str());

		if (!ifs)
		{
			bolt_log << BoltLog::LOG_ERROR << "can not open config file: " << m_config_file << "\n";
		}
		else
		{
			store(parse_config_file(ifs, m_config_file_options), m_config_map);
			notify(m_config_map);
		}
	}
	catch (exception& e)
	{
		bolt_log << BoltLog::LOG_ERROR << e.what() << "\n";
	}
}

utility::string_t Config::getConfigOption(const string con_name)
{
	if (!m_config_map.count(con_name))
	{
		bolt_log << BoltLog::LOG_ERROR << "Config read error: " << m_config_file << "\n";
		return nullptr;
	}
	return utility::conversions::to_string_t(m_config_map[con_name].as<string>());
}

std::string Config::getStdConfigOption(const std::string con_name)
{
	if (!m_config_map.count(con_name))
	{
		bolt_log << BoltLog::LOG_ERROR << "Config read error: " << m_config_file << "\n";
		return nullptr;
	}
	return m_config_map[con_name].as<string>();
}

utility::string_t Config::getServerHostWithPort()
{
	utility::string_t address = U("http://") + getConfigOption(bolt_host) + U(":");
	address.append(utility::conversions::to_string_t(getConfigOption(bolt_port)));
	return address;
}