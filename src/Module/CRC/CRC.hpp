/*!
 * \file
 * \brief Adds/builds and checks a Cyclic Redundancy Check (CRC) for a set of information bits.
 *
 * \section LICENSE
 * This file is under MIT license (https://opensource.org/licenses/MIT).
 */
#ifndef CRC_HPP_
#define CRC_HPP_

#include <string>
#include <vector>
#include <stdexcept>

#include "Tools/Perf/MIPP/mipp.h"

#include "Module/Module.hpp"

namespace aff3ct
{
namespace module
{
/*!
 * \class CRC_i
 *
 * \brief Adds/builds and checks a Cyclic Redundancy Check (CRC) for a set of information bits.
 *
 * \tparam B: type of the bits in the CRC.
 *
 * Please use CRC for inheritance (instead of CRC_i).
 */
template <typename B = int>
class CRC_i : public Module
{
protected:
	const int K; /*!< Number of information bits (the CRC bits are included in K) */

public:
	/*!
	 * \brief Constructor.
	 *
	 * \param K:        number of information bits (the CRC bits are included in K).
	 * \param n_frames: number of frames to process in the CRC.
	 * \param name:     CRC's name.
	 */
	CRC_i(const int K, const int n_frames = 1, const std::string name = "CRC_i")
	: Module(n_frames, name), K(K)
	{
		if (K <= 0)
			throw std::invalid_argument("aff3ct::module::CRC: \"K\" has to be greater than 0.");
	}

	/*!
	 * \brief Destructor.
	 */
	virtual ~CRC_i()
	{
	}

	/*!
	 * \brief Gets the size of the CRC (the number of bits for the CRC signature).
	 *
	 * \return the size of the CRC.
	 */
	virtual int size() const = 0;

	/*!
	 * \brief Computes and adds the CRC in the vector of information bits (the CRC bits are often put at the end of the
	 *        vector).
	 *
	 * \param U_K: a vector (size = K - CRC<B>::size()) containing the information bits, adds "CRC<B>::size()" bits in
	 *             U_K.
	 */
	void build(mipp::vector<B>& U_K)
	{
		if (this->K * this->n_frames != (int)U_K.size())
			throw std::length_error("aff3ct::module::CRC: \"U_K.size()\" has to be equal to \"K\" * \"n_frames\".");

		this->build(U_K.data());
	}

	virtual void build(B *U_K)
	{
		for (auto f = 0; f < this->n_frames; f++)
			this->_build(U_K + f * this->K);
	}

	/*!
	 * \brief Checks if the CRC is verified or not.
	 *
	 * \param V_K:      a vector containing information bits plus the CRC bits.
	 * \param n_frames: you should not use this parameter unless you know what you are doing, this parameter
	 *                  redefine the number of frames to check specifically in this method.
	 *
	 * \return true if the CRC is verified, false otherwise.
	 */
	bool check(const mipp::vector<B>& V_K, const int n_frames = -1)
	{
		if (this->K * n_frames != (int)V_K.size() || this->K * this->n_frames != (int)V_K.size())
			throw std::length_error("aff3ct::module::CRC: \"V_K.size()\" has to be equal to \"K\" * \"n_frames\".");

		if (n_frames <= 0 && n_frames != -1)
			throw std::invalid_argument("aff3ct::module::CRC: \"n_frames\" has to be greater than 0 (or equal "
			                            "to -1).");

		return this->check(V_K.data(), n_frames);
	}

	virtual bool check(const B *V_K, const int n_frames = -1)
	{
		const int real_n_frames = (n_frames != -1) ? n_frames : this->n_frames;

		auto f = 0;
		while (f < real_n_frames && this->_check(V_K + f * this->K))
			f++;

		return f == real_n_frames;
	}

protected:
	virtual bool _build(const B *V_K)
	{
		throw std::runtime_error("aff3ct::module::CRC: \"_build\" is unimplemented.");
		return false;
	}

	virtual bool _check(const B *V_K)
	{
		throw std::runtime_error("aff3ct::module::CRC: \"_check\" is unimplemented.");
		return false;
	}
};
}
}

#include "SC_CRC.hpp"

#endif
