// Copyright © Ian Good

#pragma once

#include "Where.h"

namespace Giters
{
	auto NonNull() { return Where([](auto&& p) { return p != nullptr; }); }
}
