#pragma once

#include <type_traits>
#include <functional>
#include <iomanip>
#include <string>

#include "Common.h"

template <class T>
class Table
{
public:
	using Row = std::vector<T>;

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

	Row& operator[](size_t i) const
	{
		return m_rows[i];
	}

	Row& operator[](size_t i)
	{
		return m_rows[i];
	}
	
	auto cbegin() const {
		return m_rows.cbegin();
	}
	
	auto cend() const {
		return m_rows.cend();
	}

	auto begin() const {
		return m_rows.begin();
	}
	
	auto end() const {
		return m_rows.end();
	}

	auto begin() {
		return m_rows.begin();
	}
	
	auto end() {
		return m_rows.end();
	}

	friend std::ostream& operator<<(std::ostream& strm, const Table<T>& table)
	{
		constexpr auto filler = ' ';
		constexpr auto columnOffset = 5;

		const auto lengths = table.CalcColumnLengths();
		if (table.m_columnLegend) // TODO: copy-paste
		{
			size_t i = 0;
			for (const auto& columnLegend : *(table.m_columnLegend))
			{
				strm << std::setfill(filler) << std::setw(lengths[i++] + columnOffset) << columnLegend;
			}
			strm << std::endl;
		}
		for (const auto& row : table.m_rows)
		{
			size_t i = 0;
			for (const auto& cell : row)
			{
				strm << std::setfill(filler) << std::setw(lengths[i++] + columnOffset) << table.m_cellPrinter(cell);
			}
			strm << std::endl;
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

	std::optional<std::vector<std::string>> m_columnLegend; // TODO: optional processing
	std::vector<Row> m_rows;

	std::function<std::string(T)> m_cellPrinter;
};
