// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		template <typename TVisitor>
		struct ForEach_t
		{
			explicit ForEach_t(TVisitor&& inVisitor)
				: visitor(std::forward<TVisitor>(inVisitor))
			{
			}

			TVisitor&& visitor;
		};
	}

	template <typename TVisitor>
	auto ForEach(TVisitor&& visitor)
	{
		return GitersImpl::ForEach_t<TVisitor>(std::forward<TVisitor>(visitor));
	}

	template <typename TSeq, typename TVisitor>
	void operator|(TSeq&& source, GitersImpl::ForEach_t<TVisitor>&& visitor)
	{
		for (auto&& elem : source)
		{
			std::invoke(visitor.visitor, elem);
		}
	}
}
