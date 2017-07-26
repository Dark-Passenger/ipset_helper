# ipset_helper
simple wrapper class over libipset currently only supports adding and deleting an ip from a named ipset.

The object is a non creating reference to an ipset.

RAII is used to maintain a session with the ipset library and for automatic cleanup.
