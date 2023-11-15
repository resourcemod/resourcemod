#pragma once

#include "rana.hpp"

namespace rana {

class printer
{
public:
	printer() : encoding_(rana::encoding::utf8), indent_(true), indent_length_(4), space_before_(true), space_after_(true)
	{
	}

	printer &encoding(rana::encoding e) { encoding_ = e; return *this; }
	rana::encoding get_encoding() const { return encoding_; }
        printer &ascii(bool enable) { encoding_ = enable ? rana::encoding::ascii : rana::encoding::utf8; return *this; }
	printer &latin1(bool enable) { encoding_ = enable ? rana::encoding::latin1 : rana::encoding::utf8; return *this; }
	printer &utf8(bool enable) { encoding_ = enable ? rana::encoding::utf8 : rana::encoding::ascii; return *this; }
	printer &pretty(bool enable) { indent(enable); space_after(enable); space_before(enable); return *this; }
	printer &indent(bool enable) { indent_ = enable; return *this; }
	printer &indent_length(std::size_t length) { indent_length_ = length; return *this; }
	printer &space_before(bool enable) { space_before_ = enable; return *this; }
	printer &space_after(bool enable) { space_after_ = enable; return *this; }

	void to_file(const value &v, const std::string &filename) { std::ofstream stream(filename); to_stream(v, stream); }
	std::string to_string(const value &v) { std::stringstream ss; to_stream(v, ss); return ss.str(); }
	void to_stream(const value &v, std::ostream &stream) { write(v, stream, 0); }

private:
    static char int2hex(int i)
    {
        switch(i)
        {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default: throw std::runtime_error(std::to_string(i));
        }
    }

    void encode_string(const std::string &s, std::ostream &stream)
    {
        static std::vector<unsigned char> previous;

        for(auto cs : s)
        {
            unsigned char c = cs;

            if(previous.empty())
            {
                bool not_handled = false;

                switch(c)
                {
                case '"': stream << "\\\""; break;
                case '\\': stream << "\\\\"; break;
                case '/': stream << "\\/"; break;
                case '\b': stream << "\\b"; break;
                case '\f': stream << "\\f"; break;
                case '\n': stream << "\\n"; break;
                case '\r': stream << "\\r"; break;
                case '\t': stream << "\\t"; break;
                default: not_handled = true;
                }

                if(not_handled)
                {
                    if(c <= 0x7F || encoding_ == rana::encoding::utf8)
                    {
                        stream << c;
                    }
                    else
                    {
                        previous.push_back(c);
                    }
                }
            }
            else
            {
                previous.push_back(c);
                int expected = 0;
                uint32_t code_point = 0;

                if(previous.front() ^ 0x40 >> 2)
                {
                    code_point += previous[0] & 0x1F;
                    code_point <<= 6;
                    code_point += previous[1] & 0x3F;

                    previous.clear();

                    if(code_point < 0xFF && encoding_ == rana::encoding::latin1)
                    {
                        stream << (char)code_point;
                    }
                    else if(code_point <= 0xD7FF)
                    {
                        std::string encoded = "\\u";
                        encoded.append(1, int2hex((code_point >> 12) & 0xF));
                        encoded.append(1, int2hex((code_point >> 8) & 0xF));
                        encoded.append(1, int2hex((code_point >> 4) & 0xF));
                        encoded.append(1, int2hex(code_point & 0xF));
                        stream << encoded;
                    }
                }
                else if(previous.front() ^ 0x40 >> 3)
                {
                    expected = 3;
                    throw std::runtime_error("unsupported");
                }
                else if(previous.front() ^ 0x40 >> 4)
                {
                    expected = 4;
                    throw std::runtime_error("unsupported");
                }
            }
        }
    }

	void write_scalar(const value &v, std::ostream &stream)
	{
		if (v.is_null())
		{
			stream << "null";
			return;
		}
		if (v.is_true())
		{
			stream << "true";
			return;
		}
		if (v.is_false())
		{
			stream << "false";
			return;
		}
		if (v.is_number())
		{
			stream << v.as_number();
			return;
		}
		if (v.is_string())
		{
            stream << "\"";
            encode_string(v.as_string(), stream);
            stream << "\"";
			return;
		}
		throw std::runtime_error("not scalar");
	}

	void write(const value &v, std::ostream &stream, std::size_t depth, const std::string &key = "", bool has_key = false)
	{
		std::string full_indent_string;

		if (indent_)
		{
			if (indent_string_.size() != indent_length_)
			{
				indent_string_ = std::string(indent_length_, ' ');
			}

			for (std::size_t i = 0; i < depth; i++)
			{
				full_indent_string.append(indent_string_);
			}

			stream << full_indent_string;
		}

		if (has_key)
		{
            stream << "\"";
            encode_string(key, stream);
            stream << "\"";

			if (space_before_)
			{
				stream << " ";
			}

			stream << ":";

			if (space_after_)
			{
				stream << " ";
			}
		}

		if (!v.is_container())
		{
			write_scalar(v, stream);
			return;
		}

		std::size_t count = 0;

		if (v.is_array())
		{
			stream << "[";

			if (indent_)
			{
				stream << std::endl;
			}

			for (const auto &element : v.iter_array())
			{
				write(element, stream, depth + 1);

				if (count++ < v.size() - 1)
				{
					stream << ",";

					if (space_after_)
					{
						stream << " ";
					}
				}

				if (indent_)
				{
					stream << std::endl;
				}
			}

			if (indent_)
			{
				stream << full_indent_string;
			}

			stream << "]";
		}
		else
		{
			stream << "{";

			if (indent_)
			{
				stream << std::endl;
			}

			for (const auto &key_value_pair : v.iter_object())
			{
				write(key_value_pair.second, stream, depth + 1, key_value_pair.first, true);

				if (count++ < v.size() - 1)
				{
					stream << ",";

					if (space_after_)
					{
						stream << " ";
					}
				}

				if (indent_)
				{
					stream << std::endl;
				}
			}

			if (indent_)
			{
				stream << full_indent_string;
			}

			stream << "}";
		}
	}

	rana::encoding encoding_;
	bool indent_;
	std::size_t indent_length_;
	bool space_before_;
	bool space_after_;

	std::string indent_string_;
};

}
