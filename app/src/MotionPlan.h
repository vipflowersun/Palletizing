#pragma once

#include "Palletizer.h"

#include <list>
#include <vector>

class MotionPlan
{
public:
	struct MotionPlanInput
	{
		Palletizer::PalletizingOutput palletizingOutput;
		std::size_t gripperCount = 0;
	};

	enum class ActionType
	{
		Load,
		Place
	};

	struct MotionAction
	{
		ActionType type = ActionType::Load;
		double centerX = 0.0;
		double centerY = 0.0;
		double angleDegrees = 0.0;
		std::vector<std::size_t> gripperIndices;
	};

	struct MotionPlanOutput
	{
		bool success = false;
		const char* message = "";
		std::list<MotionAction> actions;
	};

	MotionPlanOutput Calculate(const MotionPlanInput& input) const;
};
