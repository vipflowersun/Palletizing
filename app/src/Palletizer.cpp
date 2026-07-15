#include "Palletizer.h"

#include <cmath>
#include <limits>
#include <algorithm>

bool Palletizer::IsSafeArea(std::int64_t length, std::int64_t width)
{
	return length <= std::numeric_limits<std::int64_t>::max() / width;
}

bool Palletizer::IsBetterLayout(const Layout& candidate, const Layout& best)
{
	if (candidate.boxCount != best.boxCount)
	{
		return candidate.boxCount > best.boxCount;
	}
	if (candidate.outlineError != best.outlineError)
	{
		return candidate.outlineError < best.outlineError;
	}

	const std::int64_t candidateArea = candidate.occupiedLength * candidate.occupiedWidth;
	const std::int64_t bestArea = best.occupiedLength * best.occupiedWidth;
	if (candidateArea != bestArea)
	{
		return candidateArea < bestArea;
	}
	if (candidate.occupiedLength != best.occupiedLength)
	{
		return candidate.occupiedLength > best.occupiedLength;
	}
	return candidate.stripsAlongX && !best.stripsAlongX;
}

bool Palletizer::IsValidLayout(const Layout& layout, std::int64_t palletLength, std::int64_t palletWidth,
	std::int64_t boxLength, std::int64_t boxWidth, std::int64_t outlineTolerance)
{
	if (layout.strips.empty() || layout.boxCount < 2)
	{
		return false;
	}

	const std::int64_t runLimit = layout.stripsAlongX ? palletLength : palletWidth;
	const std::int64_t stackLimit = layout.stripsAlongX ? palletWidth : palletLength;
	std::int64_t expectedOffset = 0;
	std::int64_t minimumRun = runLimit;
	std::int64_t maximumRun = 0;
	std::int64_t countedBoxes = 0;
	bool has0 = false;
	bool has90 = false;

	for (const Strip& strip : layout.strips)
	{
		const std::int64_t boxRun = layout.stripsAlongX
			? (strip.orientation == Orientation::Degrees0 ? boxLength : boxWidth)
			: (strip.orientation == Orientation::Degrees0 ? boxWidth : boxLength);
		const std::int64_t boxThickness = layout.stripsAlongX
			? (strip.orientation == Orientation::Degrees0 ? boxWidth : boxLength)
			: (strip.orientation == Orientation::Degrees0 ? boxLength : boxWidth);

		if (strip.offset != expectedOffset || strip.boxCount <= 0 ||
			strip.thickness != boxThickness || strip.occupiedLength != strip.boxCount * boxRun ||
			strip.occupiedLength > runLimit || strip.offset + strip.thickness > stackLimit)
		{
			return false;
		}

		expectedOffset += strip.thickness;
		minimumRun = std::min(minimumRun, strip.occupiedLength);
		maximumRun = std::max(maximumRun, strip.occupiedLength);
		countedBoxes += strip.boxCount;
		has0 = has0 || strip.orientation == Orientation::Degrees0;
		has90 = has90 || strip.orientation == Orientation::Degrees90;
	}

	return has0 && has90 && countedBoxes == layout.boxCount &&
		maximumRun - minimumRun <= outlineTolerance;
}

void Palletizer::GeneratePlacements(Layout& layout, std::int64_t boxLength, std::int64_t boxWidth)
{
	layout.placements.clear();
	layout.placements.reserve(static_cast<std::size_t>(layout.boxCount));

	for (const Strip& strip : layout.strips)
	{
		const std::int64_t placedLength = strip.orientation == Orientation::Degrees0 ? boxLength : boxWidth;
		const std::int64_t placedWidth = strip.orientation == Orientation::Degrees0 ? boxWidth : boxLength;
		const std::int64_t runStep = layout.stripsAlongX ? placedLength : placedWidth;

		for (std::int64_t index = 0; index < strip.boxCount; ++index)
		{
			InternalPlacement placement;
			placement.x = layout.stripsAlongX ? index * runStep : strip.offset;
			placement.y = layout.stripsAlongX ? strip.offset : index * runStep;
			placement.length = placedLength;
			placement.width = placedWidth;
			placement.orientation = strip.orientation;
			layout.placements.push_back(placement);
		}
	}
}

