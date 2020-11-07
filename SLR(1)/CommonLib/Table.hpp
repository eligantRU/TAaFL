#pragma once

#include <type_traits>
#include <functional>
#include <algorithm>
#include <optional>
#include <variant>
#include <iomanip>
#include <string>

#include "Settings.hpp"
#include "Common.hpp"

template <class T = void>
class Table
{
	static const auto COLUMN_OFFSET = 5;

public:
	static const auto EMPTY_CELL = '-';
	static const auto FILLER = ' ';

	using Row = std::vector<T>;

	Table(std::istream& strm, const std::vector<std::string>& legend, std::function<T(std::string)>&& converter) // TODO: cellPrinter
		:m_columnLegend(legend)
	{
		std::string line;
		while (std::getline(strm, line))
		{
			Row tableRow;
			const auto row = Tokenize(line, FILLER);
			for (const auto& cell : row)
			{
				const auto el = converter(cell);
				tableRow.push_back(el);
			}
			AddRow(tableRow);
		}
	}

	Table(std::vector<std::string> legend)
		:Table(legend, [](const auto& cell) {
			return std::visit([](auto&& arg) {
				return ToString(arg);
				}, cell);
			})
	{
	}

	Table(std::vector<std::string> legend, std::function<std::string(T)>&& cellPrinter)
		:m_columnLegend(legend)
		,m_cellPrinter(cellPrinter)
	{
	}

	void AddRow(Row row)
	{
		if (m_columnLegend.value_or(std::vector<std::string>{}).size() != row.size())
		{
			throw std::runtime_error("Row cannot contains more or less values than legend");
		}
		m_rows.emplace_back(row);
	}

	const Row& operator[](size_t i) const
	{
		return m_rows[i];
	}

	Row& operator[](size_t i)
	{
		return m_rows[i];
	}

	bool empty() const
	{
		return m_rows.size();
	}
	
	auto cbegin() const
	{
		return m_rows.cbegin();
	}
	
	auto cend() const
	{
		return m_rows.cend();
	}

	auto begin() const
	{
		return m_rows.begin();
	}
	
	auto end() const
	{
		return m_rows.end();
	}

	auto begin()
	{
		return m_rows.begin();
	}
	
	auto end()
	{
		return m_rows.end();
	}

	friend std::ostream& operator<<(std::ostream& strm, const Table<T>& table)
	{
		const auto lengths = table.CalcColumnLengths();
		if (table.m_columnLegend)
		{
			table.PrintRow(strm, *(table.m_columnLegend), lengths, [](const std::string& val) { return val; });
		}
		for (const auto& row : table.m_rows)
		{
			table.PrintRow(strm, row, lengths, std::bind(table.m_cellPrinter, std::placeholders::_1));
		}
		return strm;
	}

private:
	std::vector<size_t> CalcColumnLengths() const // TODO: can be optimized via mutable row with bool flag
	{
		std::vector<size_t> lengths(
			m_columnLegend
				? m_columnLegend->size()
				: (m_rows.empty() ? 0 : m_rows.front().size()),
			0);
		if (m_columnLegend)
		{
			for (size_t i = 0; i < m_columnLegend->size(); ++i)
			{
				lengths[i] = (*m_columnLegend)[i].size();
			}
		}

		for (const auto& row : m_rows)
		{
			std::vector<size_t> rowLengths(row.size(), 0);
			std::transform(row.cbegin(), row.cend(), rowLengths.begin(), [this](const auto& cell) {
				return m_cellPrinter(cell).size();
			});
			
			for (size_t i = 0; i < row.size(); ++i)
			{
				lengths[i] = std::max(lengths[i], rowLengths[i]);
			}
		}
		return lengths;
	}

	template<class E, class F>
	static void PrintRow(std::ostream& strm, const std::vector<E>& row, const std::vector<size_t>& lengths, F&& printStrategy)
	{		
		size_t i = 0;
		for (const auto& el : row)
		{
			if constexpr (Settings::USE_PRETTY_TABLE)
			{
				strm << std::setfill(FILLER) << std::setw(lengths[i] + (i ? COLUMN_OFFSET : 0)) << printStrategy(el);
			}
			else
			{
				strm << (i ? std::string(1, FILLER) : "") << printStrategy(el);
			}
			++i;
		}
		strm << std::endl;
	}

	std::optional<std::vector<std::string>> m_columnLegend; // TODO: not so optional now
	std::vector<Row> m_rows;

	std::function<std::string(T)> m_cellPrinter;
};
