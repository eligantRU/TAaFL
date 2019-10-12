#pragma once

template <class T>
class Matrix
{
public:
	Matrix(size_t rows, size_t columns, const T & defVal)
		:m_rows(rows)
		,m_columns(columns)
		,m_data(rows * columns, defVal)
	{
	}

	T & operator()(size_t row, size_t column)
	{
		return m_data[GetCellIndex(row, column)];
	}

	const T & operator()(size_t row, size_t column) const
	{
		return m_data[GetCellIndex(row, column)];
	}

	size_t GetRowCount() const noexcept
	{
		return m_rows;
	}

	size_t GetColumnCount() const noexcept
	{
		return m_columns;
	}

private:
	size_t GetCellIndex(size_t row, size_t column) const noexcept
	{
		return (row * m_columns + column);
	}

	std::vector<T> m_data;
	size_t m_rows;
	size_t m_columns;
};
