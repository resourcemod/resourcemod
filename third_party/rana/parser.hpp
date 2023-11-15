#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "value.hpp"

namespace {

enum class character_class
{
    literal,
    whitespace,
    quote,
    structure,
    invalid
};

character_class classify(char c)
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return character_class::whitespace;
        case '"':
            return character_class::quote;
        case '[':
        case ',':
        case ']':
        case '{':
        case ':':
        case '}':
            return character_class::structure;
        default:
            return character_class::literal;
    }
}
    
uint8_t hex2int(char c)
{
    if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    else if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    
    throw std::runtime_error("bad hex char");
}
    
}

namespace rana {

class parse_error : public std::runtime_error
{
public:
    parse_error(std::size_t line, std::size_t column, const char *buffer, std::size_t buffer_length, const std::string &message) : std::runtime_error(create_message(line, column, buffer, buffer_length, message))
    {
    }
    
private:
	static std::string create_message(std::size_t line, std::size_t column, const char *buffer, std::size_t buffer_length, const std::string &message)
    {
        std::string error_message = "during parsing of JSON string at line " + std::to_string(line) + ", column " + std::to_string(column) + ": ";
        error_message += " error: " + message + "\n";
        
	std::size_t current_line = 1;
	std::size_t i = 0;
	std::size_t line_start = 0;
	std::size_t line_end = 0;
        
        while (current_line <= line && i < buffer_length)
        {
            i++;
            
            if (buffer[i] == '\n')
            {
                current_line++;
                line_start = i;
            }
        }
        
        while (i < buffer_length)
        {
            if (buffer[i] == '\n')
            {
                line_end = i;
                break;
            }
            
            i++;
            line_end = i;
        }
        
        error_message += std::string(buffer + line_start, buffer + line_end);
        error_message += "\n" + std::string(column == 0 ? 0 : column - 1, ' ') + '^';
        
        return error_message;
    }
};

class parser
{
public:
    parser() : buffer_(nullptr), buffer_position_(0), line_(1), column_(0), root_(nullptr), popped_container_(false), surrogate_pair_({ 0, 0 }), unicode_nibble_(0), state_(state::normal)
    {
    }
    
	~parser() = default;

	void parse_stream(std::istream &stream, value &result)
	{
		result = value::invalid();
		root_ = &result;
		stack_.push_back(root_);

		if (stream.good())
		{
			char c;
			bool still_parsing;

			do
			{
				stream.get(c);
				still_parsing = scan(c);
			} while (still_parsing && stream);
		}
	}
    
    void parse_string(const char *string, std::size_t string_length, value &result);
    
    
	void parse_string(const std::string &string, value &result)
    {
        parser p;
        
        p.set_buffer(string.data(), string.size());
        p.parse_next(result);
        
        if (result.is_valid())
        {
            value next;
            p.parse_next(next);
            
            if (next.is_valid())
            {
                throw p.make_error("found multiple top-level JSON values");
            }
            
            if (p.buffer_length_ > p.buffer_position_)
            {
                throw p.make_error("found invalid trailing characters in JSON string");
            }
            
            return;
        }
        
        throw p.make_error("bad JSON");
    }
    
    void parse_file(const std::string &filename, value &result)
    {
        std::fstream file(filename, std::ios::in);
        std::stringstream ss;
        ss << file.rdbuf();
        parse_string(ss.str(), result);
    }

private:
	void parse_next(value &result)
    {
        result = value::invalid();
        root_ = &result;
        stack_.push_back(root_);
        
        if (buffer_position_ < buffer_length_)
        {
            char c;
            bool still_parsing;
            
            do
            {
                c = buffer_[buffer_position_++];
                still_parsing = scan(c);
            } while (still_parsing);
        }
    }
    
	void set_buffer(const char *buffer, std::size_t buffer_length)
    {
        buffer_ = buffer;
        buffer_length_ = buffer_length;
        buffer_position_ = 0;
    }
    
