#ifndef HTTP_REQ
#define HTTP_REQ
#include <string>
#include <vector>
#include <memory>


class Request
{
  public:
    static std::unique_ptr<Request> ParseRequest(const std::string& original_request);

    std::string original_request() const;
    std::string method() const;
    std::string uri() const;
    std::string version() const;

    using Headers = std::vector<std::pair<std::string, std::string>>;
    Headers headers() const;
    std::string body() const;

  private:
    std::string m_original_request;
    std::string m_method;
    std::string m_uri;
    std::string m_version;
    std::string m_body;
    Headers m_headers;
};


#endif

