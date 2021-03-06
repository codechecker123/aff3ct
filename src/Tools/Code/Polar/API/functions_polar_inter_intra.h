#ifndef FUNCTIONS_POLAR_INTER_INTRA_H_
#define FUNCTIONS_POLAR_INTER_INTRA_H_

#include <algorithm>
#ifdef _MSC_VER
#include <iterator>
#endif
#include <mipp.h>

#include "Tools/Math/utils.h"
#include "Tools/Code/Polar/decoder_polar_functions.h"

namespace aff3ct
{
namespace tools
{
// ========================================================================================================= saturate()
// ====================================================================================================================
// ====================================================================================================================

template <typename T>
struct API_polar_inter_intra_saturate
{
	static mipp::reg init()
	{
		return mipp::set0<T>();
	}

	static mipp::reg perform(const mipp::reg r_lambda, const mipp::reg r_sat)
	{
		return r_lambda;
	}
};

template <>
struct API_polar_inter_intra_saturate<signed char>
{
	static mipp::reg init()
	{
		return mipp::set1<signed char>(-127);
	}

	static mipp::reg perform(const mipp::reg r_lambda, const mipp::reg r_sat)
	{
		return mipp::max<signed char>(r_lambda, r_sat);
	}
};

// ================================================================================================================ f()
// ====================================================================================================================
// ====================================================================================================================

template <typename R, proto_f_i<R> FI, int N_ELMTS = 0, int N_FRAMES = 1>
struct f_inter_intra
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, R *__restrict l_c, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = FI(r_lambda_a, r_lambda_b);
			mipp::store<R>(l_c +i, r_lambda_c);
		}
	}
};

template <typename R, proto_f_i<R> FI, int N_FRAMES>
struct f_inter_intra <R, FI, 0, N_FRAMES>
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, R *__restrict l_c, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * N_FRAMES;

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = FI(r_lambda_a, r_lambda_b);
			mipp::store<R>(l_c +i, r_lambda_c);
		}
	}
};

// ================================================================================================================ g()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, typename R, proto_g_i<B,R> GI, int N_ELMTS = 0, int N_FRAMES = 1>
struct g_inter_intra
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, const B *__restrict s_a, R *__restrict l_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_u        = mipp::load<B>(s_a +i);
			const auto r_lambda_c = GI(r_lambda_a, r_lambda_b, r_u);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

template <typename B, typename R, proto_g_i<B,R> GI, int N_FRAMES>
struct g_inter_intra <B, R, GI, 0, N_FRAMES>
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, const B *__restrict s_a, R *__restrict l_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_u        = mipp::load<B>(s_a +i);
			const auto r_lambda_c = GI(r_lambda_a, r_lambda_b, r_u);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

// =============================================================================================================== g0()
// ====================================================================================================================
// ====================================================================================================================

template <typename R, proto_g0_i<R> G0I, int N_ELMTS = 0, int N_FRAMES = 1>
struct g0_inter_intra
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, R *__restrict l_c, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = G0I(r_lambda_a, r_lambda_b);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

template <typename R, proto_g0_i<R> G0I, int N_FRAMES>
struct g0_inter_intra <R,G0I,0,N_FRAMES>
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, R *__restrict l_c, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = G0I(r_lambda_a, r_lambda_b);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

// =============================================================================================================== gr()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, typename R, proto_g_i<B,R> GI, int N_ELMTS = 0, int N_FRAMES = 1>
struct gr_inter_intra
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, const B *__restrict s_a, R *__restrict l_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		const auto r_u = mipp::load<B>(s_a);
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = GI(r_lambda_a, r_lambda_b, r_u);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

template <typename B, typename R, proto_g_i<B,R> GI, int N_FRAMES>
struct gr_inter_intra <B,R,GI,0,N_FRAMES>
{
	static void apply(const R *__restrict l_a, const R *__restrict l_b, const B *__restrict s_a, R *__restrict l_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * N_FRAMES;

		const auto r_sat = API_polar_inter_intra_saturate<R>::init();

		const auto r_u = mipp::load<B>(s_a);
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_lambda_b = mipp::load<R>(l_b +i);
			const auto r_lambda_c = GI(r_lambda_a, r_lambda_b, r_u);

			mipp::store<R>(l_c +i, API_polar_inter_intra_saturate<R>::perform(r_lambda_c, r_sat));
		}
	}
};

