#pragma once
#include <vector>
#include <cstdint>

class Palletizer
{
public:
	enum class Orientation
	{
		Degrees0,
		Degrees90
	};

	struct PalletizingInput
	{
		std::int64_t palletLength = 0;
		std::int64_t palletWidth = 0;
		std::int64_t boxLength = 0;
		std::int64_t boxWidth = 0;
		std::int64_t availableBoxes = 0;
		std::int64_t outlineTolerance = 0;
	};

	struct BoxPlacement
	{
		double centerX = 0.0;
		double centerY = 0.0;
		std::int64_t length = 0;
		std::int64_t width = 0;
		Orientation orientation = Orientation::Degrees0;
	};

	struct PalletizingOutput
	{
		bool success = false;
		const char* message = "";
		std::int64_t boxCount = 0;
		double coverage = 0.0;
		std::int64_t occupiedLength = 0;
		std::int64_t occupiedWidth = 0;
		std::int64_t outlineError = 0;
		std::vector<BoxPlacement> placements;
	};

	struct InternalPlacement
	{
		std::int64_t x = 0;
		std::int64_t y = 0;
		std::int64_t length = 0;
		std::int64_t width = 0;
		Orientation orientation = Orientation::Degrees0;
	};

	struct Strip
	{
		Orientation orientation = Orientation::Degrees0;
		std::int64_t offset = 0;
		std::int64_t thickness = 0;
		std::int64_t occupiedLength = 0;
		std::int64_t boxCount = 0;
	};

	struct Layout
	{
		std::vector<Strip> strips;
		std::vector<InternalPlacement> placements;
		bool stripsAlongX = true;
		std::int64_t boxCount = 0;
		std::int64_t occupiedLength = 0;
		std::int64_t occupiedWidth = 0;
		std::int64_t outlineError = 0;
	};
public:
	PalletizingOutput Calculate(const PalletizingInput& input) const;
	PalletizingOutput MirrorOutput(const PalletizingOutput& output) const;
private:
	static bool IsSafeArea(std::int64_t length, std::int64_t width);
	static bool IsBetterLayout(const Layout& candidate, const Layout& best);
	static bool IsValidLayout(const Layout& layout, std::int64_t palletLength, std::int64_t palletWidth,
		std::int64_t boxLength, std::int64_t boxWidth, std::int64_t outlineTolerance);
	static void GeneratePlacements(Layout& layout, std::int64_t boxLength, std::int64_t boxWidth);
	static void AddStripCandidates(std::int64_t runLimit, std::int64_t stackLimit,
		std::int64_t run0, std::int64_t thickness0,
		std::int64_t run90, std::int64_t thickness90,
		std::int64_t availableBoxes, std::int64_t outlineTolerance, bool stripsAlongX,
		std::vector<Layout>& candidates);
};