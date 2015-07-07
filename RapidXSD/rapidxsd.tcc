
#pragma once

namespace rapidxsd {

	/////////////////////////////////////////////////////////////////////////
	// field_validator && field_trait

	struct field_validator_key
	{
		static bool validate(rapidxml::xml_node<> *node)
		{
			rapidxml::xml_node<> *first_child = node->first_node();
			if (first_child == 0)
				return false;

			return (first_child->type() != rapidxml::node_data);
		}
	};

	struct field_validator_value
	{
		static bool validate(rapidxml::xml_node<> *node)
		{
			rapidxml::xml_node<> *first_child = node->first_node();
			if (first_child == 0)
				return false;

			return (first_child->type() == rapidxml::node_data);
		}
	};

	template <typename T>
	struct field_trait
	{
		typedef field_validator_key validator;
	};

	/////////////////////////////////////////////////////////////////////////
	// base-type field_trait

#define DECLARE_BASE_TYPE_TRAIT(Type) \
	template <> \
	struct field_trait<Type> { \
		typedef field_validator_value validator; \
	};

	DECLARE_BASE_TYPE_TRAIT(char);
	DECLARE_BASE_TYPE_TRAIT(long);
	DECLARE_BASE_TYPE_TRAIT(long long);
	DECLARE_BASE_TYPE_TRAIT(double);
	DECLARE_BASE_TYPE_TRAIT(std::string);

#undef DECLARE_BASE_TYPE_TRAIT

	/////////////////////////////////////////////////////////////////////////
	// base-type field_parser

	template <>
	struct field_parser<char>
	{
		static bool parse_field(parser &parser, char &ch)
		{
			const char* str = parser.get_string_and_advance();
			if (str == 0)
				return false;
			if (*str == '\0')
				return false;
			if (*(str+1) != '\0')
				return false;

			ch = *str;
			return true;
		}
	};

	template <>
	struct field_parser<long>
	{
		static bool parse_field(parser &parser, long &n)
		{
			const char* str = parser.get_string_and_advance();
			if (str == 0)
				return false;

			char* end = 0;
			long num = strtol(str, &end, 10);
			if (*end != '\0')
				return false;

			n = num;
			return true;
		}
	};

	template <>
	struct field_parser<long long>
	{
		static bool parse_field(parser &parser, long long &n)
		{
			const char* str = parser.get_string_and_advance();
			if (str == 0)
				return false;

			char* end = 0;
			long long num = strtoll(str, &end, 10);
			if (*end != '\0')
				return false;

			n = num;
			return true;
		}
	};

	template <>
	struct field_parser<double>
	{
		static bool parse_field(parser &parser, double &n)
		{
			const char* str = parser.get_string_and_advance();
			if (str == 0)
				return false;

			char* end = 0;
			double num = strtod(str, &end);
			if (*end != '\0')
				return false;

			n = num;
			return true;
		}
	};

	template <>
	struct field_parser<std::string>
	{
		static bool parse_field(parser &parser, std::string &s)
		{
			const char* str = parser.get_string_and_advance();
			if (str == 0)
				return false;

			s = str;
			return true;
		}
	};

	/////////////////////////////////////////////////////////////////////////
	// base-type field_composer

	template <>
	struct field_composer<char>
	{
		static bool compose_field(composer &composer, const char &ch)
		{
			return composer.put_char(ch);
		}
	};

	template <>
	struct field_composer<long>
	{
		static bool compose_field(composer &composer, const long &n)
		{
			char buff[128];
			snprintf(buff, sizeof(buff), "%ld", n);
			return composer.put_string(buff);
		}
	};

	template <>
	struct field_composer<long long>
	{
		static bool compose_field(composer &composer, const long long &n)
		{
			char buff[128];
			snprintf(buff, sizeof(buff), "%lld", n);
			return composer.put_string(buff);
		}
	};

	template <>
	struct field_composer<double>
	{
		static bool compose_field(composer &composer, const double &n)
		{
			char buff[128];
			snprintf(buff, sizeof(buff), "%f", n);
			return composer.put_string(buff);
		}
	};

	template <>
	struct field_composer<std::string>
	{
		static bool compose_field(composer &composer, const std::string &str)
		{
			// TODO: escape XML characters
			return composer.put_string(str.c_str(), str.size());
		}
	};

	/////////////////////////////////////////////////////////////////////////
	// dependant-type field_trait, field_parser and field_composer

#define DECLARE_DEPENDANT_TYPE_TRAIT(Type, BaseType) \
	template <> \
	struct field_trait<Type> { \
		typedef field_validator_value validator; \
	}; \
	template <> \
	struct field_parser<Type> { \
		static bool parse_field(parser &parser, Type &data) { \
			BaseType base_data; \
			if (!field_parser<BaseType>::parse_field(parser, base_data)) \
				return false; \
			data = base_data; \
			return true; \
		} \
	}; \
	template <> \
	struct field_composer<Type> { \
		static bool compose_field(composer &composer, const Type &data) { \
			return field_composer<BaseType>::compose_field(composer, static_cast<const BaseType &>(data)); \
		} \
	}

