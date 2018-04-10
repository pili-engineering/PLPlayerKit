//
//  MMMaterialDesignSpinner.h
//  Pods
//
//  Created by Michael Maxwell on 12/28/14.
//
//

#import <UIKit/UIKit.h>
#import "ActivityTracking.h"

//! Project version number for MMMaterialDesignSpinner.
FOUNDATION_EXPORT double MMMaterialDesignSpinnerVersionNumber;

//! Project version string for MMMaterialDesignSpinner.
FOUNDATION_EXPORT const unsigned char MMMaterialDesignSpinnerVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <Cent/PublicHeader.h>

/**
 *  A control similar to iOS' UIActivityIndicatorView modeled after Google's Material Design Activity spinner.
 */
@interface MMMaterialDesignSpinner : UIView <ActivityTracking>

@end
