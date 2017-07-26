#include <arpa/inet.h>
#include <libipset/types.h>
#include <libipset/session.h>
#include <stdexcept>
#include "ipset_helper.h"

ipset::ipset(const std::string &ipset_name) : name(ipset_name)
{
    ipset_load_types();

    session = ipset_session_init(printf);
    if (!session)
    {
        throw std::runtime_error("Cannot initialize ipset");
    }

    /* return success on attempting to add an existing / remove an
     * non-existing rule */
    ipset_envopt_parse(session, IPSET_ENV_EXIST, nullptr);
}

ipset::~ipset()
{
    ipset_session_fini(session);
}

bool convert_ip(const std::string& ip, uint8_t &family, struct in_addr *addr)
{
    int retval = inet_pton(NFPROTO_IPV4, ip.c_str(), addr);
    if(retval == 1)
    {
       family = NFPROTO_IPV4;
    }
    else
    {
        retval = inet_pton(NFPROTO_IPV6, ip.c_str(), addr);
        if(retval == 1)
        {
            family = NFPROTO_IPV6;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool ipset::execute(const int cmd, const std::string &ip)
{
    int r = ipset_session_data_set(session, IPSET_SETNAME, name.c_str());
    /* since the IPSET_SETNAME option is valid, this should never fail */
    if(r != 0)
    {
        std::runtime_error("ipset name setting failed");
    }

    const struct ipset_type *type = ipset_type_get(session, static_cast<enum ipset_cmd>(cmd));
    if (!type)
    {
        /* possible reasons for failure: set name does not exist */
        return false;
    }

    uint8_t family;
    struct in_addr *addr = nullptr;

    if(!convert_ip(ip, family, addr))
    {
        return false;
    }

    ipset_session_data_set(session, IPSET_OPT_FAMILY, &family);
    ipset_session_data_set(session, IPSET_OPT_IP, addr);

    /*
     * future scope for setting timeout periods on ipset.
    if (timeout)
        ipset_session_data_set(session, IPSET_OPT_TIMEOUT, &timeout);
        */

    r = ipset_cmd(session, static_cast<enum ipset_cmd>(cmd), /*lineno*/ 0);
    /* assume that errors always occur if NOT in set. To do it otherwise,
     * see lib/session.c for IPSET_CMD_TEST in ipset_cmd */
    if(r != 0)
    {
        return false;
    }

    return true;

}

bool ipset::add_ip(const std::string &ip)
{
    return execute(IPSET_CMD_ADD, ip);
}

bool ipset::delete_ip(const std::string &ip)
{
    return execute(IPSET_CMD_DEL, ip);
}
