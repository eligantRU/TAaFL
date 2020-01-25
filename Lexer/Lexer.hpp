#pragma once

#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <optional>
#include <istream>
#include <string>
#include <regex>

// TODO: std::istream::unget - m_buffered_ch will be redundant
// TODO: line number with position in the line

enum class LexemeType
{
	DataType,
	ReservedIdentifier,
	KeyWord,
	Identifier,
	LeftSquareBracket,
	RightSquareBracket,
	LeftCurlyBracket,
	RightCurlyBracket,
	OpenParenthesis,
	CloseParenthesis,
	LogicalOperation,
	MathOperation,
	StringValue,
	NumberValue,
	BooleanValue,
	AssignmentOperator,
	Semicolon,
	Comma,
	EndOfFile,
	Error,
};

struct Lexeme
{
	LexemeType type;
	std::string lexeme;
};

namespace
{
	
const std::unordered_set<char> IGNORED_CHARS = { ' ', '\t' };
const std::unordered_set<char> SEPARATORS = {
	' ', '\t', ';', ',', '{', '}', '(', ')', '[', ']', '=', '<', '>', '!', '/', '*', '+', '-'
};

const std::unordered_set<std::string> DATA_TYPES = { "void", "string", "double", "int", "bool", "char" };
const std::unordered_set<std::string> RESERVED_IDENTIFIERS = { "main", "print", "read" };
const std::unordered_set<std::string> KEY_WORDS = { "while", "break", "continue", "if", "else", "return" };
const std::unordered_set<std::string> LOGICAL_OPERATIONS = { "==", "&&", "||", "<", ">", "!" };
const std::unordered_set<std::string> MATH_OPERATIONS = { "+", "-", "*", "/" };

bool IsNumber(const std::string & lexeme)
{
	return std::regex_match(lexeme, std::regex("[-+]?\\d+"))
		|| std::regex_match(lexeme, std::regex("[-+]?\\d+\\.\\d+"))
		|| std::regex_match(lexeme, std::regex("[-+]?\\.\\d+"))
		|| std::regex_match(lexeme, std::regex("[-+]?\\d+\\."));
}

bool IsIdentifier(const std::string & lexeme)
{
	return std::regex_match(lexeme, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"));
}

LexemeType ClassifyLexeme(const std::string & lexeme)
{
	if (DATA_TYPES.count(lexeme)) return LexemeType::DataType;
	if (RESERVED_IDENTIFIERS.count(lexeme)) return LexemeType::ReservedIdentifier;
	if (MATH_OPERATIONS.count(lexeme)) return LexemeType::MathOperation;
	if (LOGICAL_OPERATIONS.count(lexeme)) return LexemeType::LogicalOperation;
	if (KEY_WORDS.count(lexeme)) return LexemeType::KeyWord;
	if (lexeme == "=") return LexemeType::AssignmentOperator;
	if (lexeme == "(") return LexemeType::OpenParenthesis;
	if (lexeme == ")") return LexemeType::CloseParenthesis;
	if (lexeme == "{") return LexemeType::LeftCurlyBracket;
	if (lexeme == "}") return LexemeType::RightCurlyBracket;
	if (lexeme == "[") return LexemeType::LeftSquareBracket;
	if (lexeme == "]") return LexemeType::RightSquareBracket;
	if (lexeme == ";") return LexemeType::Semicolon;
	if (lexeme == ",") return LexemeType::Comma;
	if ((lexeme.front() == '"') && (lexeme.back() == '"')) return LexemeType::StringValue;
	if ((lexeme == "true") || (lexeme == "false")) return LexemeType::BooleanValue;
	if (IsNumber(lexeme)) return LexemeType::NumberValue;
	if (IsIdentifier(lexeme)) return LexemeType::Identifier;
	return LexemeType::Error;
}

std::string LexemeTypeToString(LexemeType type)
{
	switch (type)
	{
	case LexemeType::DataType:
		return "DataType";
	case LexemeType::ReservedIdentifier:
		return "ReservedIdentifier";
	case LexemeType::KeyWord:
		return "KeyWord";
	case LexemeType::Identifier:
		return "Identifier";
	case LexemeType::LeftSquareBracket:
		return "LeftSquareBracket";
	case LexemeType::RightSquareBracket:
		return "RightSquareBracket";
	case LexemeType::LeftCurlyBracket:
		return "LeftCurlyBracket";
	case LexemeType::RightCurlyBracket:
		return "RightCurlyBracket";
	case LexemeType::OpenParenthesis:
		return "OpenParenthesis";
	case LexemeType::CloseParenthesis:
		return "CloseParenthesis";
	case LexemeType::LogicalOperation:
		return "LogicalOperation";
	case LexemeType::MathOperation:
		return "MathOperation";
	case LexemeType::StringValue:
		return "StringValue";
	case LexemeType::NumberValue:
		return "NumberValue";
	case LexemeType::BooleanValue:
		return "BooleanValue";
	case LexemeType::AssignmentOperator:
		return "AssignmentOperator";
	case LexemeType::Semicolon:
		return "Semicolon";
	case LexemeType::Comma:
		return "Comma";
	case LexemeType::EndOfFile:
		return "EndOfFile";
	case LexemeType::Error:
	default:
		return "Error";
	}
}

class EndOfFileException
	:public std::exception
{
public:
	explicit EndOfFileException(bool isControlled = true)
		:std::exception()
		,m_isControlled(isControlled)
	{}

	bool isControlled() const
	{
		return m_isControlled;
	}

private:
	bool m_isControlled = false;
};

};

class Lexer
{
public:
	Lexer(std::istream & strm)
		:m_strm(strm)
	{
		m_strm >> std::noskipws;
	}

