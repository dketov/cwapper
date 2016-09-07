#ifndef __RESTFUL_HPP
#define __RESTFUL_HPP

#include "cwapper.hpp"

namespace cwapper {
	class restful: public cppcms::application {
	public:
		restful(cppcms::service &);
	
{% for r in api.routes %}\
		void ${r.fid}(${r.signature});
{% for m in r.path.methods %}\
		virtual cwapper::data ${m.operationId}(${m.signature});
{% end %}
{% end %}\
{% end %}\
	private:
		cwapper::data body();
		cwapper::parameter parameter(std::string);
		void CORS();
		void __api__();	
	};
}

#endif
