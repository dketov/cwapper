#ifndef __CWAPPER_HPP
#define __CWAPPER_HPP

#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_mapper.h>
#include <cppcms/url_dispatcher.h>

#include <iostream>
#include <cstdint>

#include <boost/algorithm/string.hpp>

#include <map>
#include <vector>

#include <boost/variant.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

namespace cwapper {
	typedef std::vector<std::string> stringarray;
	typedef boost::variant<std::string, stringarray> parameter;
	typedef std::map<std::string, parameter> qstring;

	enum contentType { JSON, XML };
	
	struct data {
		contentType type;
		boost::property_tree::ptree tree;

		data(contentType value = JSON) { type = value; };
		std::string mimeType() const { 
			std::string contentTypes[] = { "application/json", "text/xml" };

			return contentTypes[this->type];
		}; 
	};
	
	class error: std::exception {};
}

#endif
