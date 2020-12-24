// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		template <typename TSeq, typename TSelector>
		struct SelectImpl
		{
			using SourceIter_t = decltype(std::begin(std::declval<TSeq>()));
			using SourceEnd_t = decltype(std::end(std::declval<TSeq>()));

			struct Iter_t
			{
				bool operator!=(const SourceEnd_t& end) const { return sourceIter != end; }
				auto operator*() { return std::invoke(selector, *sourceIter); }
				void operator++() { ++sourceIter; }

				Iter_t(TSeq& source, TSelector& inSelector)
					: sourceIter(std::begin(source))
					, sourceEnd(std::end(source))
					, selector(inSelector)
				{
				}

				SourceIter_t sourceIter;
				SourceEnd_t sourceEnd;
				TSelector& selector;
			};

			SelectImpl(TSeq& inSource, TSelector&& inSelector)
				: source(inSource)
				, selector(std::forward<TSelector>(inSelector))
			{
			}

			Iter_t begin() { return Iter_t(source, selector); }
			SourceEnd_t end() const { return std::end(source); }

			TSeq& source;
			TSelector selector;
		};

		template <typename TSelector>
		struct Select_t
		{
			explicit Select_t(TSelector&& inSelector)
				: selector(std::forward<TSelector>(inSelector))
			{
			}

			TSelector selector;
		};
	}

	template <typename TSelector>
	auto Select(TSelector&& selector)
	{
		return GitersImpl::Select_t<TSelector>(std::forward<TSelector>(selector));
	}

	template <typename TSeq, typename TSelector>
	auto operator|(TSeq&& source, GitersImpl::Select_t<TSelector>&& selector)
	{
		return GitersImpl::SelectImpl<TSeq, TSelector>(source, std::forward<TSelector>(selector.selector));
	}
}
