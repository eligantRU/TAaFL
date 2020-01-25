#pragma once

#include <unordered_set>
#include <iostream>
#include <optional>
#include <istream>
#include <string>

struct Lexeme
{
};

namespace
{
	
const std::unordered_set<char> IGNORED_CHARS = { ' ', '\t' };
const std::unordered_set<char> SEPARATORS = {
	' ', '\t', ';', ',', '{', '}', '(', ')', '[', ']', '=', '<', '>', '!', '/', '*', '+', '-'
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

	std::string GetLexeme()
	{
		std::string lexeme;
		do
		{
			lexeme = GetLexemeImpl();
		} while (lexeme.empty());
		return lexeme;
	}
	
private:
	std::string GetLexemeImpl()
	{
		if (m_strm.eof())
		{
			throw std::exception("EOF");
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
			throw std::exception("ERROR");
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
		throw std::exception("EOF");
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
					throw std::exception("ERROR");
				}
			}
			else
			{
				m_buffered_ch = IGNORED_CHARS.count(ch) ? std::nullopt : std::optional<char>(ch);
				return "/";
			}
		}
		throw std::exception("EOF");
	}

	std::istream & m_strm;
	std::optional<char> m_buffered_ch;
	std::optional<std::string> m_mementoLexeme;
};
