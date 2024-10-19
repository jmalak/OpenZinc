//      Zinc Interface Library - Z_NUM.CPP
//      COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "i18n.hpp"
#define USE_LOC_NUMBER
#include "p_i18n.hpp"

// ----- LOC_NUMBER -----------------------------------------------------------

void ZAF_LOCALE_WINDOW::LOC_NUMBER_DataLoad(void)
{
        // Load the object data.
        SetText(DECIMAL_SYMBOL, table->decimalSeparator);
        SetText(THOUSANDS, table->thousandsSeparator);
        int grouping = *(int *)table->grouping;
        SetValue(GROUPING, grouping);
        SetText(LEFT_PAREN, table->bnumLeftParen);
        SetText(RIGHT_PAREN, table->bnumRightParen);
        SetText(DEFDIGITS, table->defDigits);
        SetText(ALTDIGITS, table->altDigits);
        SetValue(FRACTIONS_DIGIT, table->fractionDigits);
        SetValue(INT_FRACTIONS_DIGIT, table->intFractionDigits);

        // Load the default flags.
        // positive format
        SetText(POSITIVE_SIGN, table->positiveSign);
        SetValue(P_CS_PRECEDES, table->posCurrencyPrecedes);
        SetValue(P_SIGN_POSN, table->posSignPrecedes);
        SetValue(P_SEP_BY_SPACE, table->posSpaceSeparation);
        // negative format
        SetText(NEGATIVE_SIGN, table->negativeSign);
        SetValue(N_CS_PRECEDES, table->negCurrencyPrecedes);
        SetValue(N_SIGN_POSN, table->negSignPrecedes);
        SetValue(N_SEP_BY_SPACE, table->negSpaceSeparation);
}

void ZAF_LOCALE_WINDOW::LOC_NUMBER_DataStore(void)
{
        // Store the object data.
        GetText(DECIMAL_SYMBOL, table->decimalSeparator);
        GetText(THOUSANDS, table->thousandsSeparator);
        int grouping;
        GetValue(GROUPING, &grouping);
        *(int *)table->grouping = grouping;
        GetText(LEFT_PAREN, &table->bnumLeftParen);
        GetText(RIGHT_PAREN, &table->bnumRightParen);
        GetText(DEFDIGITS, &table->defDigits);
        GetText(ALTDIGITS, &table->altDigits);
        GetValue(FRACTIONS_DIGIT, &table->fractionDigits);
        GetValue(INT_FRACTIONS_DIGIT, &table->intFractionDigits);

        // Load the default flags.
        // positive format
        GetText(POSITIVE_SIGN, table->positiveSign);
        GetValue(P_CS_PRECEDES, &table->posCurrencyPrecedes);
        GetValue(P_SIGN_POSN, &table->posSignPrecedes);
        GetValue(P_SEP_BY_SPACE, &table->posSpaceSeparation);
        // negative format
        GetText(NEGATIVE_SIGN, table->negativeSign);
        GetValue(N_CS_PRECEDES, &table->negCurrencyPrecedes);
        GetValue(N_SIGN_POSN, &table->negSignPrecedes);
        GetValue(N_SEP_BY_SPACE, &table->negSpaceSeparation);
}

