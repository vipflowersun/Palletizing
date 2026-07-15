#include "MotionPlan.h"

#include <algorithm>
#include <bit>
#include <string>
#include <unordered_map>

namespace
{
	constexpr double CoordinateTolerance = 1e-6;

	unsigned int popcount(unsigned int value)
	{
		unsigned int count = 0;

		while (value != 0)
		{
			value &= value - 1;
			++count;
		}

		return count;
	}

	struct PlacementCandidate
	{
		std::vector<std::size_t> boxIndices;
		double centerX = 0.0;
		double centerY = 0.0;
		double angleDegrees = 0.0;
		bool longAxisAlongX = true;
		int positioningNormalSign = 1;
	};

	bool NearlyEqual(double left, double right)
	{
		return std::abs(left - right) <= CoordinateTolerance;
	}

	bool HasSameGeometry(const Palletizer::BoxPlacement& left,
		const Palletizer::BoxPlacement& right)
	{
		return left.orientation == right.orientation &&
			left.length == right.length && left.width == right.width;
	}

	double GetLongCoordinate(const Palletizer::BoxPlacement& placement, bool longAxisAlongX)
	{
		return longAxisAlongX ? placement.centerX : placement.centerY;
	}

	double GetShortCoordinate(const Palletizer::BoxPlacement& placement, bool longAxisAlongX)
	{
		return longAxisAlongX ? placement.centerY : placement.centerX;
	}

	std::vector<PlacementCandidate> EnumerateCandidates(
		const std::vector<Palletizer::BoxPlacement>& placements, std::size_t gripperCount)
	{
		std::vector<PlacementCandidate> candidates;

		for (std::size_t anchorIndex = 0; anchorIndex < placements.size(); ++anchorIndex)
		{
			const Palletizer::BoxPlacement& anchor = placements[anchorIndex];
			const bool longAxisAlongX = anchor.length >= anchor.width;
			const double shortStep = static_cast<double>(longAxisAlongX ? anchor.width : anchor.length);
			std::vector<std::size_t> lineIndices;

			for (std::size_t index = 0; index < placements.size(); ++index)
			{
				const Palletizer::BoxPlacement& placement = placements[index];
				if (HasSameGeometry(anchor, placement) &&
					NearlyEqual(GetLongCoordinate(anchor, longAxisAlongX),
						GetLongCoordinate(placement, longAxisAlongX)))
				{
					lineIndices.push_back(index);
				}
			}

			std::sort(lineIndices.begin(), lineIndices.end(),
				[&placements, longAxisAlongX](std::size_t left, std::size_t right)
				{
					return GetShortCoordinate(placements[left], longAxisAlongX) >
						GetShortCoordinate(placements[right], longAxisAlongX);
				});

			const auto anchorPosition = std::find(lineIndices.begin(), lineIndices.end(), anchorIndex);
			const std::size_t first = static_cast<std::size_t>(anchorPosition - lineIndices.begin());
			for (std::size_t count = 1;
				count <= gripperCount && first + count <= lineIndices.size(); ++count)
			{
				if (count > 1)
				{
					const double previous = GetShortCoordinate(
						placements[lineIndices[first + count - 2]], longAxisAlongX);
					const double current = GetShortCoordinate(
						placements[lineIndices[first + count - 1]], longAxisAlongX);
					if (!NearlyEqual(previous - current, shortStep))
					{
						break;
					}
				}

				PlacementCandidate candidate;
				candidate.boxIndices.assign(lineIndices.begin() + first,
					lineIndices.begin() + first + count);
				candidate.longAxisAlongX = longAxisAlongX;
				for (const std::size_t index : candidate.boxIndices)
				{
					candidate.centerX += placements[index].centerX;
					candidate.centerY += placements[index].centerY;
				}
				candidate.centerX /= static_cast<double>(count);
				candidate.centerY /= static_cast<double>(count);

				candidate.positioningNormalSign = 1;
				candidate.angleDegrees = longAxisAlongX ? 0.0 : 90.0;
				candidates.push_back(candidate);

				candidate.positioningNormalSign = -1;
				candidate.angleDegrees = longAxisAlongX ? 180.0 : -90.0;
				candidates.push_back(std::move(candidate));
			}
		}

		return candidates;
	}

