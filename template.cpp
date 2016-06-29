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
#include <cstdint>

#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

namespace cwapper {
	typedef std::vector<std::string> stringarray;
	typedef boost::variant<std::string, stringarray> parameter;
	typedef std::map<std::string, parameter> qstring;
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
		dispatcher().assign("/__api__", &restful::__api__, this);
	}
{% for r in api.routes %}\
	void ${r.fid}(${r.signature}) {
		CORS();
		
		if (request().request_method() == "OPTIONS") {
			return;
		}
		if (request().http_accept() != "application/json") {
			response().make_error_response(response().not_implemented);
			return;
		}
			
{% for m in r.path.methods %}\
		
		// ${m.summary}
{% if m.description %}\
		// ${m.description}
{% end %}\
		if (request().request_method() == "${m.name}") {
{% for p in m.parameters %}\
			// ${p.name}: ${p.description}
			${p.retrieve}
{% end %}\
			std::cout << "DEBUG: ${m.name} " << request().path_info()
{% for p in m.parameters %}\
				<< std::endl << "\\t${p.name}=" << ${p.surname}
{% end %}\
			<< std::endl;
			
			response().out() << ${m.operationId}(${m.call});
			return;
		}
{% end %}\
		response().make_error_response(response().not_implemented);
	}
{% end %}\
		
{% for r in api.routes %}\
{% for m in r.path.methods %}\
	virtual cppcms::json::value ${m.operationId}(${m.signature}) {
		cppcms::json::null null;
		
		return cppcms::json::value(null);
	}
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
		
	cwapper::parameter parameter(std::string name) {
		cppcms::http::request::form_type form = request().post_or_get();
		cwapper::parameter result;

		for(cppcms::http::request::form_type::iterator it = form.begin(); it != form.end(); it++) {
			std::string key = it->first, value = it->second;

			if(key != name)
				continue;

			try {
				cwapper::stringarray& array = boost::get<cwapper::stringarray>(result);
				array.push_back(value);
				continue;
			} catch(boost::bad_get e) {}

			std::string& old = boost::get<std::string>(result);
			if(old.empty()) {
				result = value;
			} else {
				cwapper::stringarray array;
				array.push_back(old);
				array.push_back(value);
				result = array;
			}
		}

		return result;
	}
	
	void CORS() {
		response().set_header("Content-Type",
			request().http_accept());
			
		response().set_header("Access-Control-Allow-Origin", "*");
		response().set_header("Access-Control-Allow-Headers",
			request().getenv("HTTP_ACCESS_CONTROL_REQUEST_HEADERS"));
		response().set_header("Access-Control-Allow-Methods",
			request().getenv("HTTP_ACCESS_CONTROL_REQUEST_METHOD"));
	}
	
	void __api__() {
		CORS();
		std::string accept = request().http_accept();
		
		if(boost::algorithm::contains(accept, "text/yaml")) {
			response().set_header("Content-Type", "text/yaml");
			response().out() << "${api.__yaml__}";
			return;
		}

		response().set_header("Content-Type", "application/json");
		response().out() << "${api.__json__}";
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
