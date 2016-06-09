#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include "Simulation_polar_debug.hpp"

template <typename B, typename R, typename Q>
Simulation_polar_debug<B,R,Q>
::Simulation_polar_debug(const t_simulation_param& simu_params,
                   const t_code_param&       code_params,
                   const t_encoder_param&    enco_params,
                   const t_channel_param&    chan_params,
                   const t_decoder_param&    deco_params)
: Simulation_polar<B,R,Q>(simu_params, code_params, enco_params, chan_params, deco_params)
{
}

template <typename B, typename R, typename Q>
Simulation_polar_debug<B,R,Q>
::~Simulation_polar_debug()
{
}

template <typename B, typename R, typename Q>
void Simulation_polar_debug<B,R,Q>
::simulation_loop()
{
	using namespace std::chrono;

	// simulation loop
	auto t_simu = steady_clock::now();
	while (!this->analyzer->fe_limit_achieved() && // while max frame error count has not been reached
	       (this->simu_params.stop_time == seconds(0) ||
	        (steady_clock::now() - this->t_snr) < this->simu_params.stop_time))
	{
		std::clog << "-------------------------------" << std::endl;
		std::clog << "New encoding/decoding session !" << std::endl;
		std::clog << "-------------------------------" << std::endl;

		// display frozen bits
		std::clog << "Frozen bits:" << std::endl;
		this->display_bit_vector(this->frozen_bits);
		std::clog << std::endl;

		auto d_sourc = nanoseconds(0);
		auto d_encod = nanoseconds(0);
		auto d_modul = nanoseconds(0);

		if (this->code_params.generation_method != "AZCW")
		{
			// generate a random K bits vector U_K
			std::clog << "Generate random bits U_K..." << std::endl;
			auto t_sourc = steady_clock::now();
			this->source->generate(this->U_K);
			d_sourc = steady_clock::now() - t_sourc;

			// display U_K
			std::clog << "U_K:" << std::endl;
			this->display_bit_vector(this->U_K);
			std::clog << std::endl;

			// add the CRC to U_K
			std::clog << "Add the CRC to U_K..." << std::endl;
			this->crc->build(this->U_K);

			// display U_K
			std::clog << "U_K:" << std::endl;
			this->display_bit_vector(this->U_K);
			std::clog << std::endl;

			// encode U_K into a N bits vector X_N
			std::clog << "Encode U_K in X_N..." << std::endl;
			auto t_encod = steady_clock::now();
			this->encoder->encode(this->U_K, this->U_N, this->X_N);
			d_encod = steady_clock::now() - t_encod;

			// display U_N
			std::clog << "U_N:" << std::endl;
			this->display_bit_vector(this->U_N);
			std::clog << std::endl;

			// display X_N
			std::clog << "X_N:" << std::endl;
			this->display_bit_vector(this->X_N);
			std::clog << std::endl;

			// modulate
			auto t_modul = steady_clock::now();
			this->modulator->modulate(this->X_N);
			d_modul = steady_clock::now() - t_modul;
		}
		else
		{
			// display U_K
			std::clog << "U_K:" << std::endl;
			this->display_bit_vector(this->U_K);
			std::clog << std::endl;

			// display U_N
			std::clog << "U_N:" << std::endl;
			this->display_bit_vector(this->U_N);
			std::clog << std::endl;

			// display X_N
			std::clog << "X_N:" << std::endl;
			this->display_bit_vector(this->X_N);
			std::clog << std::endl;
		}

		// add noise
		std::clog << "Add noise from X_N to Y_N1..." << std::endl;
		auto t_chann = steady_clock::now();
		this->channel->add_noise(this->X_N, this->Y_N1);
		auto d_chann = steady_clock::now() - t_chann;

		// display Y_N1
		std::clog << "Y_N1:" << std::endl;
		this->display_real_vector(this->Y_N1);
		std::clog << std::endl;

		// make the quantization
		std::clog << "Make the quantization from Y_N1 to Y_N2..." << std::endl;
		auto t_quant = steady_clock::now();
		this->quantizer->process(this->Y_N1, this->Y_N2);
		auto d_quant = steady_clock::now() - t_quant;

		// display Y_N2
		std::clog << "Y_N2:" << std::endl;
		this->display_quantized_vector(this->Y_N2);
		std::clog << std::endl;

		auto d_punct = nanoseconds(0);
		if (!this->code_params.puncturer.empty())
		{
			// puncture
			std::clog << "Puncture Y_N2..." << std::endl;
			auto t_punct = steady_clock::now();
			this->puncturer->puncture(this->Y_N2);
			d_punct = steady_clock::now() - t_punct;

			// display Y_N2
			std::clog << "Y_N2:" << std::endl;
			this->display_quantized_vector(this->Y_N2);
			std::clog << std::endl;
		}

		// load data in the decoder
		auto t_load = steady_clock::now();
		this->decoder->load(this->Y_N2);
		auto d_load = steady_clock::now() - t_load;
		
		// launch decoder
		std::clog << "Decode Y_N2 and generate V_N..." << std::endl;
		auto t_decod = steady_clock::now();
		this->decoder->decode();
		auto d_decod = steady_clock::now() - t_decod;

		// store results in V_N
		auto t_store = steady_clock::now();
		this->decoder->store(this->V_N);
		auto d_store = steady_clock::now() - t_store;

		// unpack V_N if we used bit packing in the decoder (do nothing else)
		this->decoder->unpack(this->V_N);

		// display V_N
		std::clog << "V_N:" << std::endl;
		this->display_bit_vector(this->V_N);
		std::clog << std::endl;

		// check errors in the frame
		auto t_check = steady_clock::now();
		this->analyzer->check_errors(this->U_N, this->V_N);
		auto d_check = steady_clock::now() - t_check;

		// increment total durations for each operations
		this->d_sourc_total += d_sourc;
		this->d_encod_total += d_encod;
		this->d_modul_total += d_modul;
		this->d_chann_total += d_chann;
		this->d_quant_total += d_quant;
		this->d_punct_total += d_punct;
		this->d_load_total  += d_load;
		this->d_decod_total += d_decod;
		this->d_store_total += d_store;
		this->d_check_total += d_check;

		// display statistics in terminal
		if (!this->simu_params.disable_display && this->simu_params.display_freq != nanoseconds(0) && 
		    (steady_clock::now() - t_simu) >= this->simu_params.display_freq)
		{
			this->terminal->temp_report(std::clog);
			t_simu = steady_clock::now();
		}
	}

	this->terminal->legend(std::clog);
}

