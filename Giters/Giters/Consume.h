// Copyright © Ian Good

#pragma once

namespace Giters
{
	struct Consume
	{
	};
}

template <typename TSeq>
void operator|(TSeq&& source, Giters::Consume)
{
	for (auto&& elem : source)
	{
	}
}
