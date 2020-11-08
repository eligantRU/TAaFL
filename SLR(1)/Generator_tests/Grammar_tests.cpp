#include <catch2/catch.hpp>

#include "../CommonLib/Common.hpp"

#include "../Generator/Grammar.h"
#include "../Generator/GrammarImpl.h"

TEST_CASE("Non-terminal detection", "[IsNonTerminal]")
{
	REQUIRE_FALSE(IsNonTerminal(""));
	REQUIRE_FALSE(IsNonTerminal("A"));
	REQUIRE_FALSE(IsNonTerminal("ABC"));
	REQUIRE_FALSE(IsNonTerminal("<ABC"));
	REQUIRE_FALSE(IsNonTerminal("ABC>"));
	REQUIRE_FALSE(IsNonTerminal("a"));
	REQUIRE_FALSE(IsNonTerminal(TERMINAL_END_SEQUENCE));
	REQUIRE_FALSE(IsNonTerminal(NONTERMINAL_END_SEQUENCE));
	REQUIRE(IsNonTerminal("<ABC>"));
}

TEST_CASE("Empty-rule detection", "[IsEmptyRule]")
{
	REQUIRE_FALSE(IsEmptyRule(""));
	REQUIRE_FALSE(IsEmptyRule("<>"));
	REQUIRE_FALSE(IsEmptyRule("<A>"));
	REQUIRE_FALSE(IsEmptyRule("a"));
	REQUIRE_FALSE(IsEmptyRule(TERMINAL_END_SEQUENCE));
	REQUIRE(IsEmptyRule(NONTERMINAL_END_SEQUENCE));
}

TEST_CASE("End-rule detection", "[IsEndRule]")
{
	REQUIRE_FALSE(IsEndRule(""));
	REQUIRE_FALSE(IsEndRule("<>"));
	REQUIRE_FALSE(IsEndRule("<A>"));
	REQUIRE_FALSE(IsEndRule("a"));
	REQUIRE_FALSE(IsEndRule(NONTERMINAL_END_SEQUENCE));
	REQUIRE(IsEndRule(TERMINAL_END_SEQUENCE));
}

SCENARIO("Get rules from istream", "[GetRules]" )
{
	SECTION("BLA")
	{
		std::stringstream strm(
			"\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"\n"
			"<A> => a\n"
			"<B> => b\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"<A> => a\n"
			"\n"
			"<B> => b\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"<A> => a\n"
			"<B> => b\n"
			"\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"<A> =>\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"a => a\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"<A> a\n"
		);
		REQUIRE_THROWS_AS(Impl::GetRules(strm), std::logic_error);
	}

	SECTION("BLA")
	{
		std::stringstream strm(
			"<A> => <B> a\n"
			"<B> => b\n"
		);
		const auto [rules, nonTerminals, terminals] = Impl::GetRules(strm);
		REQUIRE(rules ==  decltype(rules){
			{ "<A>", { "<B>", "a" } },
			{ "<B>", { "b" } } }
		);
		REQUIRE(nonTerminals == decltype(nonTerminals){ "<A>", "<B>" });
		REQUIRE(terminals == decltype(terminals){ "a", "b" });
	}
}

SCENARIO("Validate rules", "[ValidateRules]")
{
	SECTION("Empty-rules cannot contains another symbols")
	{
		REQUIRE_THROWS_AS(Impl::ValidateRules({ { "<A>", { "e", "<A>" } } }, {}), std::logic_error);
	}

	SECTION("Rules cannot contains end-rule terminal (in raw input)")
	{
		REQUIRE_THROWS_AS(Impl::ValidateRules({ { "<A>", { TERMINAL_END_SEQUENCE } } }, {}), std::logic_error);
		REQUIRE_THROWS_AS(Impl::ValidateRules({ { "<A>", { "a", TERMINAL_END_SEQUENCE } } }, {}), std::logic_error);
	}

	SECTION("Rules cannot contains unknown non-terminals")
	{
		REQUIRE_THROWS_AS(Impl::ValidateRules({
			{ "<A>", { "<B> <C>" } },
			{ "<B>", { "b" } } },
			{ "<A>", "<B>" }), std::logic_error
		);
	}

	SECTION("Non-terminal cannot starts with reserved prefix")
	{
		{
			const auto genNonTerminal = "<" + ToString(NONTERMINAL_GEN_PREFIX) + "A>";
			REQUIRE_THROWS_AS(Impl::ValidateRules({ { genNonTerminal, { "a" } } }, { genNonTerminal }), std::logic_error);
		}
		{
			const auto genNonTerminal = "<" + ToString(NONTERMINAL_GEN_PREFIX) + "A>";
			REQUIRE_THROWS_AS(Impl::ValidateRules({ { "<A>", { genNonTerminal } } }, { "<A>", genNonTerminal }), std::logic_error);
		}
	}
}

