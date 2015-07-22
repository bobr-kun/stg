#ifndef __STG_SGCP_TCP_H__
#define __STG_SGCP_TCP_H__

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
 *    Author : Maxim Mamontov <faust@stargazer.dp.ua>
 */

#include "stg/sgcp_transport.h"

#include "stg/os_int.h"

#include <string>

#include <unistd.h> // ssize_t

namespace STG
{
namespace SGCP
{

class TCPProto : public TransportProto
{
    public:
        TCPProto(boost::asio::io_service& ios);
        virtual ~TCPProto();

        virtual ConnectionPtr connect(const std::string& address, uint16_t port) = 0;
        virtual void bind(const std::string& address, uint16_t port, Proto::AcceptHandler handler) = 0;

        typedef boost::asio::ip::tcp protocol;
    private:
        ba::io_service& m_ios;
        protocol::acceptor m_acceptor;

        void m_handleAccept(TCPConn* conn, Proto::AcceptHandler handler, const boost::system::error_code& ec)
};

} // namespace SGCP
} // namespace STG

#endif