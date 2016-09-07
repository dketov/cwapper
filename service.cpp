/*
${api.description}
*/

#include "cwapper.hpp"
#include "restful.hpp"

class service: public cwapper::restful {
public:
	service(cppcms::service &srv): restful::restful(srv) {};
	
{% for r in api.routes %}\
{% for m in r.path.methods %}\
	cwapper::data ${m.operationId}(${m.signature}) {
		// Implementation goes here
{% choose %}\
{% when m.bodyparam %}\
		return ${m.bodyparam.name};
{% end %}\
{% otherwise %}\
		return cwapper::data();
{% end %}\
{% end %}\
	}
	
{% end %}\
{% end %}\
};

int main(int argc,char ** argv) {
	try {
		cppcms::service srv(argc,argv);
		srv.applications_pool().mount(cppcms::applications_factory<service>());
		srv.run();
	} catch(std::exception const &e) {
		std::cerr << e.what() << std::endl;
	}
}