	DECLARE_DEPENDANT_TYPE_TRAIT(unsigned char,      char);
	DECLARE_DEPENDANT_TYPE_TRAIT(short,              long);
	DECLARE_DEPENDANT_TYPE_TRAIT(unsigned short,     long);
	DECLARE_DEPENDANT_TYPE_TRAIT(int,                long);
	DECLARE_DEPENDANT_TYPE_TRAIT(unsigned int,       long);
	DECLARE_DEPENDANT_TYPE_TRAIT(unsigned long,      long);
	DECLARE_DEPENDANT_TYPE_TRAIT(unsigned long long, long long);
	DECLARE_DEPENDANT_TYPE_TRAIT(float,              double);

#undef DECLARE_DEPENDANT_TYPE_TRAIT

	/////////////////////////////////////////////////////////////////////////
	// parser

	parser::parser(std::istream &is) :
		_is(is)
	{
	}

	template <typename T>
	bool parser::parse(T &t, const char* tag /* = 0 */)
	{
		std::string str;
		while (true)
		{
			char buff[1024*4];
			_is.read(buff, sizeof(buff));
			int n = _is.gcount();
			if (n == 0)
				break;
			str.append(buff, n);
		}

		rapidxml::xml_document<> doc;
		try
		{
			doc.parse<0>(const_cast<char*>(str.c_str()));
		}
		catch (rapidxml::parse_error &err)
		{
			return false;
		}

		_node = doc.first_node();
		if (_node == 0)
			return false;

		return parse_field(t, tag != 0 ? tag : _node->name());
	}

	template <typename T>
	bool parser::parse_field(T &t, const char* tag)
	{
		if (_node == 0)
			return false;
		if (0 != strcmp(_node->name(), tag))
			return false;
		if (!field_trait<T>::validator::validate(_node))
			return false;

		rapidxml::xml_node<> *next = _node->next_sibling();
		_node = _node->first_node();
		bool result = field_parser<T>::parse_field(*this, t);
		_node = next;
		return result;
	}

	template <typename T>
	bool parser::parse_field(sequence<T> &fields, const char* tag)
	{
		sequence<T> parsed_fields;

		while (true)
		{
			if (_node == 0)
				break;
			if (0 != strcmp(_node->name(), tag))
				break;

			parsed_fields.resize( parsed_fields.size() + 1 );

			rapidxml::xml_node<> *next = _node->next_sibling();
			_node = _node->first_node();
			bool result = field_parser<T>::parse_field(*this, parsed_fields.back());
			_node = next;
			if (!result)
				return false;
		}

		fields.swap(parsed_fields);
		return true;
	}

	template <typename T>
	bool parser::parse_field(optional<T> &, const char* tag)
	{
		// TODO:
		return false;
	}

	const char* parser::get_string_and_advance()
	{
		if (_node == 0)
			return 0;

		const char *result = _node->value();
		_node = _node->next_sibling();
		return result;
	}

	/////////////////////////////////////////////////////////////////////////
	// composer

	composer::composer(std::ostream &os) :
		_os(os)
	{
	}

	template <typename T>
	bool composer::compose(const T &t, const char* tag)
	{
		return compose_field(t, tag) && put_char('\n') && flush_buffer();
	}

	template <typename T>
	bool composer::compose_field(const T &t, const char* tag, int length /* = -1 */)
	{
		if (length < 0)
			length = strlen(tag);

		return
			open_element(tag, length) &&
			field_composer<T>::compose_field(*this, t) &&
			close_element(tag, length);
	}

	template <typename T>
	bool composer::compose_field(const sequence<T> &fields, const char* tag, int length /* = -1 */)
	{
		if (length < 0)
			length = strlen(tag);

		for (typename sequence<T>::const_iterator iter = fields.begin();
			iter != fields.end(); ++iter)
		{
			if (!compose_field(*iter, tag, length))
				return false;
		}
		return true;
	}

	template <typename T>
	bool composer::compose_field(const optional<T> &, const char* tag, int length)
	{
		// TODO:
		return false;
	}

	bool composer::open_element(const char* tag, int length)
	{
		if (length < 0)
			length = strlen(tag);

		return put_char('<') && put_string(tag, length) && put_char('>');
	}

	bool composer::close_element(const char* tag, int length)
	{
		if (length < 0)
			length = strlen(tag);

		return put_char('<') && put_char('/') && put_string(tag, length) && put_char('>');
	}

	bool composer::put_char(char ch)
	{
		return _os.put(ch);
	}

	bool composer::put_string(const char* str)
	{
		return _os.write(str, strlen(str));
	}

	bool composer::put_string(const char* str, int length)
	{
		return _os.write(str, length);
	}

	bool composer::flush_buffer()
	{
		return true;
	}

} // namespace rapidxsd

