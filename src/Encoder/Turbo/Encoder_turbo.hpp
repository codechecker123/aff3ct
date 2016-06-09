#ifndef ENCODER_TURBO_HPP_
#define ENCODER_TURBO_HPP_

#include <string>
#include <vector>
#include "../../Tools/MIPP/mipp.h"
#include "../../Interleaver/Interleaver.hpp"

#include "../Encoder.hpp"
#include "../Encoder_sys.hpp"

template <typename B>
class Encoder_turbo : public Encoder<B>
{
protected:
	const int K;                  // info bits
	const int N;                  // code length
	const Interleaver<short> &pi; // the interleaver
	const int n_frames;

	Encoder_sys<B> &enco_n;       // polar systematic encoder
	Encoder_sys<B> &enco_i;       // sub encoder
	
	mipp::vector<B> U_K_i;        // internal buffer for the systematic bits in the interleaved domain
	mipp::vector<B> par_n;        // internal buffer for the encoded    bits in the natural     domain
	mipp::vector<B> par_i;        // internal buffer for the encoded    bits in the interleaved domain

public:
	Encoder_turbo(const int& K, const int& N, const Interleaver<short> &pi,
	              Encoder_sys<B> &enco_n, Encoder_sys<B> &enco_i, const int n_frames = 1);
	virtual ~Encoder_turbo() {}

	virtual void encode(const mipp::vector<B>& U_K, mipp::vector<B>& X_N);

	int tail_length() const { return enco_n.tail_length() + enco_i.tail_length(); }
};

#endif // ENCODER_TURBO_HPP_