//      Zinc Interface Library - Z_CURR.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_LOC_CURRENCY
#include "p_i18n.hpp"

// ----- LOC_CURRENCY -------------------------------------------------------

void ZAF_LOCALE_WINDOW::LOC_CURRENCY_DataLoad(void)
{
        // Load the object data.
        SetText(SYMBOL, table->currencySymbol);
        SetText(INTL_SYMBOL, table->intCurrencySymbol);
        SetText(DECIMAL_SYMBOL, table->monDecimalSeparator);
        SetText(THOUSANDS, table->monThousandsSeparator);
        int grouping = (int)*table->monGrouping;
        SetValue(GROUPING, grouping);
}

void ZAF_LOCALE_WINDOW::LOC_CURRENCY_DataStore(void)
{
        // Store the object data.
        GetText(SYMBOL, table->currencySymbol);
        GetText(INTL_SYMBOL, table->intCurrencySymbol);
        GetText(DECIMAL_SYMBOL, table->monDecimalSeparator);
        GetText(THOUSANDS, table->monThousandsSeparator);
        int grouping;
        GetValue(GROUPING, &grouping);
        *table->monGrouping = (char)grouping;
}

