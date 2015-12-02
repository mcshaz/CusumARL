//https://git.imaginary.ca/eris/creativity/blob/94de1984921061e6d1b5dead904a1fb603832d1b/cli.cpp

#include <regex>

// Returns a double converted to a string, trimming off insignificant 0s.
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
std::string output_string(T v) {
	return std::to_string(v);
}

template <>
std::string output_string(double v) {
	std::string z = std::to_string(v);
	std::regex trim_trailing("(\\.\\d*?)0+$"), trim_empty_dot("\\.$");
	return std::regex_replace(std::regex_replace(z, trim_trailing, "$1"), trim_empty_dot, "");
}

enum RangeBounds { excludeBounds = 0, includeMin = 1, incudeMax = 2, includeBounds = 3 };
/// Constraint that takes a numeric minimum (and optionally a maximum); the argument must be &gt;= min
/// and &lt;= max.
template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
class RangeConstraint : public TCLAP::Constraint<T> {
public:
	RangeConstraint() = default; // Bounded only by data type limits
	RangeConstraint(T minimum, T maximum, RangeBounds bounds = RangeBounds::includeBounds) 
		: min(minimum), max(maximum), inclMin(((int)bounds & (int)RangeBounds::includeMin)>0), inclMax(((int)bounds & (int)RangeBounds::incudeMax)>0) {}
	static RangeConstraint LE(T maximum) { return RangeConstraint(LOWEST, maximum); }
	static RangeConstraint GE(T minimum) { return RangeConstraint(minimum, BIGGEST); }
	static RangeConstraint LT(T maximum) { return RangeConstraint(LOWEST, maximum, RangeBounds::excludeBounds); }
	static RangeConstraint GT(T minimum) { return RangeConstraint(minimum, BIGGEST, RangeBounds::excludeBounds); }
	inline 
	virtual std::string description() const override {
		if (max == BIGGEST)
			return ("must be " + minStr);
		else if (min == LOWEST)
			return ("must be " + maxStr);
		else
			return ( "must be "
			+ minStr + " and " + maxStr);
	}
	virtual std::string shortID() const override {
		std::string type =
			std::is_floating_point<T>::value ? "R" /* u8"?" u8"??" */ :
			std::is_integral<T>::value ? (std::is_unsigned<T>::value ? "N":"Z" /* "?": "?" u8"??" : u8"??" */) :
			"v"; // fallback
		bool show_min = (min != LOWEST || LOWEST == 0),
			show_max = (max != BIGGEST);
		return
			(show_min && show_max) ? type + lt_str + output_string(min) + ',' + output_string(max) + gt_str :
			(show_min) ? type + lt_str + output_string(min) + ',' + gt_str:
			(show_max) ? type + lt_str + ',' + output_string(max) + gt_str:
			type;
			/*
			(show_min && show_max) ? output_string(min) + lt_str + type + lt_str + output_string(max) :
			(show_min) ? type + gt_str + output_string(min) :
			(show_max) ? type + lt_str + output_string(max) :
			type;
			*/
	}
	virtual bool check(const T &val) const override {
		return (inclMin ? val >= min : val > min)
			&& (inclMax ? val <= max : val < max);
	}
	static constexpr T LOWEST = std::numeric_limits<T>::has_infinity ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::lowest();
	static constexpr T BIGGEST = std::numeric_limits<T>::has_infinity ? std::numeric_limits<T>::infinity() : std::numeric_limits<T>::max();
	const T min = LOWEST;
	const T max = BIGGEST;
	const bool inclMin;
	const bool inclMax;
	const std::string lt_str{ inclMin ? "[":"(" /* "<" : u8"?" */},
		gt_str{ inclMax ? "]":")"/* ">" : u8"?" */ },
		minStr {
			(inclMin ? "at least " : "greater than ") + output_string(min)
		},
		maxStr {
			(inclMax ? "at most " : "less than ") + output_string(max)
		};

};