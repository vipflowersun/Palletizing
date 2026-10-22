#pragma once
#include <vector>
#include <cstdint>

// 单层混合朝向码垛：按条带排列箱子，方案必须同时包含 0° 和 90° 朝向。
// 所有尺寸、坐标及公差使用同一长度单位；X 对应托盘长度，Y 对应托盘宽度。
class Palletizer
{
public:
	// 箱子相对于输入长宽方向的平面朝向。
	enum class Orientation
	{
		Degrees0,  // 输入箱长沿 X，输入箱宽沿 Y。
		Degrees90  // 输入箱宽沿 X，输入箱长沿 Y。
	};

	// 码垛输入约束；不考虑箱子高度及多层堆叠。
	struct PalletizingInput
	{
		std::int64_t palletLength = 0;     // 托盘沿 X 方向的尺寸，必须为正。
		std::int64_t palletWidth = 0;      // 托盘沿 Y 方向的尺寸，必须为正。
		std::int64_t boxLength = 0;        // 箱子未旋转时沿 X 方向的尺寸，必须为正。
		std::int64_t boxWidth = 0;         // 箱子未旋转时沿 Y 方向的尺寸，必须为正。
		std::int64_t availableBoxes = 0;   // 可用箱子数量上限，混合朝向方案至少需要两个箱子。
		std::int64_t outlineTolerance = 0; // 两种朝向条带的排列长度允许差值，必须非负。
	};

	// 单箱摆放结果：以托盘一角为原点，输出箱子中心坐标；布局不做整体居中。
	struct BoxPlacement
	{
		double centerX = 0.0;             // 箱子中心的 X 坐标。
		double centerY = 0.0;             // 箱子中心的 Y 坐标。
		std::int64_t length = 0;          // 摆放后沿 X 方向的尺寸，已包含旋转效果。
		std::int64_t width = 0;           // 摆放后沿 Y 方向的尺寸，已包含旋转效果。
		Orientation orientation = Orientation::Degrees0; // 箱子的摆放朝向。
	};

	// 码垛结果；先检查 success，失败时通过 message 获取原因。
	struct PalletizingOutput
	{
		bool success = false;                   // 是否成功生成满足约束的方案。
		const char* message = "";               // 状态说明，指向静态字符串，无需调用方释放。
		std::int64_t boxCount = 0;               // 当前单层实际摆放的箱子总数。
		double coverage = 0.0;                  // 箱子总面积 / 托盘面积 × 100，单位为百分数。
		std::int64_t occupiedLength = 0;         // 布局外接包围矩形沿 X 方向的尺寸。
		std::int64_t occupiedWidth = 0;          // 布局外接包围矩形沿 Y 方向的尺寸。
		std::int64_t outlineError = 0;           // 两种朝向条带的排列长度差的绝对值，非托盘余量。
		std::vector<BoxPlacement> placements;   // 逐箱摆放信息，成功时数量等于 boxCount。
	};

	// 内部单箱位置，使用箱子最小 X、Y 角点坐标，而非中心坐标。
	struct InternalPlacement
	{
		std::int64_t x = 0;                     // 箱子在 X 方向的起始坐标。
		std::int64_t y = 0;                     // 箱子在 Y 方向的起始坐标。
		std::int64_t length = 0;                // 摆放后沿 X 方向的尺寸。
		std::int64_t width = 0;                 // 摆放后沿 Y 方向的尺寸。
		Orientation orientation = Orientation::Degrees0; // 箱子的摆放朝向。
	};

	// 同一朝向箱子紧密排列形成的条带；排列方向与条带堆叠方向相互垂直。
	struct Strip
	{
		Orientation orientation = Orientation::Degrees0; // 条带内所有箱子的朝向。
		std::int64_t offset = 0;                // 条带在堆叠方向上的起始偏移。
		std::int64_t thickness = 0;             // 条带在堆叠方向上占用的尺寸。
		std::int64_t occupiedLength = 0;        // 条带沿排列方向的总长，不一定沿 X。
		std::int64_t boxCount = 0;              // 此条带包含的箱子数量。
	};

	// 内部候选布局；条带信息确定后，再生成逐箱位置。
	struct Layout
	{
		std::vector<Strip> strips;              // 按堆叠顺序存放的条带。
		std::vector<InternalPlacement> placements; // 生成后的逐箱角点位置。
		bool stripsAlongX = true;              // true：沿 X 排箱、沿 Y 堆叠；false：相反。
		std::int64_t boxCount = 0;              // 所有条带的箱子总数。
		std::int64_t occupiedLength = 0;        // 布局包围矩形的 X 向尺寸。
		std::int64_t occupiedWidth = 0;         // 布局包围矩形的 Y 向尺寸。
		std::int64_t outlineError = 0;          // 最长条带与最短条带的排列长度差。
	};
public:
	// 搜索两个排列方向的条带方案，不超出托盘且不超过可用箱数。
	// 依次优先：箱数更多、轮廓误差更小、包围面积更小、占用长度更大、沿 X 排列。
	PalletizingOutput Calculate(const PalletizingInput& input) const;
	// 返回布局包围矩形中心对称后的副本，相当于绕该中心旋转 180°。
	// 中心坐标变为 (occupiedLength - centerX, occupiedWidth - centerY)，其他字段不变。
	PalletizingOutput MirrorOutput(const PalletizingOutput& output) const;
private:
	// 检查正尺寸的面积乘积能否由 int64_t 安全表示。
	static bool IsSafeArea(std::int64_t length, std::int64_t width);
	// 按 Calculate 的优先级判断候选布局是否优于当前最优布局。
	static bool IsBetterLayout(const Layout& candidate, const Layout& best);
	// 检查条带连续性、尺寸、边界、箱数、混合朝向及轮廓公差。
	static bool IsValidLayout(const Layout& layout, std::int64_t palletLength, std::int64_t palletWidth,
		std::int64_t boxLength, std::int64_t boxWidth, std::int64_t outlineTolerance);
	// 根据条带生成逐箱角点位置及旋转后的尺寸，覆盖原有 placements。
	static void GeneratePlacements(Layout& layout, std::int64_t boxLength, std::int64_t boxWidth);
	// 向 candidates 追加指定排列方向的有效候选方案。
	// runLimit 为排列方向上限，stackLimit 为堆叠方向上限；
	// run0/thickness0、run90/thickness90 分别为两种朝向箱子的排列尺寸和堆叠尺寸。
	static void AddStripCandidates(std::int64_t runLimit, std::int64_t stackLimit,
		std::int64_t run0, std::int64_t thickness0,
		std::int64_t run90, std::int64_t thickness90,
		std::int64_t availableBoxes, std::int64_t outlineTolerance, bool stripsAlongX,
		std::vector<Layout>& candidates);
};