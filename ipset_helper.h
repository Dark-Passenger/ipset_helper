#include <string>

class ipset
{
    struct ipset_session *session;
    std::string name;
    bool execute(const int , const std::string &);
    bool convert_ip(const std::string&, uint8_t &, struct in_addr *);
    public:
        ipset() = delete;
        ipset(const std::string &);
        ~ipset();
        bool add_ip(const std::string &);
        bool delete_ip(const std::string &);
};
