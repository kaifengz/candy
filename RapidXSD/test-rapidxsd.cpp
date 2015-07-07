
#include "rapidxsd.h"
#include <iostream>
#include <sstream>

class service_info_t
{
public:
	bool serialize(std::ostream &os)
	{
		rapidxsd::composer composer(os);
		return composer.compose(*this, "service_info");
	}

	bool unserialize(std::istream &is)
	{
		rapidxsd::parser parser(is);
		return parser.parse(*this, "service_info");
	}

public:
	std::string name;
	long long state;
	long in_queue_size;
	unsigned int out_queue_size;
	char doing_count;
	unsigned char in_queue_traffic;
	double out_queue_traffic;
	rapidxsd::sequence<std::string> config;
};

namespace rapidxsd {

	template <>
	struct field_parser<service_info_t>
	{
		static bool parse_field(parser &parser, service_info_t &info)
		{
			return
				parser.parse_field(info.name, "name") &&
				parser.parse_field(info.state, "state") &&
				parser.parse_field(info.in_queue_size, "in_queue_size") &&
				parser.parse_field(info.out_queue_size, "out_queue_size") &&
				parser.parse_field(info.doing_count, "doing_count") &&
				parser.parse_field(info.in_queue_traffic, "in_queue_traffic") &&
				parser.parse_field(info.out_queue_traffic, "out_queue_traffic") &&
				parser.parse_field(info.config, "config");
		}
	};

	template <>
	struct field_composer<service_info_t>
	{
		static bool compose_field(composer &composer, const service_info_t &info)
		{
			return
				composer.compose_field(info.name, "name") &&
				composer.compose_field(info.state, "state") &&
				composer.compose_field(info.in_queue_size, "in_queue_size") &&
				composer.compose_field(info.out_queue_size, "out_queue_size") &&
				composer.compose_field(info.doing_count, "doing_count") &&
				composer.compose_field(info.in_queue_traffic, "in_queue_traffic") &&
				composer.compose_field(info.out_queue_traffic, "out_queue_traffic") &&
				composer.compose_field(info.config, "config");
		}
	};

} // namespace rapidxsd


int main()
{
	service_info_t info;
	info.name = "inmta";
	info.state = 2;
	info.in_queue_size = 12;
	info.out_queue_size = 34;
	info.doing_count = 56;
	info.in_queue_traffic = 78;
	info.out_queue_traffic = 90;
	info.config.push_back("policy");
	info.config.push_back("config-2");

	std::cout << "====================" << std::endl;
	info.serialize(std::cout);
	std::cout << "====================" << std::endl;

	std::stringstream ss;
	info.serialize(ss);

	service_info_t info2;
	info2.unserialize(ss);

	info2.serialize(std::cout);
	std::cout << "====================" << std::endl;
	return 0;
}

