#pragma once

#include <memory>
#include <mutex>

#include <boost/program_options.hpp>
#include <logger.hpp>
namespace po = boost::program_options;
#include <cpprest/asyncrt_utils.h>


class Config
{
public:

	
	static const std::string mysql_host;
	static const std::string mysql_port;
	static const std::string mysql_database;
	static const std::string mysql_user;
	static const std::string mysql_password;

	static const std::string bolt_host;
	static const std::string bolt_port;


	virtual ~Config() {}
	static Config& getInstance();

	utility::string_t Config::getConfigOption(const std::string con_name);
	std::string Config::getStdConfigOption(const std::string con_name);
	utility::string_t getServerHostWithPort();

private:
	/// <summary>
	/// Initializes a new instance of the <see cref="Config"/> class.
	/// Read the configuration file and store configuration option in a variable
	/// </summary>
	Config(void);

	Config(const Config& src);
	Config &operator=(const Config& rhs);
	static std::unique_ptr<Config> m_instance;
	static std::once_flag m_instance_flag;

	BoltLog bolt_log;


	/// <summary>
	/// variables map to store read configuration options
	/// </summary>
	po::variables_map m_config_map;



	std::string m_config_file = "c:/bolt_config.cfg";

	/// <summary>
	/// A Group of options that will be
	/// read from config file
	/// </summary>
	po::options_description m_config_file_options;


	/// <summary>
	/// A group of options that will be
	/// allowed in config file
	/// </summary>
	po::options_description m_config;
};