/*
${api.description}
*/
#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>

#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>
#include <iostream>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

namespace cwapper {
	typedef std::vector<std::string> stringarray;
	typedef boost::variant<std::string, stringarray> qsparam;
	typedef std::map<std::string, qsparam> qstring;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& v) {
	if (!v.empty()) {
		std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(out, ", "));
	}
	return out;
}

class restful: public cppcms::application {
public:
	restful(cppcms::service &srv): cppcms::application(srv) {
{% for r in api.routes %}\
		dispatcher().assign("${r.re}", &restful::${r.fid}, this${r.args});
{% end %}\
	}
{% for r in api.routes %}\
	void ${r.fid}(${r.signature}) {
		cwapper::qstring query = this->query();
{% for m in r.path.methods %}\
		
		// ${m.summary}
		if (request().request_method() == "${m.name}") {
{% for p in m.parameters %}\
			${p.retrieve}
{% end %}\
			std::cout << "DEBUG: ${m.name} " << request().path_info()
{% for p in m.parameters %}\
				<< std::endl << "\\t${p.name}=" << ${p.surname}
{% end %}\
			<< std::endl;
			
			${m.operationId}(${m.call});

			return;
		}
{% end %}\
		response().make_error_response(response().not_implemented);
	}
{% end %}\
		
{% for r in api.routes %}\
{% for m in r.path.methods %}\
	virtual void ${m.operationId}(${m.signature}) {}
{% end %}\
{% end %}\

private:
	cppcms::json::value body() {
		std::pair<void*, ssize_t> post = request().raw_post_data();
		cppcms::json::value rawbody;
		std::istringstream ss(std::string((char *)post.first, post.second));
		if (rawbody.load(ss, true)) {
			std::cout << "DEBUG:\t<json body>: " << rawbody << std::endl;
		} else {
			response().make_error_response(response().not_acceptable, "Invalid JSON data");
		}
		return rawbody;
	}
	
	cwapper::qstring query() {
		std::istringstream iss(request().query_string());
		cwapper::qstring params;

		std::string keyval, key, val;

		while(std::getline(iss, keyval, '&'))
		{
			std::istringstream iss(keyval);

			if(std::getline(std::getline(iss, key, '='), val)) {
				boost::algorithm::trim(key);
				boost::algorithm::trim(val);
				
				if(boost::algorithm::ends_with(key, "[]")) {
					boost::algorithm::trim_right_if(key, boost::algorithm::is_any_of("[]"));
					try {
						cwapper::stringarray &array =
							boost::get<cwapper::stringarray>(params[key]);
						array.push_back(val);
					} catch(boost::bad_get e) {
						cwapper::stringarray init(1, val);
						params[key] = init;
					}
				} else {
					params[key] = val;
				}
			}
		}
		
		return params;
	}
};

int main(int argc,char ** argv) {
	try {
		cppcms::service srv(argc,argv);
		srv.applications_pool().mount(cppcms::applications_factory<restful>());
		srv.run();
	} catch(std::exception const &e) {
		std::cerr << e.what() << std::endl;
	}
}
