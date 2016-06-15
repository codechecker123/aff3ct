#ifndef ENCODER_REPETITION_SYS_HPP_
#define ENCODER_REPETITION_SYS_HPP_

#include <vector>
#include "../../Tools/MIPP/mipp.h"

#include "../Encoder_sys.hpp"

template <typename B>
class Encoder_repetition_sys : public Encoder_sys<B>
{
protected:
	const int K;         // info bits
	const int N;         // code length
	const int rep_count; // number of repetition

	const bool buffered_encoding;
	
public:
	Encoder_repetition_sys(const int& K, const int& N, const bool buffered_encoding = true);
	virtual ~Encoder_repetition_sys() {}

	virtual void encode_sys(const mipp::vector<B>& U_K, mipp::vector<B>& par);
	virtual void encode    (const mipp::vector<B>& U_K, mipp::vector<B>& X_N);
};

#endif // ENCODER_REPETITION_SYS_HPP_
