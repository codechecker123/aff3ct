#include "../../../Decoder/decoder_functions.h"

#include "Scaling_factor_constant.hpp"

template <typename R>
Scaling_factor_constant<R>
::Scaling_factor_constant(const int K, const int n_ite, const float alpha) 
: Scaling_factor<R>(K, n_ite), alpha(alpha)
{
}

template <typename R>
void Scaling_factor_constant<R>
::operator()(mipp::vector<R> &ext, const int ite) 
{
	const auto loop_size = (int)ext.size();

	for (auto i = 0; i < loop_size; i++)
		ext[i] *= alpha;
}

// ==================================================================================== explicit template instantiation 
#include "../../types.h"
#ifdef MULTI_PREC
template class Scaling_factor_constant<Q_8>;
template class Scaling_factor_constant<Q_16>;
template class Scaling_factor_constant<Q_32>;
template class Scaling_factor_constant<Q_64>;
#else
template class Scaling_factor_constant<Q>;
#endif
// ==================================================================================== explicit template instantiation