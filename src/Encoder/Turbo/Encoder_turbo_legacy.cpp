#include <cassert>
#include <vector>
#include <cmath>

#include "Encoder_turbo_legacy.hpp"

template <typename B>
Encoder_turbo_legacy<B>
::Encoder_turbo_legacy(const int& K, const int& N, const Interleaver<short> &pi, const int n_frames)
: K(K),
  N(N),
  pi(pi),
  n_frames(n_frames),
  rsc(K, 2*K, n_frames, false),
  U_K_i(K*n_frames),
  X_N_n((2 * (K + rsc.get_n_states()))*n_frames),
  X_N_i((2 * (K + rsc.get_n_states()))*n_frames)
{
	assert(n_frames > 0);
}

template <typename B>
void Encoder_turbo_legacy<B>
::encode(const mipp::vector<B>& U_K, mipp::vector<B>& X_N)
{
	assert(U_K.size() == (unsigned) (K * n_frames));
	assert(X_N.size() == (unsigned) ((N + (4 * rsc.get_n_states())) * n_frames));
	assert((N / K) == 3);

	pi.interleave(U_K, U_K_i);
	rsc.encode(U_K,   X_N_n);
	rsc.encode(U_K_i, X_N_i);

	for (auto f = 0; f < n_frames; f++)
	{
		const auto off1 = (3*K + (4 * rsc.get_n_states())) * f;
		const auto off2 = (2*K + (2 * rsc.get_n_states())) * f;
		for (auto i = 0; i < K; i++)
		{
			X_N[off1 + 3*i +0] = X_N_n[off2 + 2*i +0];
			X_N[off1 + 3*i +1] = X_N_n[off2 + 2*i +1];
			X_N[off1 + 3*i +2] = X_N_i[off2 + 2*i +1];
		}

		const auto off1_tails_n = off1 + 3*K;
		const auto off2_tails_n = off2 + 2*K;
		for (auto i = 0; i < rsc.get_n_states(); i++)
		{
			X_N[off1_tails_n + 2*i +0] = X_N_n[off2_tails_n + 2*i +0];
			X_N[off1_tails_n + 2*i +1] = X_N_n[off2_tails_n + 2*i +1];
		}

		const auto off1_tails_i = off1_tails_n + 2*rsc.get_n_states();
		const auto off2_tails_i = off2_tails_n;
		for (auto i = 0; i < rsc.get_n_states(); i++)
		{
			X_N[off1_tails_i + 2*i +0] = X_N_i[off2_tails_i + 2*i +0];
			X_N[off1_tails_i + 2*i +1] = X_N_i[off2_tails_i + 2*i +1];
		}
	}
}

// ==================================================================================== explicit template instantiation 
#include "../../Tools/types.h"
#ifdef MULTI_PREC
template class Encoder_turbo_legacy<B_8>;
template class Encoder_turbo_legacy<B_16>;
template class Encoder_turbo_legacy<B_32>;
template class Encoder_turbo_legacy<B_64>;
#else
template class Encoder_turbo_legacy<B>;
#endif
// ==================================================================================== explicit template instantiation