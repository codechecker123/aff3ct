#ifndef FUNCTIONS_POLAR_INTER_H_
#define FUNCTIONS_POLAR_INTER_H_

#include <algorithm>
#include <mipp.h>

#include "Tools/Math/utils.h"
#include "Tools/Code/Polar/decoder_polar_functions.h"

#include "functions_polar_inter_intra.h"

namespace aff3ct
{
namespace tools
{
// ============================================================================================================== rep()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, typename R, proto_h_i<B,R> HI, int N_ELMTS = 0>
struct rep_inter
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * mipp::nElReg<R>();

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		auto r_sum_l = mipp::set0<R>();
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			r_sum_l = mipp::add<R>(API_polar_inter_intra_saturate<R>::perform(r_lambda_a, r_sat), r_sum_l);
		}

		const auto r_r = HI(r_sum_l);
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			mipp::store<B>(s_a +i, r_r);
		}
	}
};


template <typename B, typename R, proto_h_i<B,R> HI>
struct rep_inter <B, R, HI, 0>
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * mipp::nElReg<R>();

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		auto r_sum_l = mipp::set0<R>();
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			r_sum_l = mipp::add<R>(API_polar_inter_intra_saturate<R>::perform(r_lambda_a, r_sat), r_sum_l);
		}

		const auto r_r = HI(r_sum_l);
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			mipp::store<B>(s_a +i, r_r);
		}
	}
};

// ============================================================================================================== spc()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, typename R, proto_h_i<B,R> HI, int N_ELMTS = 0>
struct spc_inter
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * mipp::nElReg<R>();

		// vectorized part of the SPC
		auto r_cur_min_abs = mipp::set1<R>(std::numeric_limits<R>::max());
		auto r_prod_sign   = mipp::set1<R>((R)1);

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_l_a     = mipp::load<R>(l_a +i);
			const auto r_s_a     = HI(r_l_a);
			const auto r_sign    = r_s_a;
			const auto r_abs_l_a = mipp::abs<R>(r_l_a);

			r_cur_min_abs = mipp::min<R>(r_cur_min_abs, r_abs_l_a);
			r_prod_sign   = mipp::xorb<R>(r_prod_sign, r_sign);

			mipp::store<B>(s_a +i, r_s_a);
			// mipp::store<R>(l_a +i, r_abs_l_a);
		}

#ifndef _MSC_VER
		R cur_min_abs[mipp::nElmtsPerRegister<R>()] __attribute__((aligned(MIPP_REQUIRED_ALIGNMENT)));
#else
		R cur_min_abs[mipp::nElmtsPerRegister<R>()];
#endif
		mipp::store<R>(cur_min_abs, r_cur_min_abs);

#ifndef _MSC_VER
		R prod_sign  [mipp::nElmtsPerRegister<R>()] __attribute__((aligned(MIPP_REQUIRED_ALIGNMENT)));
#else
		R prod_sign  [mipp::nElmtsPerRegister<R>()];
#endif
		mipp::store<R>(prod_sign,   r_prod_sign);

		// sequential part of the SPC
		for (auto i = 0; i < stride; i++)
		{
			if (prod_sign[i] < 0) // make the correction
			{
				auto j = 0;
				// while (l_a[j * stride + i] != cur_min_abs[i]) j++;
				while (std::abs(l_a[j * stride + i]) != cur_min_abs[i]) j++;
				s_a[j * stride + i] = (s_a[j * stride + i] == 0) ? bit_init<B>() : 0;
			}
		}
	}
};

template <typename B, typename R, proto_h_i<B,R> HI>
struct spc_inter <B, R, HI, 0>
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * mipp::nElReg<R>();

		// vectorized part of the SPC
		auto r_cur_min_abs = mipp::set1<R>(std::numeric_limits<R>::max());
		auto r_prod_sign   = mipp::set1<R>((R)1);

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_l_a     = mipp::load<R>(l_a +i);
			const auto r_s_a     = HI(r_l_a);
			const auto r_sign    = r_s_a;
			const auto r_abs_l_a = mipp::abs<R>(r_l_a);

			r_cur_min_abs = mipp::min<R>(r_cur_min_abs, r_abs_l_a);
			r_prod_sign   = mipp::xorb<R>(r_prod_sign, r_sign);

			mipp::store<B>(s_a +i, r_s_a);
			// mipp::store<R>(l_a +i, r_abs_l_a);
		}

#ifndef _MSC_VER
		R cur_min_abs[mipp::nElmtsPerRegister<R>()] __attribute__((aligned(MIPP_REQUIRED_ALIGNMENT)));
#else
		R cur_min_abs[mipp::nElmtsPerRegister<R>()];
#endif
		mipp::store<R>(cur_min_abs, r_cur_min_abs);
#ifndef _MSC_VER
		R prod_sign  [mipp::nElmtsPerRegister<R>()] __attribute__((aligned(MIPP_REQUIRED_ALIGNMENT)));
#else
		R prod_sign  [mipp::nElmtsPerRegister<R>()];
#endif
		mipp::store<R>(prod_sign,   r_prod_sign);

		// sequential part of the SPC
		for (auto i = 0; i < stride; i++)
		{
			if (prod_sign[i] < 0) // make the correction
			{
				auto j = 0;
				// while (l_a[j * stride + i] != cur_min_abs[i]) j++;
				while (std::abs(l_a[j * stride + i]) != cur_min_abs[i]) j++;
				s_a[j * stride + i] = (s_a[j * stride + i] == 0) ? bit_init<B>() : 0;
			}
		}
	}
};
}
}

#endif /* FUNCTIONS_POLAR_INTER_H_ */
