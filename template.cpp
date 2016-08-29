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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

namespace cwapper {
	typedef std::vector<std::string> stringarray;
	typedef boost::variant<std::string, stringarray> parameter;
	typedef std::map<std::string, parameter> qstring;

	enum contentType { JSON, XML };
	std::string contentTypes[] = { "application/json", "text/xml" };
	
	struct data {
		contentType type;
		boost::property_tree::ptree tree;

		data(contentType value = JSON) { type = value; };
		std::string mimeType() { return cwapper::contentTypes[this->type]; }; 
	};
	
	class error: std::exception {};
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& v) {
	if (!v.empty()) {
		std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(out, ", "));
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const cwapper::data& v) {
	if(v.type == cwapper::JSON) {
		boost::property_tree::json_parser::write_json(out, v.tree);
	}
	if(v.type == cwapper::XML) {
		boost::property_tree::xml_parser::write_xml(out, v.tree);
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
			
{% for m in r.path.methods %}\
		
		// ${m.summary}
{% if m.description %}\
		// ${m.description}
{% end %}\
		if (request().request_method() == "${m.name}") {
{% for p in m.parameters %}\
			// ${p.name}: ${p.description}
			${p.retrieve};
			
{% end %}\
			std::cout << "DEBUG <method>: ${m.name} " << request().path_info()
{% for p in m.parameters %}\
				<< std::endl << "\\t${p.name}=" << ${p.surname}
{% end %}\
			<< std::endl;
			
			cwapper::data result = ${m.operationId}(${m.call});
			
			response().set_header("Content-Type", result.mimeType());
			response().out() << result;
			return;
		}
{% end %}\
		response().make_error_response(response().not_implemented);
	}
{% end %}\

// ---------------------------------------------------------------------
		
{% for r in api.routes %}\
{% for m in r.path.methods %}\
	virtual cwapper::data ${m.operationId}(${m.signature}) {
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

private:
	cwapper::data body() {
		std::pair<void*, ssize_t> post = request().raw_post_data();
		std::istringstream ss(std::string((char *)post.first, post.second));

		std::cerr << "DEBUG <body>:\t " << request().content_type() << std::endl;

		boost::property_tree::ptree tree;
		cwapper::data data;
	
		if (request().content_type() == "application/json") {
			data.type = cwapper::JSON;
			try {
				boost::property_tree::json_parser::read_json(ss, tree);
			} catch (boost::property_tree::json_parser::json_parser_error &e) {
				response().make_error_response(response().bad_request, e.what());
				throw cwapper::error();
			}
		}
		if (request().content_type() == "text/xml" ||
			request().content_type() == "application/xml" ) {
			data.type = cwapper::XML;
			try {
				boost::property_tree::xml_parser::read_xml(ss, tree);
			} catch (boost::property_tree::xml_parser::xml_parser_error &e) {
				response().make_error_response(response().bad_request, e.what());
				throw cwapper::error();
			}
		}
		data.tree = tree;
		return data;
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
