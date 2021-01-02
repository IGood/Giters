// Copyright © Ian Good

#pragma once

#include <vector>

namespace Giters
{
	namespace GitersImpl
	{
		struct ToVector_t
		{
			size_t initialCapacity;
		};

		template <typename TSelector>
		struct ToVectorSelect_t
		{
			explicit ToVectorSelect_t(size_t inInitialCapacity, TSelector&& inSelector)
				: initialCapacity(inInitialCapacity)
				, selector(std::forward<TSelector>(inSelector))
			{
			}

			size_t initialCapacity;
			TSelector selector;
		};
	}

	inline auto ToVector(size_t initialCapacity = 0)
	{
		return GitersImpl::ToVector_t{ initialCapacity };
	}

	template <typename TSelector>
	auto ToVector(TSelector&& selector)
	{
		return GitersImpl::ToVectorSelect_t<TSelector>(0, std::forward<TSelector>(selector));
	}

	template <typename TSelector>
	auto ToVector(size_t initialCapacity, TSelector&& selector)
	{
		return GitersImpl::ToVectorSelect_t<TSelector>(initialCapacity, std::forward<TSelector>(selector));
	}
}

template <typename TSeq>
auto operator|(TSeq&& source, Giters::GitersImpl::ToVector_t&& toVector)
{
	using Elem_t = std::remove_reference_t<decltype(*std::begin(source))>;

	std::vector<Elem_t> vector;
	vector.reserve(toVector.initialCapacity);

	for (auto&& elem : source)
	{
		vector.push_back(elem);
	}

	return vector;
}

template <typename TSeq, typename TSelector>
auto operator|(TSeq&& source, Giters::GitersImpl::ToVectorSelect_t<TSelector>&& selector)
{
	using Elem_t = std::remove_const_t<std::remove_reference_t<decltype(std::invoke(selector.selector, *std::begin(source)))>>;

	std::vector<Elem_t> vector;
	vector.reserve(selector.initialCapacity);

	for (auto&& elem : source)
	{
		vector.push_back(std::invoke(selector.selector, elem));
	}

	return vector;
}
