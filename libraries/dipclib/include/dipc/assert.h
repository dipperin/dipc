//
// Created by zhou.yang on 2018/11/1.
//

#pragma once

#include "exception.h"

namespace dipc{
    /**
     * @brief Dipc assertion.
     *
     */

    #define DipcAssert(A, ...) ::dipc::assertAux(A, #A, __LINE__, __FILE__, __func__, ##__VA_ARGS__)
    /**
     * @brief Assertion A equals B.
     *
     */
    #define DipcAssertEQ(A, B, ...) DipcAssert(((A)==(B)),##__VA_ARGS__)
    /**
     * @brief Assertion A not equal to B.
     */
    #define DipcAssertNE(A, B, ...) DipcAssert(((A)!=(B)), ##__VA_ARGS__)

    /**
     * @brief Dipc assertion implementation, assertion failure output failure location and error message.
     * @param cond      Assertion condition.
     * @param conndStr  Assertion prompt.
     * @param line      The line number of the code that failed the assertion.
     * @param file      Code file with assertion failure.
     * @param func      Function with assertion failure.
     * @param args      Argument list.
     */
    template<typename... Args>
    inline void assertAux(bool cond, const char *condStr, unsigned line, const char *file, const char *func, Args&&... args) {
        if (!cond) {
           dipcThrow("Assertion failed:", condStr, "func:", func, "line:", line, "file:", file, args...);
        }
    }
}

