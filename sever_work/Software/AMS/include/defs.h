// $Id: defs.h,v 1.2 2025/05/23 16:04:27 jianan Exp $
//  
//  defs.h
//  RichRingXR
//
//  Created by Jianan Xiao on 2025/04/28.
//

#pragma once

// #define LOCAL_TEST
#define ISISS
#define USING_PMT_CORRECTION
#define USING_REFRACTIVE_INDEX_CORRECTION
#define USING_BETA_UNIFORMITY_CORRECTION
// #define USING_RICH_N
// #define USING_RICHCHARGE_CUT
#define USING_RICHRINGR_CORRECTION

#ifdef LOCAL_TEST
#undef USING_RICHRINGR_CORRECTION
#endif

#ifndef ISISS
#undef USING_PMT_CORRECTION
#undef USING_BETA_UNIFORMITY_CORRECTION
#endif