SCENARIO("Grammar validation", "[ValidateGrammar]")
{
	SECTION("Grammar cannot be empty")
	{
		REQUIRE_NOTHROW(Impl::ValidateGrammar({ { "<A>", { "a" } } }));
		REQUIRE_THROWS_AS(Impl::ValidateGrammar({}), std::logic_error);
	}
}

SCENARIO("Replenished grammar recognition", "[IsReplenishedGrammar]")
{
	REQUIRE(Impl::IsReplenishedGrammar({
		{ "<A>", { "<B>", "<C>" } },
		{ "<B>", { "b" } },
		{ "<C>", { "C" } },
	}));

	REQUIRE(Impl::IsReplenishedGrammar({
		{ "<A>", { "<B>", "<C>" } },
		{ "<A>", { "<C>", "<B>" } },
		{ "<B>", { "b" } },
		{ "<C>", { "C" } },
	}));

	REQUIRE_FALSE(Impl::IsReplenishedGrammar({
		{ "<A>", { "<A>", "<B>", "<C>" } },
		{ "<A>", { "a" } },
		{ "<B>", { "b" } },
		{ "<C>", { "C" } },
	}));
}

SCENARIO("Replenished grammar", "[MakeReplenished]")
{
	GIVEN("Replenished grammar")
	{
		const Grammar grammar = {
			{ "<A>", { "<B>", "<C>" } },
			{ "<A>", { "<C>", "<B>" } },
			{ "<B>", { "b" } },
			{ "<C>", { "C" } },
		};
		const NonTerminalsSet nonTerminals = { "<A>", "<B>", "<C>" };
		const TerminalsSet terminals = { "b", "c" };

		WHEN("MakeReplenished")
		{
			const auto [resGrammar, resNonTerminals, resTerminals] = Impl::MakeReplenished(grammar, nonTerminals, terminals);

			THEN("Axiom ending with end-rule terminal, others are same")
			{
				REQUIRE(resGrammar == decltype(grammar){
					{ "<A>", { "<B>", "<C>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { "<C>", "<B>", TERMINAL_END_SEQUENCE } },
					{ "<B>", { "b" } },
					{ "<C>", { "C" } },
				});
			}
			THEN("Nothing happened with non-terminals")
			{
				REQUIRE(resNonTerminals == nonTerminals);
			}
			THEN("End-rule terminal added to terminals")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.insert(TERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}

	GIVEN("Grammar with epsables")
	{
		const Grammar grammar = {
			{ "<A>", { "<B>", "a" } },
			{ "<A>", { NONTERMINAL_END_SEQUENCE } },
			{ "<B>", { "b" } },
			{ "<B>", { NONTERMINAL_END_SEQUENCE } },
		};
		const NonTerminalsSet nonTerminals = { "<A>", "<B>" };
		const TerminalsSet terminals = { "a", "b", NONTERMINAL_END_SEQUENCE };

		WHEN("RemoveEmptyRules")
		{
			const auto [resGrammar, resNonTerminals, resTerminals] = Impl::MakeReplenished(grammar, nonTerminals, terminals);

			THEN("Grammar has no empty-rules")
			{
				REQUIRE(resGrammar == decltype(grammar){
					{ "<A>", { "<B>", "a", TERMINAL_END_SEQUENCE } },
					{ "<A>", { TERMINAL_END_SEQUENCE } },
					{ "<B>", { "b" } },
					{ "<B>", { NONTERMINAL_END_SEQUENCE } },
				});
			}
			THEN("Nothing happened with non-terminals")
			{
				REQUIRE(resNonTerminals == nonTerminals);
			}
			THEN("Terminals has no empty-rule terminal & has end-rule terminal")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.insert(TERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}

	GIVEN("Unreplenished grammar")
	{
		const Grammar grammar = {
			{ "<A>", { "<A>", "<B>", "<C>" } },
			{ "<A>", { "<C>", "<B>" } },
			{ "<B>", { "b" } },
			{ "<C>", { "C" } },
		};
		const NonTerminalsSet nonTerminals = { "<A>", "<B>", "<C>" };
		const TerminalsSet terminals = { "b", "c" };

		WHEN("MakeReplenished")
		{
			const auto [resGrammar, resNonTerminals, resTerminals] = Impl::MakeReplenished(grammar, nonTerminals, terminals);

			THEN("Added new rules in grammar")
			{
				REQUIRE(resGrammar.size() == (grammar.size() + 1));
				REQUIRE(std::find(grammar.cbegin(), grammar.cend(), *(resGrammar.cbegin())) == grammar.cend());
			}
			THEN("Added new non-terminal")
			{
				REQUIRE(resNonTerminals.size() == (nonTerminals.size() + 1));
				REQUIRE(std::find_if(resNonTerminals.cbegin(), resNonTerminals.cend(), [](const auto& nonTerminal) {
					return nonTerminal.substr(1, strlen(NONTERMINAL_GEN_PREFIX)) == std::string(NONTERMINAL_GEN_PREFIX);
				}) != resNonTerminals.cend());
			}
			THEN("New non-terminal nether exists in right sides of rules")
			{
				const auto surrogateNonTerminal = *std::find_if(resNonTerminals.cbegin(), resNonTerminals.cend(), [](const auto& nonTerminal) {
					return nonTerminal.substr(1, strlen(NONTERMINAL_GEN_PREFIX)) == std::string(NONTERMINAL_GEN_PREFIX);
				});
				bool contains = false;
				for (const auto& [left, right] : resGrammar)
				{
					if (std::find(right.cbegin(), right.cend(), surrogateNonTerminal) != right.cend())
					{
						contains = true;
					}
				}
				REQUIRE_FALSE(contains);
			}
			THEN("End-rule terminal added to terminals")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.insert(TERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}
}

SCENARIO("Empty-rules removing", "[RemoveEmptyRules]")
{
	GIVEN("Grammar with no end-rule terminals")
	{
		const Grammar grammar = {
			{ "<A>", { "a" } },
		};
		const TerminalsSet terminals = { "a" };

		WHEN("RemoveEmptyRules")
		{
			REQUIRE_THROWS_AS(Impl::RemoveEmptyRules(grammar, terminals), std::logic_error);
		}
	}
	
	GIVEN("Grammar with no empty-rules")
	{
		const Grammar grammar = {
			{ "<A>", { "<A>", "<B>", "<C>", TERMINAL_END_SEQUENCE } },
			{ "<A>", { "<C>", "<B>", TERMINAL_END_SEQUENCE } },
			{ "<B>", { "b" } },
			{ "<C>", { "C" } },
		};
		const TerminalsSet terminals = { "b", "c", TERMINAL_END_SEQUENCE };

		WHEN("RemoveEmptyRules")
		{
			const auto [resGrammar, resTerminals] = Impl::RemoveEmptyRules(grammar, terminals);

			THEN("nothing happened with grammar")
			{
				REQUIRE(resGrammar == grammar);
			}
			THEN("nothing happened with terminals")
			{
				REQUIRE(resTerminals == terminals);
			}
		}
	}

	GIVEN("Grammar with empty-rules")
	{
		const Grammar grammar = {
			{ "<A>", { "<B>", TERMINAL_END_SEQUENCE } },
			{ "<B>", { "b" } },
			{ "<B>", { NONTERMINAL_END_SEQUENCE } },
		};
		const TerminalsSet terminals = { "b", NONTERMINAL_END_SEQUENCE };

		WHEN("RemoveEmptyRules")
		{
			const auto [resGrammar, resTerminals] = Impl::RemoveEmptyRules(grammar, terminals);

			THEN("Grammar has no empty-rules")
			{
				REQUIRE(resGrammar == decltype(grammar){
					{ "<A>", { "<B>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { TERMINAL_END_SEQUENCE } },
					{ "<B>", { "b" } },
				});
			}
			THEN("Terminals lost empty-rule terminal")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.erase(NONTERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}

	GIVEN("Grammar with 2+ epsable-non-terminals in a row")
	{
		const Grammar grammar = {
			{ "<A>", { "<B>", "<C>", TERMINAL_END_SEQUENCE } },
			{ "<B>", { "b" } },
			{ "<B>", { NONTERMINAL_END_SEQUENCE } },
			{ "<C>", { "c" } },
			{ "<C>", { NONTERMINAL_END_SEQUENCE } },
		};
		const TerminalsSet terminals = { "b", "c", NONTERMINAL_END_SEQUENCE };

		WHEN("RemoveEmptyRules")
		{
			const auto [resGrammar, resTerminals] = Impl::RemoveEmptyRules(grammar, terminals);

			THEN("Grammar has no empty-rules")
			{
				REQUIRE(resGrammar == decltype(grammar){
					{ "<A>", { "<B>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { TERMINAL_END_SEQUENCE } },
					{ "<A>", { "<B>", "<C>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { "<C>", TERMINAL_END_SEQUENCE } },
					{ "<B>", { "b" } },
					{ "<C>", { "c" } },
				});
			}
			THEN("Terminals has no empty-rule terminal")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.erase(NONTERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}

	GIVEN("Grammar with tiered epsable-non-terminals")
	{
		const Grammar grammar = {
			{ "<A>", { "<B>", "<C>", TERMINAL_END_SEQUENCE } },
			{ "<B>", { "<C>" } },
			{ "<B>", { "b" } },
			{ "<C>", { "c" } },
			{ "<C>", { NONTERMINAL_END_SEQUENCE } },
		};
		const TerminalsSet terminals = { "b", "c", NONTERMINAL_END_SEQUENCE };

		WHEN("RemoveEmptyRules")
		{
			const auto [resGrammar, resTerminals] = Impl::RemoveEmptyRules(grammar, terminals);

			THEN("Grammar has no empty-rules")
			{
				REQUIRE(resGrammar == decltype(grammar){
					{ "<A>", { "<B>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { TERMINAL_END_SEQUENCE } },
					{ "<A>", { "<B>", "<C>", TERMINAL_END_SEQUENCE } },
					{ "<A>", { "<C>", TERMINAL_END_SEQUENCE } },
					{ "<B>", { "<C>" } },
					{ "<B>", { "b" } },
					{ "<C>", { "c" } },
				});
			}
			THEN("Terminals has no empty-rule terminal")
			{
				auto terminalsCopy(terminals);
				terminalsCopy.erase(NONTERMINAL_END_SEQUENCE);

				REQUIRE(resTerminals == terminalsCopy);
			}
		}
	}
}

TEST_CASE("Get grammar from stream", "[GetGrammar]")
{
	std::stringstream strm(
		"<A> => <B> <C>\n"
		"<B> => b\n"
		"<B> => " + std::string(NONTERMINAL_END_SEQUENCE) + "\n"
		"<B> => <C>\n"
		"<C> => c\n"
		"<C> => " + std::string(NONTERMINAL_END_SEQUENCE) + "\n"
	);

	const auto grammar = GetGrammar(strm);

	SECTION("Returns replenished grammar")
	{
		REQUIRE(Impl::IsReplenishedGrammar(grammar));
	}

	SECTION("Returns with axiom end-rule ending")
	{
		bool result = false;
		for (const auto& [left, right] : grammar)
		{
			if (left == grammar.front().left)
			{
				if (right.back() != TERMINAL_END_SEQUENCE)
				{
					result = true;
				}
			}
		}
		REQUIRE_FALSE(result);
	}
}
