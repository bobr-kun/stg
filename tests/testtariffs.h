#ifndef __TEST_TARIFFS_H__
#define __TEST_TARIFFS_H__

#include "stg/tariffs.h"

#include "tariff_impl.h"

class TEST_TARIFFS : public TARIFFS {
    public:
        TEST_TARIFFS() {}

        int            ReadTariffs () { return 0; }
        const TARIFF * FindByName(const std::string & /*name*/) const { return &testTariff; }
        const TARIFF * GetNoTariff() const { return NULL; }
        int            Del(const std::string & /*name*/, const ADMIN * /*admin*/) { return 0; }
        int            Add(const std::string & /*name*/, const ADMIN * /*admin*/) { return 0; }
        int            Chg(const TARIFF_DATA & /*td*/, const ADMIN * /*admin*/) { return 0; }

        void AddNotifierAdd(NOTIFIER_BASE<TARIFF_DATA> *) {}
        void DelNotifierAdd(NOTIFIER_BASE<TARIFF_DATA> *) {}

        void AddNotifierDel(NOTIFIER_BASE<TARIFF_DATA> *) {}
        void DelNotifierDel(NOTIFIER_BASE<TARIFF_DATA> *) {}

        void           GetTariffsData(std::list<TARIFF_DATA> * /*tdl*/) {}

        size_t         Count() const { return 0; }

        const std::string & GetStrError() const { return strError; }

        void           SetFee(double fee);

    private:
        std::string strError;
        TARIFF_IMPL testTariff;
};

inline
void TEST_TARIFFS::SetFee(double fee)
{
    TARIFF_DATA td(testTariff.GetTariffData());
    td.tariffConf.fee = fee;
    testTariff = td;
}

#endif
