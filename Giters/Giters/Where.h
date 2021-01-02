// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		template <typename TSeq, typename TPredicate>
		struct WhereImpl
		{
			using SourceIter_t = decltype(std::begin(std::declval<TSeq>()));
			using SourceEnd_t = decltype(std::end(std::declval<TSeq>()));

			struct Iter_t
			{
				bool operator!=(const SourceEnd_t& end) const { return sourceIter != end; }
				auto operator*() { return *sourceIter; }
				void operator++()
				{
					do
					{
						++sourceIter;
					} while (sourceIter != sourceEnd && !std::invoke(predicate, *sourceIter));
				}

				Iter_t(TSeq& source, TPredicate& inPredicate)
					: sourceIter(std::begin(source))
					, sourceEnd(std::end(source))
					, predicate(inPredicate)
				{
					while (sourceIter != sourceEnd && !std::invoke(predicate, *sourceIter))
					{
						++sourceIter;
					}
				}

				SourceIter_t sourceIter;
				SourceEnd_t sourceEnd;
				TPredicate& predicate;
			};

			WhereImpl(TSeq& inSource, TPredicate&& inPredicate)
				: source(inSource)
				, predicate(std::forward<TPredicate>(inPredicate))
			{
			}

			Iter_t begin() { return Iter_t(source, predicate); }
			SourceEnd_t end() const { return std::end(source); }

			TSeq& source;
			TPredicate predicate;
		};

		template <typename TPredicate>
		struct Where_t
		{
			explicit Where_t(TPredicate&& inPredicate)
				: predicate(std::forward<TPredicate>(inPredicate))
			{
			}

			TPredicate predicate;
		};
	}

	template <typename TPredicate>
	auto Where(TPredicate&& predicate)
	{
		return GitersImpl::Where_t<TPredicate>(std::forward<TPredicate>(predicate));
	}
}

template <typename TSeq, typename TPredicate>
auto operator|(TSeq&& source, Giters::GitersImpl::Where_t<TPredicate>&& predicate)
{
	return Giters::GitersImpl::WhereImpl<TSeq, TPredicate>(source, std::forward<TPredicate>(predicate.predicate));
}
