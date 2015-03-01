//
//  gkIOSLog.mm
//  gkENGINE_CoreModule
//
//  Created by gameKnife on 13-5-4.
//
//

#include "gkIOSLog.h"
#include <Foundation/Foundation.h>

void IOSLog( const TCHAR* strings)
{
    NSString *string = [[NSString alloc] initWithCString:strings];
    NSLog(string);
    
    [string release];
}