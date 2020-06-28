/*
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *    Author : Maksym Mamontov <stg@madf.info>
 */

#ifndef SERVICES_IMPL_H
#define SERVICES_IMPL_H

#include "stg/services.h"
#include "stg/service_conf.h"
#include "stg/locker.h"
#include "stg/store.h"
#include "stg/noncopyable.h"
#include "stg/logger.h"

#include <list>
#include <map>
#include <string>

#include <pthread.h>

class ADMIN;

class SERVICES_IMPL : private NONCOPYABLE, public SERVICES {
public:
    explicit SERVICES_IMPL(STORE * st);
    virtual ~SERVICES_IMPL() {}

    int Add(const SERVICE_CONF & service, const ADMIN * admin);
    int Del(const std::string & name, const ADMIN * admin);
    int Change(const SERVICE_CONF & service, const ADMIN * admin);
    bool Find(const std::string & name, SERVICE_CONF * service) const;
    bool Find(const std::string & name, SERVICE_CONF_RES * service) const;
    bool Exists(const std::string & name) const;
    const std::string & GetStrError() const { return strError; }

    size_t Count() const { return data.size(); }

    int OpenSearch() const;
    int SearchNext(int, SERVICE_CONF * service) const;
    int CloseSearch(int) const;

private:
    SERVICES_IMPL(const SERVICES_IMPL & rvalue);
    SERVICES_IMPL & operator=(const SERVICES_IMPL & rvalue);

    typedef std::list<SERVICE_CONF>::iterator       iterator;
    typedef std::list<SERVICE_CONF>::const_iterator const_iterator;

    bool Read();

    std::list<SERVICE_CONF> data;
    STORE *                 store;
    STG_LOGGER &            WriteServLog;
    mutable std::map<int, const_iterator> searchDescriptors;
    mutable unsigned int    handle;
    mutable pthread_mutex_t mutex;
    std::string             strError;
};

#endif