	bool ProjectionsOverlap(double firstMinimum, double firstMaximum,
		double secondMinimum, double secondMaximum)
	{
		return std::min(firstMaximum, secondMaximum) -
			std::max(firstMinimum, secondMinimum) > CoordinateTolerance;
	}

	bool IsActiveSideBlocked(const PlacementCandidate& candidate,
		const std::vector<Palletizer::BoxPlacement>& placements,
		const std::vector<bool>& placed)
	{
		const Palletizer::BoxPlacement& member = placements[candidate.boxIndices.front()];
		const double halfLong = static_cast<double>(
			candidate.longAxisAlongX ? member.length : member.width) / 2.0;
		const int activeNormalSign = -candidate.positioningNormalSign;
		const double activeBoundary = GetLongCoordinate(member, candidate.longAxisAlongX) +
			static_cast<double>(activeNormalSign) * halfLong;

		double groupShortMinimum = GetShortCoordinate(member, candidate.longAxisAlongX) -
			static_cast<double>(candidate.longAxisAlongX ? member.width : member.length) / 2.0;
		double groupShortMaximum = GetShortCoordinate(member, candidate.longAxisAlongX) +
			static_cast<double>(candidate.longAxisAlongX ? member.width : member.length) / 2.0;
		for (const std::size_t index : candidate.boxIndices)
		{
			const Palletizer::BoxPlacement& placement = placements[index];
			const double halfShort = static_cast<double>(
				candidate.longAxisAlongX ? placement.width : placement.length) / 2.0;
			groupShortMinimum = std::min(groupShortMinimum,
				GetShortCoordinate(placement, candidate.longAxisAlongX) - halfShort);
			groupShortMaximum = std::max(groupShortMaximum,
				GetShortCoordinate(placement, candidate.longAxisAlongX) + halfShort);
		}

		for (std::size_t index = 0; index < placements.size(); ++index)
		{
			if (!placed[index])
			{
				continue;
			}

			const Palletizer::BoxPlacement& obstacle = placements[index];
			const double obstacleHalfLong = static_cast<double>(
				candidate.longAxisAlongX ? obstacle.length : obstacle.width) / 2.0;
			const double obstacleTouchingEdge = GetLongCoordinate(obstacle, candidate.longAxisAlongX) -
				static_cast<double>(activeNormalSign) * obstacleHalfLong;
			if (!NearlyEqual(obstacleTouchingEdge, activeBoundary))
			{
				continue;
			}

			const double obstacleHalfShort = static_cast<double>(
				candidate.longAxisAlongX ? obstacle.width : obstacle.length) / 2.0;
			const double obstacleShort = GetShortCoordinate(obstacle, candidate.longAxisAlongX);
			if (ProjectionsOverlap(groupShortMinimum, groupShortMaximum,
				obstacleShort - obstacleHalfShort, obstacleShort + obstacleHalfShort))
			{
				return true;
			}
		}

		return false;
	}

	struct SearchState
	{
		std::vector<bool> placed;
		std::vector<bool> loadedGrippers;
		std::vector<MotionPlan::MotionAction> actions;
		std::size_t placedCount = 0;
		std::size_t loadCount = 0;
	};

	struct Rectangle
	{
		double minimumX = 0.0;
		double maximumX = 0.0;
		double minimumY = 0.0;
		double maximumY = 0.0;
	};

	Rectangle GetRectangle(const Palletizer::BoxPlacement& placement)
	{
		return {
			placement.centerX - static_cast<double>(placement.length) / 2.0,
			placement.centerX + static_cast<double>(placement.length) / 2.0,
			placement.centerY - static_cast<double>(placement.width) / 2.0,
			placement.centerY + static_cast<double>(placement.width) / 2.0
		};
	}

