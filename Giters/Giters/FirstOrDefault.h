// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		struct FirstOrDefault_t
		{
		};

		template <typename TPredicate>
		struct FirstOrDefaultWhere_t
		{
			explicit FirstOrDefaultWhere_t(TPredicate&& inPredicate)
				: predicate(std::forward<TPredicate>(inPredicate))
			{
			}

			TPredicate predicate;
		};
	}

	inline auto FirstOrDefault()
	{
		return GitersImpl::FirstOrDefault_t();
	}

	template <typename TSeq>
	auto operator|(TSeq&& source, GitersImpl::FirstOrDefault_t)
	{
		for (auto&& elem : source)
		{
			return elem;
		}

		using Elem_t = std::remove_reference_t<decltype(*std::begin(source))>;
		return Elem_t();
	}

	template <typename TPredicate>
	auto FirstOrDefault(TPredicate&& predicate)
	{
		return GitersImpl::FirstOrDefaultWhere_t<TPredicate>(std::forward<TPredicate>(predicate));
	}

	template <typename TSeq, typename TPredicate>
	auto operator|(TSeq&& source, GitersImpl::FirstOrDefaultWhere_t<TPredicate>&& predicate)
	{
		for (auto&& elem : source)
		{
			if (std::invoke(predicate.predicate, elem))
			{
				return elem;
			}
		}

		using Elem_t = std::remove_reference_t<decltype(*std::begin(source))>;
		return Elem_t();
	}
}
