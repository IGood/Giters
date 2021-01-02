// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		template <typename TSeq, typename TVisitor>
		struct VisitImpl
		{
			using SourceIter_t = decltype(std::begin(std::declval<TSeq>()));
			using SourceEnd_t = decltype(std::end(std::declval<TSeq>()));

			struct Iter_t
			{
				bool operator!=(const SourceEnd_t& end) const { return sourceIter != end; }
				auto operator*()
				{
					auto&& temp = *sourceIter;
					std::invoke(visitor, temp);
					return temp;
				}
				void operator++() { ++sourceIter; }

				Iter_t(TSeq& source, TVisitor&& inVisitor)
					: sourceIter(std::begin(source))
					, sourceEnd(std::end(source))
					, visitor(std::forward<TVisitor>(inVisitor))
				{
				}

				Iter_t(const Iter_t& other)
					: sourceIter(other.sourceIter)
					, sourceEnd(other.sourceEnd)
					, visitor(std::forward<TVisitor>(other.visitor))
				{
				}

				SourceIter_t sourceIter;
				SourceEnd_t sourceEnd;
				TVisitor&& visitor;
			};

			VisitImpl(TSeq& source, TVisitor&& visitor)
				: iter(source, std::forward<TVisitor>(visitor))
			{
			}

			Iter_t begin() const { return iter; }
			SourceEnd_t end() const { return iter.sourceEnd; }

			Iter_t iter;
		};

		template <typename TVisitor>
		struct Visit_t
		{
			explicit Visit_t(TVisitor&& inVisitor)
				: visitor(std::forward<TVisitor>(inVisitor))
			{
			}

			TVisitor&& visitor;
		};
	}

	template <typename TVisitor>
	auto Visit(TVisitor&& visitor)
	{
		return GitersImpl::Visit_t<TVisitor>(std::forward<TVisitor>(visitor));
	}
}

template <typename TSeq, typename TVisitor>
auto operator|(TSeq&& source, Giters::GitersImpl::Visit_t<TVisitor>&& visitor)
{
	return Giters::GitersImpl::VisitImpl<TSeq, TVisitor>(source, std::forward<TVisitor>(visitor.visitor));
}
