#ifndef AVERAGE_H
#define AVERAGE_H 1

#include <cmath> // for std::sqrt
#include <type_traits>

///
/// Functor for calculating mean/variance/std deviation of a sequence.
/// Works with both integral and floating point accumulators.
/// Note that you probably want to use a double as the underlying accumulator.
///
/// This class allows an algorithm like @ref std::for_each to operate on
/// an iterator
/// range.
///
template <class ACCUM_T, class COUNT_T = unsigned long>
class Average
{
 public:
	static_assert(std::is_floating_point<ACCUM_T>::value,
	              "Accumulator type must be floating point");

	///
	/// type of the average and variance accumulators
	///
	typedef ACCUM_T accumulator_type;
	
	///
	/// type of the samples counter
	///
	typedef COUNT_T counter_type;

	///
	/// Default constructor
	///
	Average();

	/// Copy constructor - defaulted behavior
	Average(const Average &) = default;

	/// Move constructor - defaulted behavior
	Average(Average &&) = default;

	/// Destructors - defaulted behavior
	virtual ~Average() = default;

	/// Assignment operators - defaulted behavior
	Average & operator = (const Average &) = default;

	/// Assignment operators - defaulted behavior
	Average & operator = (Average &&) = default;
	
	///
	/// @brief Performs the magic of calculating averages and variance
	/// in a stepwise manner.
	///
	/// @param value A value to add to the running average. The type of value
	///   must have a means of being cast to the type @ref accumulator_type
	///
	template<typename T>
	void operator () (const T & value);

	///
	/// Returns the mean(average) of the values iterated over
	///
	accumulator_type mean() const;

	///
	/// Returns a count of the values iterated over
	///
	counter_type numSamples() const;

	///
	/// Returns the sample variance:
	/// \f{eqnarray*}{
	/// \sigma^2 = \frac{1}{N - 1}\sum\limits_{i=0}^{N}{(x_i - \mu)^2}
	/// \f}
	///
	accumulator_type sampleVariance() const;

	///
	/// Returns the population variance:
	/// \f{eqnarray*}{
	/// \sigma^2 = \frac{1}{N}\sum\limits_{i=0}^{N}{(x_i - \mu)^2}
	/// \f}
	///
	accumulator_type populationVariance() const;

	///
	/// Returns the square root of the sample variance, or:
	/// \f{eqnarray*}{
	/// \sigma = \sqrt{\frac{1}{N - 1}\sum\limits_{i=0}^{N}{(x_i - \mu)^2}}
	/// \f}
	///
	accumulator_type standardDeviation() const;

	///
	/// Returns the minimum value that was added to the average
	///
	accumulator_type minima() const;
	
	///
	/// Returns the maximum value that was added to the average
	///
	accumulator_type maxima() const;

	///
	/// Resets the state
	///
	void reset();

 protected:
	counter_type samples;                 ///< Number of samples
	accumulator_type meanAccumulator;     ///< always set to the current mean
	accumulator_type varianceAccumulator; ///< Variance accumulator
	accumulator_type min;                 ///< Minimum value encountered
	accumulator_type max;                 ///< Maximum value encountered
};

//
// Function definitions
//

template<class A, class C>
Average<A, C>::Average()
  : samples(0)
  , meanAccumulator(0)
  , varianceAccumulator(0)
  , min(0)
  , max(0)
{
}

#include <iostream>

/// This function implements the following formulas for the
/// mean and variance; they are designed to minimize rounding errors
/// by using the following formulas:
//
//                           (sample[k] - Mean[k - 1])
//   Mean[k] = Mean[k - 1] = -------------------------
//                                      k
//
//                           (k - 1) * (sample[k] - Mean[k - 1])^2
//   VSum[k] = VSum[k - 1] + -------------------------------------
//                                           k
//
/**	\f{eqnarray*}{
///	Mean_k &=& Mean_{k-1} + \frac{sample_k - Mean_{k - 1}}{k}\\
///	Variance_k &=& Variance_{k-1} +
///   \frac{(k-1) (sample_k - Mean_{k-1}) ^ 2}{k}
///	\f}
**/
/// With these running sums, the mean is calculated at each step
/// and the sum-of-variances is calculated at each step as well.
/// The actual variance is then available by taking
///   \f$\sigma^2 = \frac{Variance_k}{k}\f$ or
///   \f$s^2 = \frac{Variance_k}{k - 1}\f$.
///
/// The translation of this into code makes a few optimizations, notably:
///    * variance calculation is done before mean
///    * \f$(sample_k - Mean_{k-1})\f$ is computed once at the beginning
///    * \f$k\f$ is incremented halfway through the VSum calculation,
///      after \f$k - 1\f$ is used
///
template<class A, class C>
template<typename T>
void Average<A, C>::operator () (const T & value)
{
	if (samples > 0)
	{
		if (min > accumulator_type(value))
			min = accumulator_type(value);

		if (max < accumulator_type(value))
			max = accumulator_type(value);
	} else
	{
		min = max = accumulator_type(value);
	}

	accumulator_type difference = (accumulator_type(value) - meanAccumulator);

	accumulator_type tmp = samples * difference * difference;

	tmp /= ++samples;

	varianceAccumulator += tmp;

	meanAccumulator += difference / samples;
}

template<class A, class C>
typename Average<A, C>::accumulator_type Average<A, C>::mean() const
	{ return meanAccumulator; }

template<class A, class C>
typename Average<A, C>::counter_type Average<A, C>::numSamples() const
	{ return samples; }

template<class A, class C>
typename Average<A, C>::accumulator_type
Average<A, C>::sampleVariance() const
	{ return (varianceAccumulator / accumulator_type(samples - 1)); }

template<class A, class C>
typename Average<A, C>::accumulator_type
Average<A, C>::populationVariance() const
	{ return (varianceAccumulator / accumulator_type(samples)); }

template<class A, class C>
typename Average<A, C>::accumulator_type
Average<A, C>::standardDeviation() const
	{ return std::sqrt(sampleVariance()); }

template<class A, class C>
typename Average<A, C>::accumulator_type Average<A, C>::minima() const
	{ return min; }

template<class A, class C>
typename Average<A, C>::accumulator_type Average<A, C>::maxima() const
	{ return max; }

template<class A, class C>
void Average<A, C>::reset()
{
	samples = 0;
	meanAccumulator = 0;
	varianceAccumulator = 0;
	min = 0;
	max = 0;
}

#endif // AVERAGE_H
