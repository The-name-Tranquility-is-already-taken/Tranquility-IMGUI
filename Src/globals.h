#include <string>
struct dat_t {
	bool isLoggedIn = false;
	std::string token = "";
};

dat_t* globals = new dat_t();