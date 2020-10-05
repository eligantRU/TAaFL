#pragma once

#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <optional>
#include <istream>
#include <string>
#include <regex>

enum class LexemeType
{
	VoidType,
	StringType,
	DoubleType,
	IntType,
	BoolType,

	MainIdentifier,
	PrintIdentifier,
	ReadIdentifier,
	Identifier,

	WhileKeyWord,
	BreakKeyWord,
	ContinueKeyWord,
	IfKeyWord,
	ElseKeyWord,
	ReturnKeyWord,

	LeftSquareBracket,
	RightSquareBracket,
	LeftCurlyBracket,
	RightCurlyBracket,
	OpenParenthesis,
	CloseParenthesis,
	
	PlusOperator,
	MinusOperator,
	MultOperator,
	DivOperator,

	EqualOperator,
	ConjunctionOperator,
	DisjunctionOperator,
	LessOperator,
	GreaterOperator,
	NotOperator,

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
	size_t lineNum;
	size_t linePos;
};

namespace
{
	
const std::unordered_set<char> IGNORED_CHARS = { ' ', '\t', '\n' };
const std::unordered_set<char> SEPARATORS = {
	' ', '\t', '\n', ';', ',', '{', '}', '(', ')', '[', ']', '=', '<', '>', '!', '/', '*', '+', '-', '"', '#'
};

bool IsNumber(const std::string & lexeme)
{
	return std::regex_match(lexeme, std::regex("[-+]?\\d+"))
		|| std::regex_match(lexeme, std::regex("[-+]?0b[01]+"))
		|| std::regex_match(lexeme, std::regex("[-+]?0x[0-9A-F]+"))
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
	if (lexeme == "void") return LexemeType::VoidType;
	if (lexeme == "string") return LexemeType::StringType;
	if (lexeme == "double") return LexemeType::DoubleType;
	if (lexeme == "int") return LexemeType::IntType;
	if (lexeme == "bool") return LexemeType::BoolType;

	if (lexeme == "main") return LexemeType::MainIdentifier;
	if (lexeme == "print") return LexemeType::PrintIdentifier;
	if (lexeme == "read") return LexemeType::ReadIdentifier;

	if (lexeme == "+") return LexemeType::PlusOperator;
	if (lexeme == "-") return LexemeType::MinusOperator;
	if (lexeme == "*") return LexemeType::MultOperator;
	if (lexeme == "/") return LexemeType::DivOperator;
	
	if (lexeme == "==") return LexemeType::EqualOperator;
	if (lexeme == "&&") return LexemeType::ConjunctionOperator;
	if (lexeme == "||") return LexemeType::DisjunctionOperator;
	if (lexeme == "<") return LexemeType::LessOperator;
	if (lexeme == ">") return LexemeType::GreaterOperator;
	if (lexeme == "!") return LexemeType::NotOperator;

	if (lexeme == "while") return LexemeType::WhileKeyWord;
	if (lexeme == "break") return LexemeType::BreakKeyWord;
	if (lexeme == "continue") return LexemeType::ContinueKeyWord;
	if (lexeme == "if") return LexemeType::IfKeyWord;
	if (lexeme == "else") return LexemeType::ElseKeyWord;
	if (lexeme == "return") return LexemeType::ReturnKeyWord;

	if (lexeme == "=") return LexemeType::AssignmentOperator;
	if (lexeme == "(") return LexemeType::OpenParenthesis;
	if (lexeme == ")") return LexemeType::CloseParenthesis;
	if (lexeme == "{") return LexemeType::LeftCurlyBracket;
	if (lexeme == "}") return LexemeType::RightCurlyBracket;
	if (lexeme == "[") return LexemeType::LeftSquareBracket;
	if (lexeme == "]") return LexemeType::RightSquareBracket;
	if (lexeme == ";") return LexemeType::Semicolon;
	if (lexeme == ",") return LexemeType::Comma;
	if ((lexeme.size() > 1) && (lexeme.front() == '"') && (lexeme.back() == '"')) return LexemeType::StringValue;
	if ((lexeme == "true") || (lexeme == "false")) return LexemeType::BooleanValue;
	if (IsNumber(lexeme)) return LexemeType::NumberValue;
	if (IsIdentifier(lexeme)) return LexemeType::Identifier;
	return LexemeType::Error;
}

std::string LexemeTypeToString(LexemeType type)
{
	switch (type)
	{
	case LexemeType::VoidType:
		return "VoidType";
	case LexemeType::StringType:
		return "StringType";
	case LexemeType::DoubleType:
		return "DoubleType";
	case LexemeType::IntType:
		return "IntType";
	case LexemeType::BoolType:
		return "BoolType";

	case LexemeType::MainIdentifier:
		return "MainIdentifier";
	case LexemeType::PrintIdentifier:
		return "PrintIdentifier";
	case LexemeType::ReadIdentifier:
		return "ReadIdentifier";
	case LexemeType::Identifier:
		return "Identifier";

	case LexemeType::WhileKeyWord:
		return "WhileKeyWord"; 
	case LexemeType::BreakKeyWord:
		return "BreakKeyWord"; 
	case LexemeType::ContinueKeyWord:
		return "ContinueKeyWord"; 
	case LexemeType::IfKeyWord:
		return "IfKeyWord"; 
	case LexemeType::ElseKeyWord:
		return "ElseKeyWord"; 
	case LexemeType::ReturnKeyWord:
		return "ReturnKeyWord"; 

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

	case LexemeType::PlusOperator:
		return "PlusOperator";
	case LexemeType::MinusOperator:
		return "MinusOperator";
	case LexemeType::MultOperator:
		return "MultOperator";
	case LexemeType::DivOperator:
		return "DivOperator";

	case LexemeType::EqualOperator:
		return "EqualOperator";
	case LexemeType::ConjunctionOperator:
		return "ConjunctionOperator";
	case LexemeType::DisjunctionOperator:
		return "DisjunctionOperator";
	case LexemeType::LessOperator:
		return "LessOperator";
	case LexemeType::GreaterOperator:
		return "GreaterOperator";
	case LexemeType::NotOperator:
		return "NotOperator";

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
				return { ex.isControlled() ? LexemeType::EndOfFile : LexemeType::Error, "", m_currentLine, m_currentPos };
			}
		} while (lexeme.empty());
		return { ClassifyLexeme(lexeme), lexeme, m_currentLine, m_currentPos };
	}
	