	void handle_structural_token(char c)
    {
        switch (c)
        {
            case '[':
            {
                if (stack_.size() == 1 && !root_->is_valid())
                {
                    *root_ = value::array();
                }
                else
                {
		    push(value::array().copy());
                }
                
                popped_container_ = false;
                break;
            }
            case ']':
            {
                if (!top().is_array())
                {
                    throw make_error("unmatched ]");
                }
                
                if (!token_string_.empty())
                {
                    top().emplace_array(parse_token());
                }
                
                stack_.pop_back();
                popped_container_ = true;
                break;
            }
            case '{':
            {
                if (stack_.size() == 1 && !root_->is_valid())
                {
                    *root_ = value::object();
                }
                else
                {
		    push(value::object().copy());
                }
                
                popped_container_ = false;
                break;
            }
            case '}':
            {
                if (!top().is_object())
                {
                    throw make_error("unmatched }");
                }
                
                if (!token_string_.empty())
                {
                    if (!key_.first)
                    {
                        throw make_error("Objects must consist of key:value pairs");
                    }
                    
		    push(parse_token());
                }
                else if (key_.first)
                {
                    throw std::runtime_error("missing value after :");
                }
                
                stack_.pop_back();
                popped_container_ = true;
                break;
            }
            case ',':
            {
                if (!token_string_.empty())
                {
		    push(parse_token());
                }
                else if (!popped_container_)
                {
                    throw make_error("Expected value before ','");
                }
                
                popped_container_ = false;
                break;
            }
            case ':':
            {
                if (stack_.empty() || !top().is_object())
                {
                    throw make_error(": not part of an object");
                }
                
                key_.first = true;
                key_.second = parse_string();
                
                popped_container_ = false;
                break;
            }
        }
    }
    
    value parse_token()
    {
        switch (token_string_[0])
        {
            case '"':
                return value(parse_string());
            case 't':
            case 'f':
            case 'n':
                return parse_literal();
        }
        
        return parse_number();
    }
    
	value parse_number()
    {
        try
        {
            double number = std::stod(token_string_);
            token_string_ = "";
            return value(number);
        }
        catch (std::invalid_argument)
        {
            throw make_error("expected a number, found \"" + token_string_ + "\"");
        }
    }
    
	value parse_literal()
    {
        if (token_string_[0] == 't')
        {
            if (token_string_ != "true")
            {
                throw make_error("expected true");
            }
            
            token_string_ = "";
            return value(true);
        }
        
        if (token_string_[0] == 'f')
        {
            if (token_string_ != "false")
            {
                throw make_error("expected false");
            }
            
            token_string_ = "";
            return value(false);
        }
        
        if (token_string_[0] == 'n')
        {
            if (token_string_ != "null")
            {
                throw make_error("expected null");
            }
            
            token_string_ = "";
            return value::null();
        }
        
        throw make_error("expected one of: [true, false, null]");
    }
    
	std::string parse_string()
    {
        if (token_string_.front() != '"' || token_string_.back() != '"')
        {
            throw std::runtime_error("expected string surrounded in quotes, found " + token_string_);
        }
        
        std::string string(token_string_.substr(1, token_string_.size() - 2));
        token_string_ = "";
        
        return string;
    }
    
