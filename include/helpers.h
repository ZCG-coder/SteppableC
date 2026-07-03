#pragma once

/*
 * Adapted from boost/current_function.hpp - BOOST_CURRENT_FUNCTION
 *
 * Copyright 2002-2018 Peter Dimov
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *  See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt
 *
 *  http://www.boost.org/libs/assert
 */

#if defined(STP_DISABLE_CURRENT_FUNCTION)

    #define STP_CURRENT_FUNCTION "(unknown)"

#elif defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
    defined(__ghs__) || defined(__clang__)

    #define STP_CURRENT_FUNCTION __PRETTY_FUNCTION__

#elif defined(__DMC__) && (__DMC__ >= 0x810)

    #define STP_CURRENT_FUNCTION __PRETTY_FUNCTION__

#elif defined(__FUNCSIG__)

    #define STP_CURRENT_FUNCTION __FUNCSIG__

#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))

    #define STP_CURRENT_FUNCTION __FUNCTION__

#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)

    #define STP_CURRENT_FUNCTION __FUNC__

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)

    #define STP_CURRENT_FUNCTION __func__

#elif defined(__cplusplus) && (__cplusplus >= 201103)

    #define STP_CURRENT_FUNCTION __func__

#else

    #define STP_CURRENT_FUNCTION "(unknown)"

#endif