	bool RectanglesOverlap(const Rectangle& left, const Rectangle& right)
	{
		return ProjectionsOverlap(left.minimumX, left.maximumX, right.minimumX, right.maximumX) &&
			ProjectionsOverlap(left.minimumY, left.maximumY, right.minimumY, right.maximumY);
	}

	bool AreCarriedBoxesCollisionFree(const PlacementCandidate& candidate,
		std::size_t firstGripper, const std::vector<bool>& loadedGrippers,
		const std::vector<Palletizer::BoxPlacement>& placements,
		const std::vector<bool>& placed)
	{
		const Palletizer::BoxPlacement& firstTarget = placements[candidate.boxIndices.front()];
		const double shortStep = static_cast<double>(
			candidate.longAxisAlongX ? firstTarget.width : firstTarget.length);
		const double firstShortCoordinate = GetShortCoordinate(firstTarget, candidate.longAxisAlongX);
		const double longCoordinate = GetLongCoordinate(firstTarget, candidate.longAxisAlongX);

		for (std::size_t gripper = 0; gripper < loadedGrippers.size(); ++gripper)
		{
			const bool releasedNow = gripper >= firstGripper &&
				gripper < firstGripper + candidate.boxIndices.size();
			if (!loadedGrippers[gripper] || releasedNow)
			{
				continue;
			}

			Palletizer::BoxPlacement carried = firstTarget;
			const double offset = static_cast<double>(gripper) - static_cast<double>(firstGripper);
			if (candidate.longAxisAlongX)
			{
				carried.centerX = longCoordinate;
				carried.centerY = firstShortCoordinate - offset * shortStep;
			}
			else
			{
				carried.centerX = firstShortCoordinate - offset * shortStep;
				carried.centerY = longCoordinate;
			}
			const Rectangle carriedRectangle = GetRectangle(carried);

			for (std::size_t boxIndex = 0; boxIndex < placements.size(); ++boxIndex)
			{
				const bool isCurrentTarget = std::find(candidate.boxIndices.begin(),
					candidate.boxIndices.end(), boxIndex) != candidate.boxIndices.end();
				if ((placed[boxIndex] || isCurrentTarget) &&
					RectanglesOverlap(carriedRectangle, GetRectangle(placements[boxIndex])))
				{
					return false;
				}
			}
		}

		return true;
	}

	bool CanPlace(const PlacementCandidate& candidate,
		const std::vector<Palletizer::BoxPlacement>& placements,
		const std::vector<bool>& placed, const std::vector<bool>& loadedGrippers,
		std::size_t firstGripper)
	{
		if (firstGripper + candidate.boxIndices.size() > loadedGrippers.size())
		{
			return false;
		}
		for (std::size_t offset = 0; offset < candidate.boxIndices.size(); ++offset)
		{
			if (!loadedGrippers[firstGripper + offset])
			{
				return false;
			}
		}
		for (const std::size_t index : candidate.boxIndices)
		{
			if (placed[index])
			{
				return false;
			}
		}

		return !IsActiveSideBlocked(candidate, placements, placed) &&
			AreCarriedBoxesCollisionFree(candidate, firstGripper, loadedGrippers,
				placements, placed);
	}

	bool HasLoadedGripper(const std::vector<bool>& loadedGrippers)
	{
		return std::find(loadedGrippers.begin(), loadedGrippers.end(), true) !=
			loadedGrippers.end();
	}

	std::size_t CountLoadedGrippers(const std::vector<bool>& loadedGrippers)
	{
		return static_cast<std::size_t>(std::count(
			loadedGrippers.begin(), loadedGrippers.end(), true));
	}

	std::size_t CountLoadedRuns(const std::vector<bool>& loadedGrippers)
	{
		std::size_t runs = 0;
		bool previousLoaded = false;
		for (const bool loaded : loadedGrippers)
		{
			if (loaded && !previousLoaded)
			{
				++runs;
			}
			previousLoaded = loaded;
		}
		return runs;
	}

