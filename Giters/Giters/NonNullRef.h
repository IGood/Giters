// Copyright © Ian Good

#pragma once

namespace Giters
{
	namespace GitersImpl
	{
		template <typename TSeq>
		struct NonNullRefImpl
		{
			using SourceIter_t = decltype(std::begin(std::declval<TSeq>()));
			using SourceEnd_t = decltype(std::end(std::declval<TSeq>()));

			struct Iter_t
			{
				bool operator!=(const SourceEnd_t& end) const { return sourceIter != end; }
				auto& operator*() { return **sourceIter; }
				void operator++()
				{
					do
					{
						++sourceIter;
					} while (sourceIter != sourceEnd && *sourceIter == nullptr);
				}

				Iter_t(TSeq& source)
					: sourceIter(std::begin(source))
					, sourceEnd(std::end(source))
				{
					while (sourceIter != sourceEnd && *sourceIter == nullptr)
					{
						++sourceIter;
					}
				}

				SourceIter_t sourceIter;
				SourceEnd_t sourceEnd;
			};

			NonNullRefImpl(TSeq& inSource)
				: source(inSource)
			{
			}

			Iter_t begin() { return Iter_t(source); }
			SourceEnd_t end() const { return std::end(source); }

			TSeq& source;
		};
	}

	struct NonNullRef
	{
	};
}

template <typename TSeq>
auto operator|(TSeq&& source, Giters::NonNullRef)
{
	return Giters::GitersImpl::NonNullRefImpl<TSeq>(source);
}
