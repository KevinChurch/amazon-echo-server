#include "request.h"
#include "logging.h"
#include <vector>
#include <iostream>

//implementation for ParseRequest
std::unique_ptr<Request> Request::ParseRequest(const std::string& original_request) {


	std::unique_ptr<Request> req(new Request);
	req->m_original_request = original_request;
	std::string method;
	std::string version;
	std::string url;
	int count = 0;


	for (unsigned int i=0; i < original_request.size(); i++)
	{
		if (original_request[i] == '\r')
			break;

		if (original_request[i] == ' ') {

		  	if (i == 0){
		    	req->m_original_request = "";
		    	std::cerr << "400 BAD REQUEST!" << std::endl;
				ERROR << "400 BAD REQUEST!";
				method = "";
				url = "";
				version = "";
				break;
			}
		  	if (original_request[i-1] == ' '){
		    	req->m_original_request = "";
		    	std::cerr << "400 BAD REQUEST!" << std::endl;
				ERROR << "400 BAD REQUEST!";
				method = "";
				url = "";
				version = "";
				break;
			}
			count++;
		    continue;
		}

		switch(count)
		{
		case 0:
			method += original_request[i];
			break;
		case 1:
			url += original_request[i];
			break;
		case 2:
			version += original_request[i];
			break;
		}
	}

	if (method != "GET") {
		std::cerr << "Invalid method request!" << std::endl;
		ERROR << "Invalid method request!";
		req->m_original_request = "";
	}

	if (version != "HTTP/1.1") {
		std::cerr << "HTTP Version not support!" << std::endl;
		ERROR << "HTTP Version not support!";
		req->m_original_request = "";
	}

	if (url.size() < 1)
	  req->m_original_request = "";

	req->m_method = method;
	req->m_uri = url;
	req->m_version = version;
	return req;

}

std::string Request::original_request() const {
	return m_original_request;
}

std::string Request::method() const {
	return m_method;
}

std::string Request::uri() const {
	return m_uri;
}

std::string Request::version() const {
	return m_version;
}

/*	we are using headers and body in response.cc file, I will delete this comment later
using Headers = std::vector<std::pair<std::string, std::string>>;
Headers Request::headers() const {
	return m_headers;
}

std::string Request::body() const {
	return m_body;
}
*/