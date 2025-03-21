#pragma once

#include <functional>
#include <algorithm>
#include <vector>

namespace courier::util
{

	template<class V>
	void vectorFastRemove(auto& containerToClear, std::vector<V>& referenceIds)
	{
		std::sort(referenceIds.begin(), referenceIds.end(), std::greater<V>());
		auto it = containerToClear.rbegin();
		auto eit = containerToClear.rbegin();
		for (auto i : referenceIds)
		{
			while (it != containerToClear.rend())
			{
				if (it->getId() == i)
				{
					if (it == eit)
					{
						//its already at last place
						eit++;
						break;
					}
					std::iter_swap(it, eit);
					eit++;
					break;
				}
				else
				{
					it++;
				}
			}
		}
		if (referenceIds.size() > 0)
		{
			if (eit == containerToClear.rend())
				eit = containerToClear.rbegin();
			V _id = eit->getId();
			while (containerToClear.back().getId() != _id)
			{
				containerToClear.pop_back();
			}
			std::sort(containerToClear.begin(), containerToClear.end(), [](const auto& a, const auto& b) -> bool { return (size_t)a.getId() < (size_t)b.getId(); });
		}
		referenceIds.clear();
	}
}

namespace courier::concepts
{

	template<class T, class U>
	concept getId = requires (T t)
	{
		{ t.getId() } -> std::same_as<U>;
	};

	template <class T, class U>
	concept Subscriber = getId<T, U>;
}