	void AppendBits(std::string& key, const std::vector<bool>& values)
	{
		const std::size_t originalSize = key.size();
		key.resize(originalSize + (values.size() + 7) / 8, '\0');
		for (std::size_t index = 0; index < values.size(); ++index)
		{
			if (values[index])
			{
				key[originalSize + index / 8] = static_cast<char>(
					key[originalSize + index / 8] | (1 << (index % 8)));
			}
		}
	}

	std::string MakeStateKey(const SearchState& state)
	{
		std::string key;
		AppendBits(key, state.placed);
		key.push_back('|');
		AppendBits(key, state.loadedGrippers);
		return key;
	}

	bool IsBetterState(const SearchState& left, const SearchState& right)
	{
		if (left.placedCount != right.placedCount)
		{
			return left.placedCount > right.placedCount;
		}
		if (left.loadCount != right.loadCount)
		{
			return left.loadCount < right.loadCount;
		}
		const std::size_t leftLoaded = CountLoadedGrippers(left.loadedGrippers);
		const std::size_t rightLoaded = CountLoadedGrippers(right.loadedGrippers);
		if (leftLoaded != rightLoaded)
		{
			return leftLoaded < rightLoaded;
		}
		return CountLoadedRuns(left.loadedGrippers) < CountLoadedRuns(right.loadedGrippers);
	}

	std::vector<std::vector<bool>> EnumerateLoadPatterns(std::size_t gripperCount,
		std::size_t remainingBoxes)
	{
		constexpr std::size_t MaximumPatterns = 4096;
		std::vector<std::vector<bool>> patterns;
		if (gripperCount < sizeof(std::size_t) * 8)
		{
			const std::size_t maskLimit = static_cast<std::size_t>(1) << gripperCount;
			for (std::size_t mask = 1; mask < maskLimit; ++mask)
			{
				if (static_cast<std::size_t>(popcount(mask)) > remainingBoxes)
				{
					continue;
				}
				std::vector<bool> pattern(gripperCount, false);
				for (std::size_t index = 0; index < gripperCount; ++index)
				{
					pattern[index] = (mask & (static_cast<std::size_t>(1) << index)) != 0;
				}
				patterns.push_back(std::move(pattern));
			}
		}
		else
		{
			for (std::size_t count = 1; count <= std::min(gripperCount, remainingBoxes); ++count)
			{
				for (std::size_t first = 0; first + count <= gripperCount; ++first)
				{
					std::vector<bool> pattern(gripperCount, false);
					std::fill(pattern.begin() + first, pattern.begin() + first + count, true);
					patterns.push_back(std::move(pattern));
				}
			}
		}

		std::sort(patterns.begin(), patterns.end(),
			[](const std::vector<bool>& left, const std::vector<bool>& right)
			{
				const std::size_t leftCount = CountLoadedGrippers(left);
				const std::size_t rightCount = CountLoadedGrippers(right);
				if (leftCount != rightCount)
				{
					return leftCount > rightCount;
				}
				const std::size_t leftRuns = CountLoadedRuns(left);
				const std::size_t rightRuns = CountLoadedRuns(right);
				if (leftRuns != rightRuns)
				{
					return leftRuns < rightRuns;
				}
				return left < right;
			});
		if (patterns.size() > MaximumPatterns)
		{
			patterns.resize(MaximumPatterns);
		}
		return patterns;
	}

	MotionPlan::MotionAction MakeLoadAction(const std::vector<bool>& loadedGrippers)
	{
		MotionPlan::MotionAction action;
		action.type = MotionPlan::ActionType::Load;
		for (std::size_t index = 0; index < loadedGrippers.size(); ++index)
		{
			if (loadedGrippers[index])
			{
				action.gripperIndices.push_back(index);
			}
		}
		return action;
	}

	MotionPlan::MotionAction MakePlaceAction(const PlacementCandidate& candidate,
		std::size_t firstGripper)
	{
		MotionPlan::MotionAction action;
		action.type = MotionPlan::ActionType::Place;
		action.centerX = candidate.centerX;
		action.centerY = candidate.centerY;
		action.angleDegrees = candidate.angleDegrees;
		for (std::size_t offset = 0; offset < candidate.boxIndices.size(); ++offset)
		{
			action.gripperIndices.push_back(firstGripper + offset);
		}
		return action;
	}
}

