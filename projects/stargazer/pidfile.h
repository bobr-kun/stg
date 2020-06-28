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

/*
 *  Header file for RAII pid-file writer
 */

/*
 $Revision: 1.2 $
 $Date: 2009/06/09 09:07:32 $
 $Author: faust $
 */

#ifndef __PID_FILE_H__
#define __PID_FILE_H__

#include <string>

class PIDFile {
public:
    explicit PIDFile(const std::string & fn);
    ~PIDFile();
private:
    std::string fileName;
};

#endif