template <typename B, typename R, typename Q>
void Simulation_polar_debug<B,R,Q>
::display_bit_vector(mipp::vector<B> vec)
{
	for(unsigned i = 0; i < vec.size(); i++)
		std::clog << std::setw(5) << i << "|";
	std::clog << std::endl;

	for(unsigned i = 0; i < vec.size(); i++)
		std::clog << std::setw(5) << ((vec[i] == 0) ? (int) 0 : (int) 1) << "|";
	std::clog << std::endl;
}

template <typename B, typename R, typename Q>
void Simulation_polar_debug<B,R,Q>
::display_real_vector(mipp::vector<R> vec)
{
	for(unsigned i = 0; i < vec.size(); i++)
		std::clog << std::setw(5) << i << "|";
	std::clog << std::endl;

	for(unsigned i = 0; i < vec.size(); i++)
		if(vec[i] >= 0)
			std::clog << std::setprecision(2) << std::setw(5) << vec[i] << "|";
		else
			std::clog << std::setprecision(1) << std::setw(5) << vec[i] << "|";
	std::clog << std::endl;
}

template <typename B, typename R, typename Q>
void Simulation_polar_debug<B,R,Q>
::display_quantized_vector(mipp::vector<Q> vec)
{
	for(unsigned i = 0; i < vec.size(); i++)
		std::clog << std::setw(5) << i << "|";
	std::clog << std::endl;

	for(unsigned i = 0; i < vec.size(); i++)
		if(vec[i] >= 0)
			std::clog << std::setprecision(2) << std::setw(5) << (int)vec[i] << "|";
		else
			std::clog << std::setprecision(1) << std::setw(5) << (int)vec[i] << "|";
	std::clog << std::endl;
}

// ==================================================================================== explicit template instantiation 
#include "../../../Tools/types.h"
#ifdef MULTI_PREC
template class Simulation_polar_debug<B_8,R_8,Q_8>;
template class Simulation_polar_debug<B_16,R_16,Q_16>;
template class Simulation_polar_debug<B_32,R_32,Q_32>;
template class Simulation_polar_debug<B_64,R_64,Q_64>;
#else
template class Simulation_polar_debug<B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation