
#pragma once

#include <istream>
#include <ostream>
#include <vector>

#include "rapidxml.hpp"

namespace rapidxsd {

	template <typename T>
	class sequence : public std::vector<T>
	{
	};

	template <typename T>
	class optional
	{
	};

	/////////////////////////////////////////////////////////////////////////
	// parser

	class parser
	{
	public:
		explicit parser(std::istream &);

	public:
		template <typename T>
		bool parse(T &, const char* tag=0);

	public:
		template <typename T>
		bool parse_field(T &, const char* tag);

		template <typename T>
		bool parse_field(sequence<T> &, const char* tag);

		template <typename T>
		bool parse_field(optional<T> &, const char* tag);

	public:
		const char* get_string_and_advance();

	private:
		std::istream &_is;
		rapidxml::xml_node<> *_node;
	};

	template <typename T>
	struct field_parser
	{
		static bool parse_field(parser &, T &);
	};


	/////////////////////////////////////////////////////////////////////////
	// composer

	class composer
	{
	public:
		explicit composer(std::ostream &);

	public:
		template <typename T>
		bool compose(const T &, const char* tag);

	public:
		template <typename T>
		bool compose_field(const T &, const char* tag, int length=-1);

		template <typename T>
		bool compose_field(const sequence<T> &, const char* tag, int length=-1);

		template <typename T>
		bool compose_field(const optional<T> &, const char* tag, int length=-1);

	public:
		bool open_element(const char* tag, int length=-1);
		bool close_element(const char* tag, int length=-1);

		bool put_char(char);
		bool put_string(const char*);
		bool put_string(const char*, int length);
		bool flush_buffer();

	private:
		std::ostream &_os;
	};

	template <typename T>
	struct field_composer
	{
		static bool compose_field(composer &, const T &);
	};

} // namespace rapidxsd

#include "rapidxsd.tcc"

