#ifndef FACTORY_INTERLEAVER_CORE_HPP
#define FACTORY_INTERLEAVER_CORE_HPP

#include <string>

#include "Tools/Interleaver/Interleaver_core.hpp"

#include "../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Interleaver_core_name;
extern const std::string Interleaver_core_prefix;
struct Interleaver_core : public Factory
{
	class parameters : public Factory::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// required parameters
		int         size     = 0;

		// optional parameters
		std::string type     = "RANDOM";
		std::string path     = "";
		int         n_cols   = 4; // number of columns of the columns interleaver
		int         n_frames = 1;
		int         seed     = 0;
		bool        uniform  = false; // set at true to regenerate the interleaver at each new frame

		// ---------------------------------------------------------------------------------------------------- METHODS
		explicit parameters(const std::string &p = Interleaver_core_prefix);
		virtual ~parameters();
		Interleaver_core::parameters* clone() const;

		// parameters construction
		void get_description(arg_map &req_args, arg_map &opt_args                              ) const;
		void store          (const arg_val_map &vals                                           );
		void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename T = uint32_t>
		tools::Interleaver_core<T>* build() const;
	};

	template <typename T = uint32_t>
	static tools::Interleaver_core<T>* build(const parameters &params);
};
}
}

#endif /* FACTORY_INTERLEAVER_CORE_HPP */
