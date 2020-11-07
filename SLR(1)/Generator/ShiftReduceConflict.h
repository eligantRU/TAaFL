#include <stdexcept>

class ShiftReduceConflict
	:public std::domain_error
{
public:
	explicit ShiftReduceConflict(size_t ruleNum)
		:std::domain_error("Shift/Reduce conflict - not a SLR(1) grammar")
		,m_ruleNum(ruleNum)
	{
	}

	size_t ConflictRuleNum() const
	{
		return m_ruleNum;
	}

private:
	size_t m_ruleNum;
};
