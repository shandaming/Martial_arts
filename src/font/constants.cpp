#include "constants.h"

namespace font
{
	constexpr int SIZE_NORMAL = 14;
	
	constexpr int SIZE_TINY = 10 * SIZE_NORMAL / 14,
			SIZE_SMALL = 12 * SIZE_NORMAL / 14,
			SIZE_15 = 15 * SIZE_NORMAL / 14,
			SIZE_PLUS = 16 * SIZE_NORMAL / 14,
			SIZE_LARGE = 18 * SIZE_NORMAL / 14,
			SIZE_TITLE = 20 * SIZE_NORMAL / 14,
			SIZE_XLARGE = 24 * SIZE_NORMAL / 14;

	constexpr size_t max_text_line_width = 4096;

	constexpr std::string ellipsis = "...",
			  unicode_minus = "-",
			  unicode_en_dash = "_",
			  unicode_em_dash = "-",
			  unicode_figure_dash = "-",
			  unicode_multiplication_sign = "x",
			  unicode_bullet = ".",
			  weapon_numbers_sep = "x",
			  weapon_details_sep = "-";

}
