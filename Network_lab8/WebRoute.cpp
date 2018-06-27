#include "WebRoute.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

string && WebRoute::route(const string & path)
{
	string ret = BASE_DIR + "/assets/" + path;
	// fs::path will eliminate repeated separators '/'
	ret = fs::path(ret).string();
	return std::move(ret);
}

WebRoute::WebRoute()
{
}


WebRoute::~WebRoute()
{
}
