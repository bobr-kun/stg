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
 *    Author : Boris Mikhailenko <stg34@stargazer.dp.ua>
 */

 /*
 $Revision: 1.9 $
 $Date: 2010/10/05 20:41:11 $
 $Author: faust $
 */

#ifndef TARIFF_CONF_H
#define TARIFF_CONF_H

#include <string>
#include <vector>

#include "resetable.h"
#include "const.h"

//-----------------------------------------------------------------------------
enum
{
    TRAFF_UP = 0,
    TRAFF_DOWN,
    TRAFF_UP_DOWN,
    TRAFF_MAX
};
//-----------------------------------------------------------------------------
struct DIRPRICE_DATA
{
    DIRPRICE_DATA()
        : hDay(0),
          mDay(0),
          hNight(0),
          mNight(0),
          priceDayA(0),
          priceNightA(0),
          priceDayB(0),
          priceNightB(0),
          threshold(0),
          singlePrice(0),
          noDiscount(0)
        {}
    int    hDay;
    int    mDay;
    int    hNight;
    int    mNight;
    double priceDayA;
    double priceNightA;
    double priceDayB;
    double priceNightB;
    int    threshold;
    int    singlePrice; // Do not use day/night division
    int    noDiscount; // Do not use threshold
};
//-----------------------------------------------------------------------------
struct DIRPRICE_DATA_RES
{
    DIRPRICE_DATA_RES()
        : hDay(),
          mDay(),
          hNight(),
          mNight(),
          priceDayA(),
          priceNightA(),
          priceDayB(),
          priceNightB(),
          threshold(),
          singlePrice(),
          noDiscount()
        {}

    DIRPRICE_DATA_RES & operator= (const DIRPRICE_DATA & rvalue)
        {
        hDay        = rvalue.hDay;
        mDay        = rvalue.mDay;
        hNight      = rvalue.hNight;
        mNight      = rvalue.mNight;
        priceDayA   = rvalue.priceDayA;
        priceNightA = rvalue.priceNightA;
        priceDayB   = rvalue.priceDayB;
        priceNightB = rvalue.priceNightB;
        threshold   = rvalue.threshold;
        singlePrice = rvalue.singlePrice;
        noDiscount  = rvalue.noDiscount;
        return *this;
        }

    DIRPRICE_DATA GetData()
        {
        DIRPRICE_DATA dd;
        dd.hDay        = hDay;
        dd.hNight      = hNight;
        dd.mDay        = mDay;
        dd.mNight      = mNight;
        dd.noDiscount  = noDiscount;
        dd.priceDayA   = priceDayA;
        dd.priceDayB   = priceDayB;

        dd.priceNightA = priceNightA;
        dd.priceNightB = priceNightB;
        dd.singlePrice = singlePrice;
        dd.threshold   = threshold;
        return dd;
        }

    RESETABLE<int>    hDay;
    RESETABLE<int>    mDay;
    RESETABLE<int>    hNight;
    RESETABLE<int>    mNight;
    RESETABLE<double> priceDayA;
    RESETABLE<double> priceNightA;
    RESETABLE<double> priceDayB;
    RESETABLE<double> priceNightB;
    RESETABLE<int>    threshold;
    RESETABLE<int>    singlePrice;
    RESETABLE<int>    noDiscount;
};
//-----------------------------------------------------------------------------
struct TARIFF_CONF
{
    double      fee;
    double      free;
    int         traffType;
    double      passiveCost;
    std::string name;

    TARIFF_CONF()
        : fee(0),
          free(0),
          traffType(TRAFF_UP_DOWN),
          passiveCost(0),
          name()
        {}

    TARIFF_CONF(const std::string & n)
        : fee(0),
          free(0),
          traffType(TRAFF_UP_DOWN),
          passiveCost(0),
          name(n)
        {}
};
//-----------------------------------------------------------------------------
struct TARIFF_CONF_RES
{
    TARIFF_CONF_RES()
        : fee(),
          free(),
          traffType(),
          passiveCost(),
          name()
        {}

    TARIFF_CONF_RES & operator=(const TARIFF_CONF & tc)
        {
        fee         = tc.fee;
        free        = tc.free;
        traffType   = tc.traffType;
        passiveCost = tc.passiveCost;
        name        = tc.name;
        return *this;
        }

    TARIFF_CONF GetData()
        {
        TARIFF_CONF tc;
        tc.fee         = fee;
        tc.free        = free;
        tc.name        = name;
        tc.passiveCost = passiveCost;
        tc.traffType   = traffType;
        return tc;
        }

    RESETABLE<double>      fee;
    RESETABLE<double>      free;
    RESETABLE<int>         traffType;
    RESETABLE<double>      passiveCost;
    RESETABLE<std::string> name;
};
//-----------------------------------------------------------------------------
struct TARIFF_DATA
{
    TARIFF_CONF                tariffConf;
    std::vector<DIRPRICE_DATA> dirPrice;

    TARIFF_DATA()
        : tariffConf(),
          dirPrice(DIR_NUM)
        {}

    TARIFF_DATA(const std::string & name)
        : tariffConf(name),
          dirPrice(DIR_NUM)
        {}

    TARIFF_DATA(const TARIFF_DATA & td)
        : tariffConf(td.tariffConf),
          dirPrice(td.dirPrice)
        {}

    TARIFF_DATA & operator=(const TARIFF_DATA & td)
        {
        tariffConf = td.tariffConf;
        dirPrice = td.dirPrice;
        return *this;
        }
};
//-----------------------------------------------------------------------------
struct TARIFF_DATA_RES
{
    TARIFF_CONF_RES                tariffConf;
    std::vector<DIRPRICE_DATA_RES> dirPrice;

    TARIFF_DATA_RES()
        : tariffConf(),
          dirPrice(DIR_NUM)
        {}

    TARIFF_DATA GetData()
        {
        TARIFF_DATA td;
        td.tariffConf = tariffConf.GetData();
        for (size_t i = 0; i < DIR_NUM; i++)
            td.dirPrice[i] = dirPrice[i].GetData();
        return td;
        }
};
//-----------------------------------------------------------------------------
#endif