void Palletizer::AddStripCandidates(std::int64_t runLimit, std::int64_t stackLimit,
	std::int64_t run0, std::int64_t thickness0,
	std::int64_t run90, std::int64_t thickness90,
	std::int64_t availableBoxes, std::int64_t outlineTolerance, bool stripsAlongX,
	std::vector<Layout>& candidates)
{
	const std::int64_t effectiveTolerance = std::min(outlineTolerance, runLimit);
	const std::int64_t maxCount0 = runLimit / run0;
	const std::int64_t maxCount90 = runLimit / run90;
	const std::int64_t maxStrips0 = stackLimit / thickness0;

	for (std::int64_t count0 = 1; count0 <= maxCount0; ++count0)
	{
		const std::int64_t length0 = count0 * run0;
		const std::int64_t minLength90 = std::max<std::int64_t>(1, length0 - effectiveTolerance);
		const std::int64_t minCount90 = std::max<std::int64_t>(1, (minLength90 + run90 - 1) / run90);
		const std::int64_t maximumNearLength = length0 > runLimit - effectiveTolerance
			? runLimit
			: length0 + effectiveTolerance;
		const std::int64_t maxNearCount90 = std::min(maxCount90, maximumNearLength / run90);

		for (std::int64_t count90 = minCount90; count90 <= maxNearCount90; ++count90)
		{
			const std::int64_t length90 = count90 * run90;

			for (std::int64_t strips0 = 1; strips0 <= maxStrips0; ++strips0)
			{
				const std::int64_t usedBy0 = strips0 * count0;
				const std::int64_t remainingStack = stackLimit - strips0 * thickness0;
				if (usedBy0 >= availableBoxes || remainingStack < thickness90)
				{
					break;
				}

				const std::int64_t strips90 = std::min(
					remainingStack / thickness90,
					(availableBoxes - usedBy0) / count90);
				if (strips90 < 1)
				{
					continue;
				}

				Layout layout;
				layout.stripsAlongX = stripsAlongX;
				layout.boxCount = usedBy0 + strips90 * count90;
				layout.outlineError = std::llabs(length0 - length90);
				const std::int64_t occupiedRun = std::max(length0, length90);
				const std::int64_t occupiedStack = strips0 * thickness0 + strips90 * thickness90;
				layout.occupiedLength = stripsAlongX ? occupiedRun : occupiedStack;
				layout.occupiedWidth = stripsAlongX ? occupiedStack : occupiedRun;

				std::int64_t offset = 0;
				for (std::int64_t index = 0; index < strips0; ++index)
				{
					layout.strips.push_back({ Orientation::Degrees0, offset, thickness0, length0, count0 });
					offset += thickness0;
				}
				for (std::int64_t index = 0; index < strips90; ++index)
				{
					layout.strips.push_back({ Orientation::Degrees90, offset, thickness90, length90, count90 });
					offset += thickness90;
				}

				if (IsValidLayout(layout,
					stripsAlongX ? runLimit : stackLimit,
					stripsAlongX ? stackLimit : runLimit,
					stripsAlongX ? run0 : thickness0,
					stripsAlongX ? thickness0 : run0,
					effectiveTolerance))
				{
					candidates.push_back(std::move(layout));
				}
			}
		}
	}
}

Palletizer::PalletizingOutput Palletizer::MirrorOutput(const Palletizer::PalletizingOutput& output) const
{
	PalletizingOutput mirrored = output;
	for (BoxPlacement& placement : mirrored.placements)
	{
		placement.centerX = static_cast<double>(output.occupiedLength) - placement.centerX;
		placement.centerY = static_cast<double>(output.occupiedWidth) - placement.centerY;
	}
	return mirrored;
}

Palletizer::PalletizingOutput Palletizer::Calculate(const Palletizer::PalletizingInput& input) const
{
	PalletizingOutput output;
	if (input.outlineTolerance < 0)
	{
		output.message = "Outline tolerance cannot be negative.";
		return output;
	}

	if (input.palletLength <= 0 || input.palletWidth <= 0 || input.boxLength <= 0 || input.boxWidth <= 0)
	{
		output.message = "All dimensions must be positive.";
		return output;
	}

	if (input.availableBoxes < 2)
	{
		output.message = "A mixed-orientation layout needs at least two boxes.";
		return output;
	}

	if (!IsSafeArea(input.palletLength, input.palletWidth) || !IsSafeArea(input.boxLength, input.boxWidth))
	{
		output.message = "Dimensions are too large for safe area calculation.";
		return output;
	}

	const std::int64_t boxArea = input.boxLength * input.boxWidth;
	if (input.availableBoxes > std::numeric_limits<std::int64_t>::max() / boxArea)
	{
		output.message = "Box count is too large for safe coverage calculation.";
		return output;
	}

	std::vector<Layout> candidates;
	AddStripCandidates(input.palletLength, input.palletWidth,
		input.boxLength, input.boxWidth, input.boxWidth, input.boxLength,
		input.availableBoxes, input.outlineTolerance, true, candidates);
	AddStripCandidates(input.palletWidth, input.palletLength,
		input.boxWidth, input.boxLength, input.boxLength, input.boxWidth,
		input.availableBoxes, input.outlineTolerance, false, candidates);

	if (candidates.empty())
	{
		output.message = "No valid single-layer mixed-orientation layout was found.";
		return output;
	}

	Layout* bestLayout = &candidates.front();
	for (Layout& candidate : candidates)
	{
		if (IsBetterLayout(candidate, *bestLayout))
		{
			bestLayout = &candidate;
		}
	}

	GeneratePlacements(*bestLayout, input.boxLength, input.boxWidth);
	if (bestLayout->placements.size() != static_cast<std::size_t>(bestLayout->boxCount))
	{
		output.message = "Placement count does not match the planned count.";
		return output;
	}

	const std::int64_t palletArea = input.palletLength * input.palletWidth;
	const std::int64_t coveredArea = bestLayout->boxCount * boxArea;
	output.success = true;
	output.message = "Success.";
	output.boxCount = bestLayout->boxCount;
	output.coverage = static_cast<double>(coveredArea) * 100.0 /
		static_cast<double>(palletArea);
	output.occupiedLength = bestLayout->occupiedLength;
	output.occupiedWidth = bestLayout->occupiedWidth;
	output.outlineError = bestLayout->outlineError;
	output.placements.reserve(bestLayout->placements.size());
	for (const InternalPlacement& placement : bestLayout->placements)
	{
		output.placements.push_back({
			static_cast<double>(placement.x) + static_cast<double>(placement.length) / 2.0,
			static_cast<double>(placement.y) + static_cast<double>(placement.width) / 2.0,
			placement.length,
			placement.width,
			placement.orientation
			});
	}
	return output;
}
