#include "GameplayTagStack.h"

void FGameplayTagStackContainer::AddStack(FGameplayTag _Tag, int32 _StackCount)
{
	if (_Tag.IsValid() == false)
	{
		return;
	}

	if (0 < _StackCount)
	{
		for (FGameplayTagStack& stack : TagStacks)
		{
			if (stack.Tag == _Tag)
			{
				const int32 newCount = stack.StackCount + _StackCount;
				stack.StackCount = newCount;
				TagCountMap[_Tag] = newCount;
				return;
			}
		}

		FGameplayTagStack& newStack = TagStacks.Emplace_GetRef(_Tag, _StackCount);
		TagCountMap.Add(_Tag, _StackCount);
	}
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag _Tag, int32 _StackCount)
{
	if (_Tag.IsValid() == false)
	{
		return;
	}

	if (0 < _StackCount)
	{
		for (auto iter = TagStacks.CreateIterator(); iter; ++iter)
		{
			FGameplayTagStack& stack = *iter;
			if (stack.Tag == _Tag)
			{
				if (stack.StackCount <= _StackCount)
				{
					iter.RemoveCurrent();
					TagCountMap.Remove(_Tag);
				}
				else
				{
					const int32 newCount = stack.StackCount - _StackCount;
					stack.StackCount = newCount;
					TagCountMap[_Tag] = newCount;
				}

				break;
			}
		}
	}
}

int32 FGameplayTagStackContainer::GetStackCount(FGameplayTag _Tag) const
{
	return TagCountMap.FindRef(_Tag);
}

bool FGameplayTagStackContainer::ContainsTag(FGameplayTag _Tag) const
{
	return TagCountMap.Contains(_Tag);
}
