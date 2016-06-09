#ifndef FACTORY_CRC_HPP
#define FACTORY_CRC_HPP

#include "../../CRC/CRC.hpp"

#include "../params.h"

template <typename B>
struct Factory_CRC
{
	static CRC<B>* build(const t_code_param &code_params, const t_decoder_param &deco_params);
};

#endif /* FACTORY_CRC_HPP */