	Lexeme GetLexeme()
	{
		std::string lexeme;
		do
		{
			try
			{
				lexeme = GetLexemeImpl();
			}
			catch (const EndOfFileException & ex)
			{
				return { ex.isControlled() ? LexemeType::EndOfFile : LexemeType::Error, "" };
			}
		} while (lexeme.empty());
		return { ClassifyLexeme(lexeme), lexeme };
	}
	
private:
	std::string GetLexemeImpl()
	{
		if (m_strm.eof())
		{
			throw EndOfFileException();
		}

		if (m_buffered_ch)
		{
			const auto ch = *m_buffered_ch;
			m_buffered_ch = std::nullopt;
			if (ch == '=')
			{
				return ProcessEqual();
			}
			if (ch == '/')
			{
				if (const auto lexeme = ProcessSlash(); lexeme)
				{
					return *lexeme;
				}
				return "";
			}
			if ((ch == '+') || (ch == '-'))
			{
				m_mementoLexeme = std::string(1, ch);
				return "";
			}

			return std::string(1, ch);
		}

		std::string lexeme;
		if (m_mementoLexeme)
		{
			lexeme = *m_mementoLexeme;
			m_mementoLexeme.reset();
		}

		char ch;

		while (!m_strm.eof() && (m_strm >> ch) && !SEPARATORS.count(ch))
		{
			if (ch == '"')
			{
				return ProcessString();
			}
			if (ch == '\n' || IGNORED_CHARS.count(ch))
			{
				continue;
			}
			lexeme += ch;
		}

		m_buffered_ch = IGNORED_CHARS.count(ch) ? std::nullopt : std::optional<char>(ch);
		
		if (ch == '/')
		{
			m_mementoLexeme = lexeme;
			return "";
		}
		return lexeme;
	}

	std::string ProcessString()
	{
		std::string lexeme;

		char ch;
		while (!m_strm.eof() && (m_strm >> ch) && (ch != '"'))
		{
			lexeme += ch;
		}
		if (m_strm.eof())
		{
			throw EndOfFileException(false);
		}
		return '"' + lexeme + '"';
	}

	std::string ProcessEqual()
	{
		char ch;
		if (!m_strm.eof())
		{
			if ((m_strm >> ch) && (ch == '='))
			{
				return "==";
			}
			else
			{
				m_buffered_ch = IGNORED_CHARS.count(ch) ? std::nullopt : std::optional<char>(ch);
				return "=";
			}
		}
		throw EndOfFileException();
	}

	std::optional<std::string> ProcessSlash()
	{
		char ch;
		if (!m_strm.eof())
		{
			if ((m_strm >> ch) && ((ch == '/') || (ch == '*')))
			{
				if (ch == '/')
				{
					while (ch != '\n')
					{
						m_strm >> ch;
					}
					return std::nullopt;
				}
				if (ch == '*')
				{
					char isLastAsterisk = false;
					while (!m_strm.eof())
					{
						m_strm >> ch;
						if (ch == '/')
						{
							return std::nullopt;
						}
						isLastAsterisk = ch == '*';
					}
					throw EndOfFileException(false);
				}
			}
			else
			{
				m_buffered_ch = IGNORED_CHARS.count(ch) ? std::nullopt : std::optional<char>(ch);
				return "/";
			}
		}
		throw EndOfFileException();
	}

	std::istream & m_strm;
	std::optional<char> m_buffered_ch;
	std::optional<std::string> m_mementoLexeme;
};
