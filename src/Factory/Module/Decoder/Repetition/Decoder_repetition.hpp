#ifndef FACTORY_DECODER_REPETITION_HPP
#define FACTORY_DECODER_REPETITION_HPP

#include <string>

#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Encoder/Encoder.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_repetition_name;
extern const std::string Decoder_repetition_prefix;
struct Decoder_repetition : public Decoder
{
	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// optional parameters
		bool buffered = true;

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Decoder_repetition_prefix);
		virtual ~parameters();
		Decoder_repetition::parameters* clone() const;

		// parameters construction
		void get_description(arg_map &req_args, arg_map &opt_args                              ) const;
		void store          (const arg_val_map &vals                                           );
		void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_SIHO<B,Q>* build(module::Encoder<B> *encoder = nullptr) const;
	};

	template <typename B = int, typename Q = float>
	static module::Decoder_SIHO<B,Q>* build(const parameters &params, module::Encoder<B> *encoder = nullptr);
};
}
}

#endif /* FACTORY_DECODER_REPETITION_HPP */