private:
	std::string GetLexemeImpl()
	{
		SkipIgnored();
		if (m_strm.eof())
		{
			throw EndOfFileException();
		}

		char ch;
		std::string lexeme;
		auto wasIterated = false;
		while (!m_strm.eof() && GetNextChar(ch))
		{
			wasIterated = true;
			if (ch == '#')
			{
				ProcessHash();
				continue;
			}

			if (SEPARATORS.count(ch))
			{
				if (!lexeme.empty() || IGNORED_CHARS.count(ch))
				{
					break;
				}
				UpdateCurrentLine(ch);

				if (ch == '=')
				{
					return ProcessEqual();
				}
				if ((ch == '+') || (ch == '-'))
				{
					const auto nextCh = m_strm.peek();
					if (isdigit(nextCh) || (nextCh == '.'))
					{
						lexeme += ch;
						continue;
					}
				}
				if (ch == '"')
				{
					return ProcessString();
				}
				return std::string(1, ch);
			}
			lexeme += ch;
		}
		if (wasIterated)
		{
			PutCharBack();
		}
		return lexeme;
	}

	void SkipIgnored()
	{
		char ch;
		while (!m_strm.eof() && GetNextChar(ch))
		{
			UpdateCurrentLine(ch);
			if (!IGNORED_CHARS.count(ch))
			{
				PutCharBack();
				break;
			}
		}
	}

	std::string ProcessString()
	{
		std::string lexeme;

		char ch;
		while (!m_strm.eof() && GetNextChar(ch) && (ch != '"'))
		{
			UpdateCurrentLine(ch);
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
		if (!m_strm.eof())
		{
			char ch;
			if (GetNextChar(ch) && (ch == '='))
			{
				return "==";
			}
			else
			{
				PutCharBack();
				return "=";
			}
		}
		throw EndOfFileException();
	}

	void ProcessHash()
	{
		if (m_strm.eof())
		{
			throw EndOfFileException();
		}
		
		char ch;
		GetNextChar(ch);
		if (ch != '#')
		{
			while (ch != '\n')
			{
				GetNextChar(ch);
			}
			UpdateCurrentLine(ch);
			return;
		}
		else
		{
			char isLastHash = false;
			while (!m_strm.eof())
			{
				GetNextChar(ch);
				UpdateCurrentLine(ch);
				if (isLastHash && (ch == '#'))
				{
					return;
				}
				isLastHash = ch == '#';
			}
			throw EndOfFileException(false);
		}
	}

	void UpdateCurrentLine(char ch)
	{
		if (ch == '\n')
		{
			++m_currentLine;
			m_currentPos = 0;
		}
	}

	std::istream & GetNextChar(char & ch)
	{
		m_strm >> ch;
		++m_currentPos;
		return m_strm;
	}

	void PutCharBack()
	{
		m_strm.unget();
		if (m_currentPos != 0)
		{
			--m_currentPos;
		}
	}

	std::istream & m_strm;
	size_t m_currentLine = 1;
	size_t m_currentPos = 0;
};