MotionPlan::MotionPlanOutput MotionPlan::Calculate(const MotionPlanInput& input) const
{
	MotionPlanOutput output;
	if (!input.palletizingOutput.success)
	{
		output.message = "Palletizing output is not successful.";
		return output;
	}
	if (input.gripperCount == 0)
	{
		output.message = "Gripper count must be positive.";
		return output;
	}

	const std::vector<Palletizer::BoxPlacement>& placements = input.palletizingOutput.placements;
	if (placements.empty())
	{
		output.message = "Palletizing output has no box placements.";
		return output;
	}

	const std::vector<PlacementCandidate> candidates =
		EnumerateCandidates(placements, input.gripperCount);
	constexpr std::size_t BeamWidth = 512;
	std::vector<SearchState> frontier(1);
	frontier.front().placed.assign(placements.size(), false);
	frontier.front().loadedGrippers.assign(input.gripperCount, false);
	SearchState solution;
	bool foundSolution = false;

	while (!frontier.empty() && !foundSolution)
	{
		std::unordered_map<std::string, SearchState> uniqueStates;
		for (const SearchState& state : frontier)
		{
			if (!HasLoadedGripper(state.loadedGrippers))
			{
				const std::vector<std::vector<bool>> loadPatterns = EnumerateLoadPatterns(
					input.gripperCount, placements.size() - state.placedCount);
				for (const std::vector<bool>& pattern : loadPatterns)
				{
					SearchState next = state;
					next.loadedGrippers = pattern;
					++next.loadCount;
					next.actions.push_back(MakeLoadAction(pattern));
					const std::string key = MakeStateKey(next);
					auto existing = uniqueStates.find(key);
					if (existing == uniqueStates.end() || IsBetterState(next, existing->second))
					{
						uniqueStates[key] = std::move(next);
					}
				}
				continue;
			}

			for (const PlacementCandidate& candidate : candidates)
			{
				for (std::size_t firstGripper = 0;
					firstGripper + candidate.boxIndices.size() <= input.gripperCount; ++firstGripper)
				{
					if (!CanPlace(candidate, placements, state.placed,
						state.loadedGrippers, firstGripper))
					{
						continue;
					}

					SearchState next = state;
					for (const std::size_t boxIndex : candidate.boxIndices)
					{
						next.placed[boxIndex] = true;
					}
					for (std::size_t offset = 0; offset < candidate.boxIndices.size(); ++offset)
					{
						next.loadedGrippers[firstGripper + offset] = false;
					}
					next.placedCount += candidate.boxIndices.size();
					next.actions.push_back(MakePlaceAction(candidate, firstGripper));
					if (next.placedCount == placements.size() &&
						!HasLoadedGripper(next.loadedGrippers))
					{
						solution = std::move(next);
						foundSolution = true;
						break;
					}

					const std::string key = MakeStateKey(next);
					auto existing = uniqueStates.find(key);
					if (existing == uniqueStates.end() || IsBetterState(next, existing->second))
					{
						uniqueStates[key] = std::move(next);
					}
				}
				if (foundSolution)
				{
					break;
				}
			}
			if (foundSolution)
			{
				break;
			}
		}

		if (foundSolution)
		{
			break;
		}

		frontier.clear();
		frontier.reserve(uniqueStates.size());
		for (auto& entry : uniqueStates)
		{
			frontier.push_back(std::move(entry.second));
		}
		std::sort(frontier.begin(), frontier.end(), IsBetterState);
		if (frontier.size() > BeamWidth)
		{
			frontier.resize(BeamWidth);
		}
	}

	if (!foundSolution)
	{
		output.message = "No complete collision-free placement sequence was found.";
		return output;
	}

	for (MotionAction& action : solution.actions)
	{
		output.actions.push_back(std::move(action));
	}

	output.success = true;
	output.message = "Success.";
	return output;
}