	bool scan(char c)
    {
        if (c == '\0')
        {
            return false;
        }
        
        column_++;
        
        if (c == '\n')
        {
            line_++;
            column_ = 0;
        }
        
        if (state_ == state::normal)
        {
            switch (classify(c))
            {
                case character_class::literal:
                    token_string_.append(1, c);
                    break;
                case character_class::whitespace:
                    break;
                case character_class::quote:
                    token_string_.append(1, '"');
                    state_ = state::string;
                    break;
                case character_class::structure:
                    handle_structural_token(c);
                    break;
                case character_class::invalid:
                    throw make_error("Invalid character");
            }
        }
        else if (state_ == state::string)
        {
            if (c == '\\' && state_ != state::string_escape)
            {
                state_ = state::string_escape;
            }
            else if (c == '"')
            {
                token_string_.append(1, '"');
                
                if (state_ == state::string_escape)
                {
                    state_ = state::string;
                }
                else
                {
                    state_ = state::normal;
                }
            }
            else
            {
                if (unicode_nibble_ > 0)
                {
                    if (unicode_nibble_ == 4)
                    {
                        if (surrogate_pair_.first <= 0x7F)
                        {
                            token_string_.append(1, surrogate_pair_.first & 0xFF);
                        }
                        else
                        {
                            
                        }
                    }
                    
                    surrogate_pair_ = { 0, 0 };
                    unicode_nibble_ = 0;
                }
                
                unsigned char u = c;
                
                if(u <= 0x7F)
                {
                    token_string_.append(1, c);
                }
                else
                {
                    token_string_.append(1, 0xC0 + (u >> 6));
                    token_string_.append(1, 0x80 + (u & 0x3F));
                }
                
                state_ = state::string;
            }
        }
        else if (state_ == state::string_escape)
        {
            if (c == 'u')
            {
                state_ = state::unicode;
            }
            else
            {
                if (c == 'n')
                {
                    token_string_.append(1, '\n');
                }
                else if (c == 'r')
                {
                    token_string_.append(1, '\r');
                }
                else if (c == '\\')
                {
                    token_string_.append(1, '\\');
                }
                else if (c == '"')
                {
                    token_string_.append(1, '"');
                }
                else if (c == '/')
                {
                    token_string_.append(1, '/');
                }
                else if (c == 'f')
                {
                    token_string_.append(1, '\f');
                }
                else if (c == 'b')
                {
                    token_string_.append(1, '\b');
                }
                else if (c == 't')
                {
                    token_string_.append(1, '\t');
                }
                else
                {
                    throw std::runtime_error("");
                }
                
                state_ = state::string;
            }
        }
        else if (state_ == state::unicode)
        {
            if (unicode_nibble_++ < 4)
            {
                surrogate_pair_.first <<= 8;
                surrogate_pair_.first += hex2int(c);
            }
            else
            {
                surrogate_pair_.second <<= 8;
                surrogate_pair_.second += hex2int(c);
            }
            
            if (unicode_nibble_ % 4 == 0)
            {
                state_ = state::string;
            }
        }
        
        if (stack_.empty())
        {
            return false;
        }
        
        return true;
    }
    
	parse_error make_error(const std::string &message)
    {
        return parse_error(line_, column_, buffer_, buffer_length_, message);
    }
    
	void push(value &&v)
    {
        if (stack_.empty())
        {
            throw make_error("stack empty");
        }
        
        if (!top().is_container())
        {
            throw make_error("not container");
        }
        
        value *inserted = nullptr;
        
        if (top().is_object())
        {
            if (!key_.first)
            {
                throw make_error("missing key for value");
            }
            
            auto inserted_iter = top().emplace_object(std::move(key_.second), std::move(v)).first;
            inserted = &inserted_iter->second;
            key_.first = false;
            key_.second.clear();
        }
        else if (top().is_array())
        {
            inserted = &*top().emplace_array(std::move(v));
        }
        else
        {
            throw std::runtime_error("top of stack not container");
        }
        
        if (inserted == nullptr)
        {
            throw "";
        }
        
        if (inserted->is_container())
        {
            stack_.push_back(inserted);
        }
    }
    
	value &top() { return *stack_.back(); }

	const char *buffer_;
	std::size_t buffer_length_;
	std::size_t buffer_position_;
	std::vector<value *> stack_;
	std::size_t line_;
	std::size_t column_;
	std::string token_string_;
	value *root_;
	std::pair<bool, std::string> key_;
	bool popped_container_;
	std::pair<uint32_t, uint32_t> surrogate_pair_;
	std::size_t unicode_nibble_;

	enum class state
	{
		normal,
		string,
		string_escape,
		unicode
	};

	state state_;
};

}