// ================================================================================================================ h()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, typename R, proto_h_i<B,R> HI, int N_ELMTS = 0, int N_FRAMES = 1>
struct h_inter_intra
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_u        = HI(r_lambda_a);
			mipp::store<B>(s_a +i, r_u);
		}
	}
};

template <typename B, typename R, proto_h_i<B,R> HI, int N_FRAMES>
struct h_inter_intra <B,R,HI,0,N_FRAMES>
{
	static void apply(const R *__restrict l_a, B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<R>();
		const     auto _n_elmts = n_elmts * N_FRAMES;
		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_lambda_a = mipp::load<R>(l_a +i);
			const auto r_u        = HI(r_lambda_a);
			mipp::store<B>(s_a +i, r_u);
		}
	}
};

// =============================================================================================================== h0()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, int N_ELMTS = 0, int N_FRAMES = 1>
struct h0_inter_intra
{
	static void apply(B *__restrict s_a, const int n_elmts = 0)
	{
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;
		std::fill(s_a, s_a + _n_elmts, 0);
	}
};

template <typename B, int N_FRAMES>
struct h0_inter_intra <B,0,N_FRAMES>
{
	static void apply(B *__restrict s_a, const int n_elmts = 0)
	{
		const auto _n_elmts = n_elmts * N_FRAMES;
		std::fill(s_a, s_a + _n_elmts, 0);
	}
};

// ================================================================================================================ x()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, proto_xo_i<B> XOI, int N_ELMTS = 0, int N_FRAMES = 1>
struct xo_inter_intra
{
	static void apply(const B *__restrict s_a, const B *__restrict s_b, B *__restrict s_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<B>();
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_u_a = mipp::load<B>(s_a +i);
			const auto r_u_b = mipp::load<B>(s_b +i);
			const auto r_u_c = XOI(r_u_a, r_u_b);
			mipp::store<B>(s_c +i, r_u_c);
		}
	}
};

template <typename B, proto_xo_i<B> XOI, int N_FRAMES>
struct xo_inter_intra <B,XOI,0,N_FRAMES>
{
	static void apply(const B *__restrict s_a, const B *__restrict s_b, B *__restrict s_c,
	                  const int n_elmts = 0)
	{
		constexpr auto stride = mipp::nElmtsPerRegister<B>();
		const     auto _n_elmts = n_elmts * N_FRAMES;

		for (auto i = 0; i < _n_elmts; i += stride)
		{
			const auto r_u_a = mipp::load<B>(s_a +i);
			const auto r_u_b = mipp::load<B>(s_b +i);
			const auto r_u_c = XOI(r_u_a, r_u_b);
			mipp::store<B>(s_c +i, r_u_c);
		}
	}
};

// =============================================================================================================== x0()
// ====================================================================================================================
// ====================================================================================================================

template <typename B, int N_ELMTS = 0, int N_FRAMES = 1>
struct xo0_inter_intra
{
	static void apply(const B *__restrict s_b, B *__restrict s_c, const int n_elmts = 0)
	{
		constexpr auto _n_elmts = N_ELMTS * N_FRAMES;
#ifdef _MSC_VER
		std::copy(s_b, s_b + _n_elmts, stdext::checked_array_iterator<B*>(s_c, _n_elmts));
#else
		std::copy(s_b, s_b + _n_elmts, s_c);
#endif
	}
};

template <typename B, int N_FRAMES>
struct xo0_inter_intra <B,0,N_FRAMES>
{
	static void apply(const B *__restrict s_b, B *__restrict s_c, const int n_elmts = 0)
	{
		const auto _n_elmts = n_elmts * N_FRAMES;
#ifdef _MSC_VER
		std::copy(s_b, s_b + _n_elmts, stdext::checked_array_iterator<B*>(s_c, _n_elmts));
#else
		std::copy(s_b, s_b + _n_elmts, s_c);
#endif
	}
};
}
}

#endif /* FUNCTIONS_POLAR_INTER_INTRA_H_ */
