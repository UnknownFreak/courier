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
			std::sort(containerToClear.begin(), containerToClear.end());
		}
		referenceIds.clear();
	}
}