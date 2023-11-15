#pragma once

#include "value.hpp"
#include "encoding.hpp"
#include "parser.hpp"
#include "printer.hpp"

namespace rana {

inline value from_file(const std::string &filename)
{
	value result;
	parser().parse_file(filename, result);
	return result;
}

inline void to_file(const value &v, const std::string &filename)
{
	printer().to_file(v, filename);
}

inline value from_string(const std::string &string)
{
	value result;
	parser().parse_string(string, result);
	return result;
}

inline std::string to_string(const value &v)
{
	return printer().to_string(v);
}

inline value from_stream(std::istream &stream)
{
	value result;
	parser().parse_stream(stream, result);
	return result;
}

inline std::istream &operator>>(std::istream &stream, value &v)
{
	v = from_stream(stream);
	return stream;
}

inline void to_stream(const value &v, std::ostream &stream)
{
	printer().to_stream(v, stream);
}

inline std::ostream &operator<<(std::ostream &stream, const value &v)
{
	to_stream(v, stream);
	return stream;
}

} // namespace rana
