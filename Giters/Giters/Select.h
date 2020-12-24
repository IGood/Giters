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

				Iter_t(TSeq& source, TSelector&& inSelector)
					: sourceIter(std::begin(source))
					, sourceEnd(std::end(source))
					, selector(std::forward<TSelector>(inSelector))
				{
				}

				Iter_t(const Iter_t& other)
					: sourceIter(other.sourceIter)
					, sourceEnd(other.sourceEnd)
					, selector(std::forward<TSelector>(other.selector))
				{
				}

				SourceIter_t sourceIter;
				SourceEnd_t sourceEnd;
				TSelector&& selector;
			};

			SelectImpl(TSeq& source, TSelector&& selector)
				: iter(source, std::forward<TSelector>(selector))
			{
			}

			Iter_t begin() const { return iter; }
			SourceEnd_t end() const { return iter.sourceEnd; }

			Iter_t iter;
		};

		template <typename TSelector>
		struct Select_t
		{
			explicit Select_t(TSelector&& inSelector)
				: selector(std::forward<TSelector>(inSelector))
			{
			}

			TSelector&& selector;
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
